#include <preview.h>
#include <localization.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>
#include <unity/scopes/Variant.h>
#include <QVariantMap>
#include <QDateTime>
#include <qjsondocument.h>

#include <iostream>
#include <cmath>

namespace sc = unity::scopes;

using namespace std;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata,
                 Config::Ptr config) :
    sc::PreviewQueryBase(result, metadata), client_(config) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    sc::Result result = PreviewQueryBase::result();
    //TODO: CLEAN UP ALL THIS MESS
    //don't judge me I was young

    //additional film info
    QJsonDocument root;
    int j=0;
    std::string ytsource = "", movie_or_tv = result["movie_or_tv"].get_string(), append,
            runtime, genres, networkstr, caststr, director, in_production, usr1str, usr2str,
            tagline, overview, imdburi, release_date, airdate;
    QDate filmdate, seriesdate;
    bool isMovie = (movie_or_tv == "movie") ? true : false;
    bool isActor = (movie_or_tv == "person") ? true : false;
    //movie and tv specific query strings
    QString trail1, trail2, trail3, networks;
    if(isMovie && !isActor){trail1="trailers";trail2="youtube";trail3="source"; append="trailers,reviews,credits";
        networks="production_companies";networkstr="<b>Production companies:</b> ";}
    else if(!isMovie && !isActor) {trail1="videos",trail2="results",trail3="key";append="videos,credits";networks="networks";networkstr="<b>Networks:</b> ";}
    else append="movie_credits";
    client_.get({result["movie_or_tv"].get_string(), result["id"].get_string()}, { { "api_key", client_.config_->moviedb_key }, { "append_to_response", append }, {"language", result["lang"].get_string()} }, root, client_.config_->moviedbroot);
    /** <root>/movie_or_tv/<filmid>?api_key=<api_key>&append_to_response=trailers */
    //get trailer video url
    QVariantMap infoitem = root.toVariant().toMap();
    QVariant trailers = infoitem[trail1].toMap()[trail2];
    if(!(trailers.toList().isEmpty())){
        QVariantMap traileritem;
        traileritem = trailers.toList().first().toMap();
        ytsource = "https://www.youtube.com/watch?v=" + traileritem[trail3].toString().toStdString();
    }
    if(!isActor){
        //get tagline and overview
        tagline = infoitem["tagline"].toString().toStdString();
        overview = infoitem["overview"].toString().toStdString();
        //retrieve all the genres
        for (const QVariant &i : infoitem["genres"].toList()) {
            QVariantMap item = i.toMap();
            genres += item["name"].toString().toStdString() + ", ";
        }if(!genres.empty()) {genres.pop_back();genres.pop_back();}
        //retrieve all the networks or production companies
        for (const QVariant &i : infoitem[networks].toList()) {
            QVariantMap item = i.toMap();
            networkstr += item["name"].toString().toStdString() + ", ";
            j++;
        }if(j!=0) {networkstr.pop_back();networkstr.pop_back();j=0;} else networkstr+="Unknown";
        //retrieve cast information
        QVariantMap infocast = infoitem["credits"].toMap();
        for (const QVariant &i : infocast["cast"].toList()) {
            QVariantMap item = i.toMap();
            caststr += item["name"].toString().toStdString() + ", ";
            j++;if(j==8) break;
        }if(j!=0) {caststr.pop_back();caststr.pop_back();j=0;} else caststr+="Unknown";
        for (const QVariant &i : infocast["crew"].toList()) {
            QVariantMap item = i.toMap();
            if(item["job"].toString().toStdString() == "Director"){
                director = item["name"].toString().toStdString();
                break;
            }
            j++;
        }if(director == "") director = "Unknown";
        //calculate runtime and other specific things
        int hours, min;
        if(isMovie){ //movie info            
            //adjust release date
            QString Qrelease_date = infoitem["release_date"].toString();
            filmdate = QDate::fromString(Qrelease_date, "yyyy-MM-dd");
            release_date = filmdate.toString(Qt::SystemLocaleLongDate).toStdString();
            min = infoitem["runtime"].toInt();
            if(min == 0) runtime = "unknown";
            else {
                hours = floor(min/60); min = min - 60*hours;
                runtime = to_string(hours) + "h " + to_string(min) + "m";}
            //other
            usr1str = infoitem["budget"].toString().toStdString();
            if(usr1str.empty() || usr1str == "0")
                usr1str = "<b>Budget:</b> unknown";
            else usr1str = "<b>Budget:</b> " + usr1str;
            usr2str = infoitem["revenue"].toString().toStdString();
            if(usr2str.empty() || usr2str == "0")
                usr2str = "<b>Revenue:</b> unknown";
            else usr2str = "<b>Revenue:</b> " + usr2str;
            imdburi = "http://www.imdb.com/title/" + infoitem["imdb_id"].toString().toStdString();
        } else { //tv show info            
            //adjust first air date
            QString Qrelease_date = infoitem["first_air_date"].toString();
            seriesdate = QDate::fromString(Qrelease_date, "yyyy-MM-dd");
            airdate = seriesdate.toString(Qt::SystemLocaleLongDate).toStdString();

            in_production = (infoitem["in_production"].toString().toStdString() == "true") ? "✔" : "no";
            //other
            usr1str = infoitem["number_of_seasons"].toString().toStdString();
            if(usr1str.empty())
                usr1str = "<b>Number of seasons:</b> unknown";
            else usr1str = "<b>Number of seasons:</b> " + usr1str;
            usr2str = infoitem["number_of_episodes"].toString().toStdString();
            if(usr2str.empty())
                usr2str = "<b>Number of episodes:</b> unknown";
            else usr2str = "<b>Number of episodes:</b> " + usr2str;
        }

    } else { //actor info
        overview = infoitem["biography"].toString().toStdString();        
        imdburi = "http://www.imdb.com/name/" + infoitem["imdb_id"].toString().toStdString();
    }

    sc::ColumnLayout layout1col(1), layout2col(2);
    // Single column layout
    if(!isActor)
        layout1col.add_column( { "headerId", "videoId", "ratingId", "genresId", "castId", "directorId",
                                 "reldateId", "expId",
                                 "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id",
                                 "summaryId", "actionsId", "revtitleId", "reviewsId"});
    else
        layout1col.add_column( {"headerId","imageId", "galleryId", "summaryId", "actionsId" });

    // Two column layout
    if(!isActor){
        layout2col.add_column( { "videoId", "ratingId", "genresId", "castId", "directorId", "reldateId",
                                 "expId",
                                 "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id"});
        layout2col.add_column( { "headerId", "summaryId", "actionsId", "revtitleId", "reviewsId" });
    } else {
        layout2col.add_column( {"imageId", "galleryId"}); layout2col.add_column( {"headerId", "summaryId", "actionsId" });}

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col });

    // sections declaration
    sc::PreviewWidget w_header("headerId", "header");
    sc::PreviewWidget w_video("videoId", "video");
    sc::PreviewWidget w_image("imageId", "image");
    sc::PreviewWidget w_gallery("galleryId", "gallery");
    sc::PreviewWidget w_expandable("expId", "expandable");
    sc::PreviewWidget w_rat("ratingId", "reviews");
    sc::PreviewWidget w_genres("genresId", "text");
    sc::PreviewWidget w_cast("castId", "text");
    sc::PreviewWidget w_director("directorId", "text");
    sc::PreviewWidget w_reldate("reldateId", "text");
    sc::PreviewWidget w_status("statusId", "text");
    sc::PreviewWidget w_runtime("runtimeId", "text");
    sc::PreviewWidget w_networks("networksId", "text");
    sc::PreviewWidget w_summary("summaryId", "text");
    sc::PreviewWidget w_usr1("usr1Id", "text");                //budget OR number of seasons
    sc::PreviewWidget w_usr2("usr2Id", "text");                //revenue OR number of episodes
    sc::PreviewWidget w_revtitle("revtitleId", "text");
    sc::PreviewWidget w_actions("actionsId", "actions");
    sc::PreviewWidget w_reviews("reviewsId", "reviews");

