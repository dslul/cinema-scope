#include <boost/algorithm/string/trim.hpp>

#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

#include <iomanip>
#include <sstream>

namespace sc = unity::scopes;
namespace alg = boost::algorithm;

using namespace std;
using namespace api;
using namespace scope;


//http://developer.ubuntu.com/api/scopes/sdk-14.10/unity.scopes.CategoryRenderer/
const static string POPULARFILMS_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "carousel",
                "card-layout": "vertical",
                "card-size": "large"
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art"
                },
                "subtitle": "artist"
            }
        }
    )";
const static string RECENTFILMS_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "grid",
                "card-layout": "vertical",
                "card-size": "small",
                "collapsed-rows": "2",
                "overlay" : "true"
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art"
                },
                "subtitle": "artist"
            }
        }
    )";

void Query::run(sc::SearchReplyProxy const& reply) {
    try {
        // Start by getting information about the query
        const sc::CannedQuery &query(sc::SearchQueryBase::query());
        //remove whitespaces
        string query_string = alg::trim_copy(query.query_string());

        Client::FilmRes filmslist, filmslist2;


        sc::Department::SPtr all_depts = sc::Department::create("", query, "Movies");
        sc::Department::SPtr dept = sc::Department::create(
                                "channel->id()", query, "TV-Series");
        all_depts->add_subdepartment(dept);
        reply->register_departments(all_depts);


        if (query_string.empty()) {
            // If the string is empty show default
            filmslist = client_.query_films("", 1);
            filmslist2 = client_.query_films("", 2);
        } else {
            // otherwise, use the query string
            filmslist = client_.query_films(query_string, 0);
        }
        // Register a category for tracks
        auto films_cat = reply->register_category("topvoted", "Featured", "",
            sc::CategoryRenderer(POPULARFILMS_TEMPLATE));
        auto films_cat2 = reply->register_category("comingsoon", "Coming soon", "",
            sc::CategoryRenderer(RECENTFILMS_TEMPLATE));
        // register_category(arbitrary category id, header title, header icon, template)

        for (const auto &flm : filmslist.films) {
            sc::CategorisedResult res(films_cat);
            res.set_uri("http://media.w3.org/2010/05/sintel/trailer.mp4"); //TODO: tempurl
            res.set_title(flm.title);
            // Set the rest of the attributes, art, artist, etc.
            res.set_art(flm.poster_path);
            res["id"] = std::to_string(flm.id);
            res["backdrop"] = flm.backdrop_path;
            res["tagline"] = flm.tagline;
            res["overview"] = flm.overview;
            res["youtubeurl"] = flm.youtubeurl;


            // Push the result
            if (!reply->push(res)) {
                // If we fail to push, it means the query has been cancelled.
                return;
            }
        }
        for (const auto &flm : filmslist2.films) {
            sc::CategorisedResult res(films_cat2);
            res.set_uri("http://media.w3.org/2010/05/sintel/trailer.mp4"); //TODO: tempurl
            res.set_title(flm.title);
            // Set the rest of the attributes, art, artist, etc.
            res.set_art(flm.poster_path);
            res["backdrop"] = flm.backdrop_path;
            res["tagline"] = flm.tagline;
            res["overview"] = flm.overview;
            res["youtubeurl"] = flm.youtubeurl;


            // Push the result
            if (!reply->push(res)) {
                // If we fail to push, it means the query has been cancelled.
                return;
            }
        }
    } catch (domain_error &e) {
        // Handle exceptions being thrown by the client API
        cerr << e.what() << endl;
        reply->error(current_exception());
    }
}


Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata,
             Config::Ptr config) :
    sc::SearchQueryBase(query, metadata), client_(config) {
}

void Query::cancelled() {
    client_.cancel();
}

