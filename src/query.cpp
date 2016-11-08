//#include <boost/algorithm/string/trim.hpp>

#include <localization.h>
#include <query.h>

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
#include <iostream>
#include <sstream>

namespace sc = unity::scopes;
//namespace alg = boost::algorithm;

using namespace std;


//http://developer.ubuntu.com/api/scopes/sdk-14.10/unity.scopes.CategoryRenderer/
const static string POPULARFILMS_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "carousel",
                "card-layout": "vertical",
                "card-size": "large",
                "overlay": false
            },
            "components": {
                "title": "title",
                "art" : {
                    "field": "art",
        "aspect-ratio": ".65"

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
                    "field": "art",
        "aspect-ratio": ".65"

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
                    "field": "art",
        "aspect-ratio": ".65"

                },
                "subtitle": "ratings"
            }
        }
    )";

void Query::run(sc::SearchReplyProxy const& reply) {
    initScope();
    try {
        // Start by getting information about the query
        sc::CannedQuery query(sc::SearchQueryBase::query());
        //remove whitespaces
        string query_string = query.query_string();
        //string query_string = alg::trim_copy(query.query_string());

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
        optionsFilter->add_option("movie", _("Movies"));
        optionsFilter->add_option("tv", _("TV-series"));
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
            sc::Department::SPtr dept1 = sc::Department::create("28", query, _("Action")); all_depts->add_subdepartment(dept1);
            sc::Department::SPtr dept2 = sc::Department::create("12", query, _("Adventure")); all_depts->add_subdepartment(dept2);
            sc::Department::SPtr dept3 = sc::Department::create("16", query, _("Animation")); all_depts->add_subdepartment(dept3);
            sc::Department::SPtr dept4 = sc::Department::create("35", query, _("Comedy")); all_depts->add_subdepartment(dept4);
            sc::Department::SPtr dept5 = sc::Department::create("80", query, _("Crime")); all_depts->add_subdepartment(dept5);
            sc::Department::SPtr dept6 = sc::Department::create("105", query, _("Disaster")); all_depts->add_subdepartment(dept6);
            sc::Department::SPtr dept7 = sc::Department::create("99", query, _("Documentary")); all_depts->add_subdepartment(dept7);
            sc::Department::SPtr dept8 = sc::Department::create("18", query, _("Drama")); all_depts->add_subdepartment(dept8);
            sc::Department::SPtr dept9 = sc::Department::create("82", query, _("Eastern")); all_depts->add_subdepartment(dept9);
            sc::Department::SPtr dept10 = sc::Department::create("2916", query, _("Erotic")); all_depts->add_subdepartment(dept10);
            sc::Department::SPtr dept11 = sc::Department::create("10751", query, _("Family")); all_depts->add_subdepartment(dept11);
            sc::Department::SPtr dept12 = sc::Department::create("10750", query, _("Fan Film")); all_depts->add_subdepartment(dept12);
            sc::Department::SPtr dept13 = sc::Department::create("14", query, _("Fantasy")); all_depts->add_subdepartment(dept13);
            sc::Department::SPtr dept14 = sc::Department::create("10753", query, _("Film Noir")); all_depts->add_subdepartment(dept14);
            sc::Department::SPtr dept15 = sc::Department::create("10769", query, _("Foreign")); all_depts->add_subdepartment(dept15);
            sc::Department::SPtr dept16 = sc::Department::create("36", query, _("History")); all_depts->add_subdepartment(dept16);
            sc::Department::SPtr dept17 = sc::Department::create("10595", query, _("Holiday")); all_depts->add_subdepartment(dept17);
            sc::Department::SPtr dept18 = sc::Department::create("27", query, _("Horror")); all_depts->add_subdepartment(dept18);
            sc::Department::SPtr dept19 = sc::Department::create("10756", query, _("Indie")); all_depts->add_subdepartment(dept19);
            sc::Department::SPtr dept20 = sc::Department::create("10402", query, _("Music")); all_depts->add_subdepartment(dept20);
            sc::Department::SPtr dept21 = sc::Department::create("22", query, _("Musical")); all_depts->add_subdepartment(dept21);
            sc::Department::SPtr dept22 = sc::Department::create("9648", query, _("Mystery")); all_depts->add_subdepartment(dept22);
            sc::Department::SPtr dept23 = sc::Department::create("10754", query, _("Neo-Noir")); all_depts->add_subdepartment(dept23);
            sc::Department::SPtr dept24 = sc::Department::create("1115", query, _("Road Movie")); all_depts->add_subdepartment(dept24);
            sc::Department::SPtr dept25 = sc::Department::create("10749", query, _("Romance")); all_depts->add_subdepartment(dept25);
            sc::Department::SPtr dept26 = sc::Department::create("878", query, _("Science Fiction")); all_depts->add_subdepartment(dept26);
            sc::Department::SPtr dept27 = sc::Department::create("10755", query, _("Short")); all_depts->add_subdepartment(dept27);
            sc::Department::SPtr dept28 = sc::Department::create("10758", query, _("Sporting Event")); all_depts->add_subdepartment(dept28);
            sc::Department::SPtr dept29 = sc::Department::create("10757", query, _("Sports Film")); all_depts->add_subdepartment(dept29);
            sc::Department::SPtr dept30 = sc::Department::create("10748", query, _("Suspense")); all_depts->add_subdepartment(dept30);
            sc::Department::SPtr dept31 = sc::Department::create("10770", query, _("TV Movie")); all_depts->add_subdepartment(dept31);
            sc::Department::SPtr dept32 = sc::Department::create("53", query, _("Thriller")); all_depts->add_subdepartment(dept32);
            sc::Department::SPtr dept33 = sc::Department::create("10752", query, _("War")); all_depts->add_subdepartment(dept33);
            sc::Department::SPtr dept34 = sc::Department::create("37", query, _("Western")); all_depts->add_subdepartment(dept34);
            reply->register_departments(all_depts);
        }else{
            sc::Department::SPtr dept1 = sc::Department::create("10759", query, _("Action & Adventure")); all_depts->add_subdepartment(dept1);
            sc::Department::SPtr dept2 = sc::Department::create("16", query, _("Animation")); all_depts->add_subdepartment(dept2);
            sc::Department::SPtr dept3 = sc::Department::create("35", query, _("Comedy")); all_depts->add_subdepartment(dept3);
            sc::Department::SPtr dept4 = sc::Department::create("99", query, _("Documentary")); all_depts->add_subdepartment(dept4);
            sc::Department::SPtr dept5 = sc::Department::create("18", query, _("Drama")); all_depts->add_subdepartment(dept5);
            sc::Department::SPtr dept6 = sc::Department::create("10761", query, _("Education")); all_depts->add_subdepartment(dept6);
            sc::Department::SPtr dept9 = sc::Department::create("10751", query, _("Family")); all_depts->add_subdepartment(dept9);
            sc::Department::SPtr dept10 = sc::Department::create("10762", query, _("Kids")); all_depts->add_subdepartment(dept10);
            sc::Department::SPtr dept11 = sc::Department::create("9648", query, _("Mystery")); all_depts->add_subdepartment(dept11);
            sc::Department::SPtr dept12 = sc::Department::create("10763", query, _("News")); all_depts->add_subdepartment(dept12);
            sc::Department::SPtr dept13 = sc::Department::create("10764", query, _("Reality")); all_depts->add_subdepartment(dept13);
            sc::Department::SPtr dept14 = sc::Department::create("10765", query, _("Sci-Fi & Fantasy")); all_depts->add_subdepartment(dept14);
            sc::Department::SPtr dept15 = sc::Department::create("10766", query, _("Soap")); all_depts->add_subdepartment(dept15);
            sc::Department::SPtr dept16 = sc::Department::create("10767", query, _("Talk")); all_depts->add_subdepartment(dept16);
            sc::Department::SPtr dept17 = sc::Department::create("10768", query, _("War & Politics")); all_depts->add_subdepartment(dept17);
            sc::Department::SPtr dept18 = sc::Department::create("37", query, _("Western")); all_depts->add_subdepartment(dept18);
            reply->register_departments(all_depts);
        }

        bool query_isempty = query_string.empty();
        if (query_isempty) {    // If the string is empty show default
            std::string depid = query.department_id(), strquery, strcatname, pagenum = "1";
            int firstcat_id = 1, secondcat_id = 2;
            if(filterid=="movie" && depid == ""){
                strquery = "upcoming";
                strcatname = _("Coming soon");
            } else if(filterid=="tv" && depid == ""){
                strquery = "airing_today";
                strcatname = _("Airing today on TV");
            } else if(filterid=="movie" && depid != ""){
                strcatname = _("Popular movies");
                secondcat_id = 1;
                pagenum = "2";
            } else if(filterid=="tv" && depid != ""){
                strcatname = _("Popular TV shows");
                secondcat_id = 1;
                pagenum = "2";
            }
            filmslist = client_.query_films(filterid, strquery, firstcat_id, depid, "1", s_language);
            filmslist2 = client_.query_films(filterid, strquery, secondcat_id, depid, pagenum, s_language);

            auto films_cat = reply->register_category("topvoted", _("Featured"), "",
                sc::CategoryRenderer(POPULARFILMS_TEMPLATE));
            auto films_cat2 = reply->register_category("comingsoon", strcatname, "",
                sc::CategoryRenderer(RECENTFILMS_TEMPLATE));
            // register_category(arbitrary category id, header title, header icon, template)

            for (const auto &flm : filmslist.films) {
                sc::CategorisedResult res(films_cat);
                res.set_uri("http://www.google.com/movies?near=" + place + "&q=" + flm.title + "&view=map");
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
            filmslist = client_.query_films("movie", query_string, 0, "", "", s_language);
            filmslist2 = client_.query_films("tv", query_string, 0, "", "", s_language);
            actorslist = client_.query_films("person", query_string, 0, "", "", s_language);
            auto films_cat = reply->register_category("searchmovie", "Movies", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            auto films_cat2 = reply->register_category("searchtv", "TV series", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            auto actors_cat = reply->register_category("searchact", "Actors", "",
                sc::CategoryRenderer(SEARCHFILM_TEMPLATE));
            //print searched film list
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
            //print searched tv shows list
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
            //print searched actors list
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

    s_location = settings().at("location").get_string();

    int home = settings().at("homepage").get_int();
    s_homepage = (home==0) ? "Movies" : "TV-series";

    int lang = settings().at("language").get_int();
    if(lang == 0) s_language = "en";
    else if(lang == 1) s_language = "it";
    else if(lang == 2) s_language = "de";
    else if(lang == 3) s_language = "fr";
}