//general sections
    w_header.add_attribute_mapping("title", "title");
    if(tagline.empty())
        w_summary.add_attribute_value("title", sc::Variant("<b>Summary</b>"));
    else
        w_summary.add_attribute_value("title", sc::Variant("<i>"+tagline+"</i>"));
    w_summary.add_attribute_value("text", sc::Variant(overview));


    //MOVIES AND TV SHOWS sections
    if(isActor == false) {
        sc::VariantBuilder rat_builder;
        sc::VariantBuilder rev_builder;
        w_video.add_attribute_value("source", sc::Variant(ytsource));
        w_video.add_attribute_mapping("screenshot", "backdrop");
        w_genres.add_attribute_value("text", sc::Variant("<b>Genres:</b> " + genres));
        w_cast.add_attribute_value("text", sc::Variant("<b>Cast:</b> " + caststr));
        w_director.add_attribute_value("text", sc::Variant("<b>Director:</b> " + director));
        rat_builder.add_tuple({{"rating", sc::Variant(floor(infoitem["vote_average"].toFloat()+0.5)/2)}});
        w_rat.add_attribute_value("reviews", rat_builder.end());
        w_status.add_attribute_value("text", sc::Variant("<b>Status:</b> "+infoitem["status"].toString().toStdString()));
        w_expandable.add_attribute_value("title", sc::Variant("Info"));
        w_expandable.add_attribute_value("collapsed-widgets", sc::Variant(2));
        w_networks.add_attribute_value("text", sc::Variant(networkstr));
        w_usr1.add_attribute_value("text", sc::Variant(usr1str));
        w_usr2.add_attribute_value("text", sc::Variant(usr2str));
        //MOVIES specific sections
        if(isMovie){
            w_reldate.add_attribute_value("text", sc::Variant("<b>Release date:</b> "+release_date));
            w_runtime.add_attribute_value("text", sc::Variant("<b>Runtime:</b> "+ runtime));
            w_revtitle.add_attribute_value("title", sc::Variant("<b>Reviews</b>"));
            QVariantMap item, inforeviews = infoitem["reviews"].toMap();
            for (const QVariant &i : inforeviews["results"].toList()) {
                item = i.toMap();
                rev_builder.add_tuple({{"author", sc::Variant("<b>"+item["author"].toString().toStdString()+"</b>")},{"review", sc::Variant(item["content"].toString().toStdString())}});
            }
            if(!item.isEmpty()) w_reviews.add_attribute_value("reviews", rev_builder.end());
            else w_revtitle.add_attribute_value("text", sc::Variant(_("No reviews avaiable.")));
        //TV SHOWS specific sections
        } else {
            w_reldate.add_attribute_value("text", sc::Variant("<b>First air date</b>: "+airdate));
            w_runtime.add_attribute_value("text", sc::Variant("<b>In production</b>: "+ in_production));

        }
        w_expandable.add_widget(w_genres);
        w_expandable.add_widget(w_cast);
        w_expandable.add_widget(w_director);
        w_expandable.add_widget(w_status);
        w_expandable.add_widget(w_reldate);
        w_expandable.add_widget(w_runtime);
        w_expandable.add_widget(w_networks);
        w_expandable.add_widget(w_usr1);
        w_expandable.add_widget(w_usr2);

    //actor specific sections
    } else if(isActor) {
        w_image.add_attribute_mapping("source", "art");
        sc::VariantArray imgarr;
        QVariantMap item, inforeviews = infoitem["movie_credits"].toMap();
        for (const QVariant &i : inforeviews["cast"].toList()) {
            item = i.toMap();
            std::string tmp = item["poster_path"].toString().toStdString();
            if(!tmp.empty())
                imgarr.push_back(sc::Variant("http://image.tmdb.org/t/p/w154" + tmp));
        }
        w_gallery.add_attribute_value("sources", sc::Variant(imgarr));
        w_summary.add_attribute_value("title", sc::Variant("<b>Biography</b>"));
    }


    // Define the actions section
    std::string homestr = infoitem["homepage"].toString().toStdString();
    sc::VariantBuilder act_builder;
    if(isMovie && (QDate::currentDate() > filmdate && QDate::currentDate().year()-filmdate.year()<=1))
        act_builder.add_tuple({
            {"id", sc::Variant("oncinema")},
            {"label", sc::Variant(_("Find cinema"))},
            {"uri", result["uri"]}
        });
    if(homestr != "")
        act_builder.add_tuple({
            {"id", sc::Variant("homepg")},
            {"label", sc::Variant("Homepage")},
            {"uri", sc::Variant(homestr)}
        });
    std::string tmptitle = result["title"].get_string();
    act_builder.add_tuple({
        {"id", sc::Variant("googleit")},
        {"label", sc::Variant(_("Google it!"))},
        {"uri", sc::Variant("https://www.google.it/?q="+tmptitle)}
    });
    if(imdburi != "" && imdburi != "http://www.imdb.com/title/" && imdburi != "http://www.imdb.com/name/")
        act_builder.add_tuple({
            {"id", sc::Variant("imdb")},
            {"label", sc::Variant("IMDb")},
            {"uri", sc::Variant(imdburi)}
        });
    w_actions.add_attribute_value("actions", act_builder.end());

    // Push each of the sections
    if(isActor) {
        reply->push( { w_header, w_image,  w_gallery,
                   w_summary, w_actions });
    } else {
        reply->push( { w_header, w_video, w_rat, w_expandable,
                   w_summary, w_actions, w_revtitle, w_reviews });
    }
}
