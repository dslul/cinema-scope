#include <boost/algorithm/string/trim.hpp>

#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/SearchMetadata.h>

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
                "card-size": "large",
                "overlay": true
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art"
                },
                "subtitle": "ratings"
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
                "card-size": "medium",
                "overlay": true,
                "collapsed-rows": "2"
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art"
                },
                "subtitle": "ratings"
            }
        }
    )";
const static string SEARCHFILM_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "grid",
                "card-layout": "horizontal",
                "card-size": "medium"
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art"
                },
                "subtitle": "ratings"
            }
        }
    )";

void Query::run(sc::SearchReplyProxy const& reply) {
    initScope();
    try {
        // Start by getting information about the query
        const sc::CannedQuery &query(sc::SearchQueryBase::query());
        //remove whitespaces
        string query_string = alg::trim_copy(query.query_string());

        Client::FilmRes filmslist, filmslist2;
        //store location
        std::string place;
        if(s_location == ""){ //if location config is null retrieve from gps
            auto metadata = search_metadata();
            if (metadata.has_location()) {
                auto location = metadata.location();
                if (location.has_city()) {
                    place = location.city();
                }
            }
            if (place.empty() || place == "None") { //fallback
                place = "London";
            }
        }else
            place = s_location;

        sc::Department::SPtr all_depts = sc::Department::create("", query, "Movies");
        sc::Department::SPtr dept = sc::Department::create(
                                "channel->id()", query, "TV-Series");
        all_depts->add_subdepartment(dept);
        reply->register_departments(all_depts);

        bool query_isempty = query_string.empty();
        if (query_isempty) {
            // If the string is empty show default
            filmslist = client_.query_films("", 1, s_language);
            filmslist2 = client_.query_films("", 2, s_language);
            // Register a category for tracks
            auto films_cat = reply->register_category("topvoted", "Featured", "",
                sc::CategoryRenderer(POPULARFILMS_TEMPLATE));
            auto films_cat2 = reply->register_category("comingsoon", "Coming soon", "",
                sc::CategoryRenderer(RECENTFILMS_TEMPLATE));
            // register_category(arbitrary category id, header title, header icon, template)

            for (const auto &flm : filmslist.films) {
                sc::CategorisedResult res(films_cat);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title);
                res.set_title(flm.title);
                // Set the rest of the attributes, art, artist, etc.
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                res["ratings"] = "☆ " + out.str();
                res["backdrop"] = flm.backdrop_path;


                // Push the result
                if (!reply->push(res)) {
                    // If we fail to push, it means the query has been cancelled.
                    return;
                }
            }

            for (const auto &flm : filmslist2.films) {
                sc::CategorisedResult res(films_cat2);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title);
                res.set_title(flm.title);
                // Set the rest of the attributes, art, artist, etc.
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                res["ratings"] = "☆ " + out.str();
                res["backdrop"] = flm.backdrop_path;


                // Push the result
                if (!reply->push(res)) {
                    // If we fail to push, it means the query has been cancelled.
                    return;
                }
            }
        } else {
            // otherwise, use the query string
            filmslist = client_.query_films(query_string, 0, s_language);
            auto films_cat = reply->register_category("search", "", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            for (const auto &flm : filmslist.films) {
                sc::CategorisedResult res(films_cat);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title);
                res.set_title(flm.title);
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                res["ratings"] = "☆ " + out.str();
                res["backdrop"] = flm.backdrop_path;

                if (!reply->push(res))
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

void Query::initScope()
{
    unity::scopes::VariantMap config = settings();  // The settings method is provided by the base class
    if (config.empty())
        cerr << "CONFIG EMPTY!" << endl;

    s_location = config["location"].get_string();     // Prints "London" unless the user changed the value
    cerr << "location: " << s_location << endl;

    int tmp = config["language"].get_int();
    if(tmp == 0) s_language = "en";        //don't trust switch he's a bad guy
    else if(tmp == 1) s_language = "it";
    else if(tmp == 2) s_language = "de";
    else if(tmp == 3) s_language = "fr";

    cerr << tmp << endl;
    cerr << "language: " << s_language << endl;
}
