#include <boost/algorithm/string/trim.hpp>

#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/FilterBase.h>
#include <unity/scopes/FilterOption.h>
#include <unity/scopes/FilterState.h>
#include <unity/scopes/OptionSelectorFilter.h>
#include <unity/scopes/Department.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/CannedQuery.h>
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

        Client::FilmRes filmslist, filmslist2, actorslist;
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

        //filters definition
        sc::Filters filters;
        sc::OptionSelectorFilter::SPtr optionsFilter = sc::OptionSelectorFilter::create("category", s_homepage);
        optionsFilter->set_display_hints(1);
        optionsFilter->add_option("movie", "Movies");
        optionsFilter->add_option("tv", "TV-series");
        optionsFilter->active_options(query.filter_state());
        filters.push_back(optionsFilter);
        reply->push(filters, query.filter_state());

        std::string filterid;
        if (optionsFilter->has_active_option(query.filter_state())){
            // Get a set of active filters(1 element only)
            auto o = *(optionsFilter->active_options(query.filter_state()).begin());
            filterid = o->id();
        }
        //set filter according to settings
        if(filterid == "" && s_homepage == "Movies") filterid = "movie";
        else if(filterid == "" && s_homepage == "TV-series") filterid = "tv";

        //departments definition (done by hand for performance reasons)
        sc::Department::SPtr all_depts = sc::Department::create("", query, "All genres");
        if(filterid == "movie"){
            sc::Department::SPtr dept1 = sc::Department::create("28", query, "Action"); all_depts->add_subdepartment(dept1);
            sc::Department::SPtr dept2 = sc::Department::create("12", query, "Adventure"); all_depts->add_subdepartment(dept2);
            sc::Department::SPtr dept3 = sc::Department::create("16", query, "Animation"); all_depts->add_subdepartment(dept3);
            sc::Department::SPtr dept4 = sc::Department::create("35", query, "Comedy"); all_depts->add_subdepartment(dept4);
            sc::Department::SPtr dept5 = sc::Department::create("80", query, "Crime"); all_depts->add_subdepartment(dept5);
            sc::Department::SPtr dept6 = sc::Department::create("105", query, "Disaster"); all_depts->add_subdepartment(dept6);
            sc::Department::SPtr dept7 = sc::Department::create("99", query, "Documentary"); all_depts->add_subdepartment(dept7);
            sc::Department::SPtr dept8 = sc::Department::create("18", query, "Drama"); all_depts->add_subdepartment(dept8);
            sc::Department::SPtr dept9 = sc::Department::create("82", query, "Eastern"); all_depts->add_subdepartment(dept9);
            sc::Department::SPtr dept10 = sc::Department::create("2916", query, "Erotic"); all_depts->add_subdepartment(dept10);
            sc::Department::SPtr dept11 = sc::Department::create("10751", query, "Family"); all_depts->add_subdepartment(dept11);
            sc::Department::SPtr dept12 = sc::Department::create("10750", query, "Fan Film"); all_depts->add_subdepartment(dept12);
            sc::Department::SPtr dept13 = sc::Department::create("14", query, "Fantasy"); all_depts->add_subdepartment(dept13);
            sc::Department::SPtr dept14 = sc::Department::create("10753", query, "Film Noir"); all_depts->add_subdepartment(dept14);
            sc::Department::SPtr dept15 = sc::Department::create("10769", query, "Foreign"); all_depts->add_subdepartment(dept15);
            sc::Department::SPtr dept16 = sc::Department::create("36", query, "History"); all_depts->add_subdepartment(dept16);
            sc::Department::SPtr dept17 = sc::Department::create("10595", query, "Holiday"); all_depts->add_subdepartment(dept17);
            sc::Department::SPtr dept18 = sc::Department::create("27", query, "Horror"); all_depts->add_subdepartment(dept18);
            sc::Department::SPtr dept19 = sc::Department::create("10756", query, "Indie"); all_depts->add_subdepartment(dept19);
            sc::Department::SPtr dept20 = sc::Department::create("10402", query, "Music"); all_depts->add_subdepartment(dept20);
            sc::Department::SPtr dept21 = sc::Department::create("22", query, "Musical"); all_depts->add_subdepartment(dept21);
            sc::Department::SPtr dept22 = sc::Department::create("9648", query, "Mystery"); all_depts->add_subdepartment(dept22);
            sc::Department::SPtr dept23 = sc::Department::create("10754", query, "Neo-Noir"); all_depts->add_subdepartment(dept23);
            sc::Department::SPtr dept24 = sc::Department::create("1115", query, "Road Movie"); all_depts->add_subdepartment(dept24);
            sc::Department::SPtr dept25 = sc::Department::create("10749", query, "Romance"); all_depts->add_subdepartment(dept25);
            sc::Department::SPtr dept26 = sc::Department::create("878", query, "Science Fiction"); all_depts->add_subdepartment(dept26);
            sc::Department::SPtr dept27 = sc::Department::create("10755", query, "Short"); all_depts->add_subdepartment(dept27);
            sc::Department::SPtr dept28 = sc::Department::create("10758", query, "Sporting Event"); all_depts->add_subdepartment(dept28);
            sc::Department::SPtr dept29 = sc::Department::create("10757", query, "Sports Film"); all_depts->add_subdepartment(dept29);
            sc::Department::SPtr dept30 = sc::Department::create("10748", query, "Suspense"); all_depts->add_subdepartment(dept30);
            sc::Department::SPtr dept31 = sc::Department::create("10770", query, "TV Movie"); all_depts->add_subdepartment(dept31);
            sc::Department::SPtr dept32 = sc::Department::create("53", query, "Thriller"); all_depts->add_subdepartment(dept32);
            sc::Department::SPtr dept33 = sc::Department::create("10752", query, "War"); all_depts->add_subdepartment(dept33);
            sc::Department::SPtr dept34 = sc::Department::create("37", query, "Western"); all_depts->add_subdepartment(dept34);
            reply->register_departments(all_depts);
        }else{
            sc::Department::SPtr dept1 = sc::Department::create("10759", query, "Action & Adventure"); all_depts->add_subdepartment(dept1);
            sc::Department::SPtr dept2 = sc::Department::create("16", query, "Animation"); all_depts->add_subdepartment(dept2);
            sc::Department::SPtr dept3 = sc::Department::create("35", query, "Comedy"); all_depts->add_subdepartment(dept3);
            sc::Department::SPtr dept4 = sc::Department::create("99", query, "Documentary"); all_depts->add_subdepartment(dept4);
            sc::Department::SPtr dept5 = sc::Department::create("18", query, "Drama"); all_depts->add_subdepartment(dept5);
            sc::Department::SPtr dept6 = sc::Department::create("10761", query, "Education"); all_depts->add_subdepartment(dept6);
            sc::Department::SPtr dept9 = sc::Department::create("10751", query, "Family"); all_depts->add_subdepartment(dept9);
            sc::Department::SPtr dept10 = sc::Department::create("10762", query, "Kids"); all_depts->add_subdepartment(dept10);
            sc::Department::SPtr dept11 = sc::Department::create("9648", query, "Mystery"); all_depts->add_subdepartment(dept11);
            sc::Department::SPtr dept12 = sc::Department::create("10763", query, "News"); all_depts->add_subdepartment(dept12);
            sc::Department::SPtr dept13 = sc::Department::create("10764", query, "Reality"); all_depts->add_subdepartment(dept13);
            sc::Department::SPtr dept14 = sc::Department::create("10765", query, "Sci-Fi & Fantasy"); all_depts->add_subdepartment(dept14);
            sc::Department::SPtr dept15 = sc::Department::create("10766", query, "Soap"); all_depts->add_subdepartment(dept15);
            sc::Department::SPtr dept16 = sc::Department::create("10767", query, "Talk"); all_depts->add_subdepartment(dept16);
            sc::Department::SPtr dept17 = sc::Department::create("10768", query, "War & Politics"); all_depts->add_subdepartment(dept17);
            sc::Department::SPtr dept18 = sc::Department::create("37", query, "Western"); all_depts->add_subdepartment(dept18);
            reply->register_departments(all_depts);
        }

        bool query_isempty = query_string.empty();
        if (query_isempty) {    // If the string is empty show default
            std::string strquery, strcatname;
            if(filterid=="movie"){
                strquery = "upcoming";
                strcatname = "Coming soon";
            } else {
                strquery = "airing_today";
                strcatname = "Airing today on TV";
            }

            filmslist = client_.query_films(filterid, strquery, 1, query.department_id(), s_language);
            filmslist2 = client_.query_films(filterid, strquery,  2, query.department_id(), s_language);

            auto films_cat = reply->register_category("topvoted", "Featured", "",
                sc::CategoryRenderer(POPULARFILMS_TEMPLATE));
            auto films_cat2 = reply->register_category("comingsoon", strcatname, "",
                sc::CategoryRenderer(RECENTFILMS_TEMPLATE));
            // register_category(arbitrary category id, header title, header icon, template)

            for (const auto &flm : filmslist.films) {
                sc::CategorisedResult res(films_cat);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title);
                res.set_title(flm.title);
                // Set the rest of the attributes, art, artist, etc.
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["movie_or_tv"] = filterid;
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                if(out.str() == "0")
                    res["ratings"] = "☆ " + out.str();
                else
                    res["ratings"] = "★ " + out.str();
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
                res["movie_or_tv"] = filterid;
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                if(out.str() == "0")
                    res["ratings"] = "☆ " + out.str();
                else
                    res["ratings"] = "★ " + out.str();
                res["backdrop"] = flm.backdrop_path;


                // Push the result
                if (!reply->push(res)) {
                    // If we fail to push, it means the query has been cancelled.
                    return;
                }
            }
        } else {
            // otherwise, use the query string
            cerr << query.department_id() << endl;
            filmslist = client_.query_films("movie", query_string, 0, query.department_id(), s_language);
            filmslist2 = client_.query_films("tv", query_string, 0, query.department_id(), s_language);
            actorslist = client_.query_films("person", query_string, 0, query.department_id(), s_language);
            auto films_cat = reply->register_category("searchmovie", "Movies", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            auto films_cat2 = reply->register_category("searchtv", "TV series", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            auto actors_cat = reply->register_category("searchact", "Actors", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            //print film list
            for (const auto &flm : filmslist.films) {
                sc::CategorisedResult res(films_cat);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title);
                res.set_title(flm.title);
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["movie_or_tv"] = "movie";
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                if(out.str() == "0")
                    res["ratings"] = "☆ " + out.str();
                else
                    res["ratings"] = "★ " + out.str();
                res["backdrop"] = flm.backdrop_path;

                if (!reply->push(res))
                    return;
            }
            //print tv shows list
            for (const auto &flm : filmslist2.films) {
                sc::CategorisedResult res(films_cat2);
                res.set_uri("google.com");
                res.set_title(flm.title);
                res.set_art(flm.poster_path);
                res["id"] = std::to_string(flm.id);
                res["movie_or_tv"] = "tv";
                res["lang"] = s_language;
                //set precision 2 to ratings
                std::ostringstream out;
                out << std::setprecision(2) << flm.vote_average;
                if(out.str() == "0")
                    res["ratings"] = "☆ " + out.str();
                else
                    res["ratings"] = "★ " + out.str();
                res["backdrop"] = flm.backdrop_path;

                if (!reply->push(res))
                    return;
            }
            //print actors list
            for (const auto &act : actorslist.films) {
                sc::CategorisedResult res(actors_cat);
                res.set_uri("google.com");
                res.set_title(act.title);
                res.set_art(act.poster_path); //profile_path
                res["id"] = std::to_string(act.id);
                res["movie_or_tv"] = "person";
                res["lang"] = s_language;

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

void Query::initScope() //init settings
{
    unity::scopes::VariantMap config = settings();
    if (config.empty())
        cerr << "CONFIG EMPTY!" << endl;

    s_location = config["location"].get_string();

    int home = config["homepage"].get_int();
    s_homepage = (home==0) ? "Movies" : "TV-series";

    int lang = config["language"].get_int();
    if(lang == 0) s_language = "en";        //don't trust switch he's a bad guy
    else if(lang == 1) s_language = "it";
    else if(lang == 2) s_language = "de";
    else if(lang == 3) s_language = "fr";
}
