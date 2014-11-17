#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>
#include <unity/scopes/Variant.h>
#include <QVariantMap>

#include <iostream>
#include <cmath>

namespace sc = unity::scopes;

using namespace std;
using namespace api;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata,
                 Config::Ptr config) :
    sc::PreviewQueryBase(result, metadata), client_(config) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {    
    sc::Result result = PreviewQueryBase::result();


    //additional film info
    QJsonDocument root;
    int j=0;
    std::string ytsource = "", movie_or_tv = result["movie_or_tv"].get_string(), append,
            runtime, genres, networkstr, in_production, usr1str, usr2str, tagline, overview;
    bool isMovie = (movie_or_tv == "movie") ? true : false;
    bool isActor = (movie_or_tv == "person") ? true : false;
    //movie and tv specific query strings
    QString trail1, trail2, trail3, networks;
    if(isMovie && !isActor){trail1="trailers";trail2="youtube";trail3="source"; append="trailers,reviews";
        networks="production_companies";networkstr="Production companies: ";}
    else if(!isMovie && !isActor) {trail1="videos",trail2="results",trail3="key";append="videos";networks="networks";networkstr="Networks: ";}
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
        }if(j!=0) {networkstr.pop_back();networkstr.pop_back();} else networkstr+="Unknown";
        //calculate runtime and other specific things
        int hours, min;
        if(isMovie){ //movie info
            min = infoitem["runtime"].toInt();
            if(min == 0) runtime = "unknown";
            else {
                hours = floor(min/60); min = min - 60*hours;
                runtime = to_string(hours) + "h " + to_string(min) + "m";}
            //other
            usr1str = infoitem["budget"].toString().toStdString();
            if(usr1str.empty() || usr1str == "0")
                usr1str = "Budget: unknown";
            else usr1str = "Budget: " + usr1str;
            usr2str = infoitem["revenue"].toString().toStdString();
            if(usr2str.empty() || usr2str == "0")
                usr2str = "Revenue: unknown";
            else usr2str = "Revenue: " + usr2str;
        } else { //tv show info
            in_production = (infoitem["in_production"].toString().toStdString() == "true") ? "✔" : "no";
            //other
            usr1str = infoitem["number_of_seasons"].toString().toStdString();
            if(usr1str.empty())
                usr1str = "Number of seasons: unknown";
            else usr1str = "Number of seasons: " + usr1str;
            usr2str = infoitem["number_of_episodes"].toString().toStdString();
            if(usr2str.empty())
                usr2str = "Number of episodes: unknown";
            else usr2str = "Number of episodes: " + usr2str;
        }
    } else { //actor info
        overview = infoitem["biography"].toString().toStdString();

    }


    sc::ColumnLayout layout1col(1), layout2col(2);
    // Single column layout
    if(!isActor)
    layout1col.add_column( { "headerId", "videoId", "ratingId", "genresId", "reldateId", "expId",
                             "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id",
                             "summaryId", "actionsId", "revtitleId", "reviewsId"});
    else layout1col.add_column( {"headerId","imageId", "galleryId", "summaryId"});

    // Two column layout
    if(!isActor){
    layout2col.add_column( { "videoId", "ratingId", "genresId", "reldateId", "expId",
                             "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id"});
    layout2col.add_column( { "headerId", "summaryId", "actionsId", "revtitleId", "reviewsId" });
    }else{layout2col.add_column( {"imageId", "galleryId" }); layout2col.add_column( {"headerId", "summaryId" });}

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
    w_summary.add_attribute_value("title", sc::Variant(tagline));
    w_summary.add_attribute_value("text", sc::Variant(overview));


//movie and tv shows sections
    if(isActor == false){
        sc::VariantBuilder rat_builder;
        sc::VariantBuilder rev_builder;
        w_video.add_attribute_value("source", sc::Variant(ytsource));
        w_video.add_attribute_mapping("screenshot", "backdrop");
        w_genres.add_attribute_value("text", sc::Variant("Genres: " + genres));
        rat_builder.add_tuple({{"rating", sc::Variant(floor(infoitem["vote_average"].toFloat()+0.5)/2)}});
        w_rat.add_attribute_value("reviews", rat_builder.end());
        w_status.add_attribute_value("text", sc::Variant("Status: "+infoitem["status"].toString().toStdString()));
        w_expandable.add_attribute_value("title", sc::Variant("Info"));
        w_expandable.add_attribute_value("collapsed-widgets", sc::Variant(2));
        w_networks.add_attribute_value("text", sc::Variant(networkstr));
        w_usr1.add_attribute_value("text", sc::Variant(usr1str));
        w_usr2.add_attribute_value("text", sc::Variant(usr2str));
//movie specific sections
        if(isMovie){
            w_reldate.add_attribute_value("text", sc::Variant("Release date: "+infoitem["release_date"].toString().toStdString()));
            w_runtime.add_attribute_value("text", sc::Variant("Runtime: "+ runtime));
            w_revtitle.add_attribute_value("title", sc::Variant("Reviews"));
            QVariantMap item, inforeviews = infoitem["reviews"].toMap();
            for (const QVariant &i : inforeviews["results"].toList()) {
                item = i.toMap();
                rev_builder.add_tuple({{"author", sc::Variant(item["author"].toString().toStdString())},{"review", sc::Variant(item["content"].toString().toStdString())}});
            }
            if(!item.isEmpty()) w_reviews.add_attribute_value("reviews", rev_builder.end());
            else w_revtitle.add_attribute_value("text", sc::Variant("No reviews avaiable."));
//tv show specific sections
        } else {
            w_reldate.add_attribute_value("text", sc::Variant("First air date: "+infoitem["first_air_date"].toString().toStdString()));
            w_runtime.add_attribute_value("text", sc::Variant("In production: "+ in_production));

        }
        w_expandable.add_widget(w_genres);
        w_expandable.add_widget(w_status);
        w_expandable.add_widget(w_reldate);
        w_expandable.add_widget(w_runtime);
        w_expandable.add_widget(w_networks);
        w_expandable.add_widget(w_usr1);
        w_expandable.add_widget(w_usr2);

//actor specific sections
    }else if(isActor){
        w_image.add_attribute_mapping("source", "art");
        sc::VariantArray imgarr;
        QVariantMap item, inforeviews = infoitem["movie_credits"].toMap();
        for (const QVariant &i : inforeviews["cast"].toList()) {
            item = i.toMap(); std::string tmp = item["poster_path"].toString().toStdString();
            if(!tmp.empty())
                imgarr.push_back(sc::Variant("http://image.tmdb.org/t/p/w300" + tmp));
        }
        w_gallery.add_attribute_value("sources", sc::Variant(imgarr));
        w_summary.add_attribute_value("title", sc::Variant("Biography"));
    }


    // Define the actions section
    std::string homestr = infoitem["homepage"].toString().toStdString();
    sc::VariantBuilder act_builder;
    if(isMovie)
        act_builder.add_tuple({
            {"id", sc::Variant("oncinema")},
            {"label", sc::Variant("Find cinema")},
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
            {"label", sc::Variant("Google it!")},
            {"uri", sc::Variant("https://www.google.it/?q="+tmptitle+"#q="+tmptitle)}
        });
    w_actions.add_attribute_value("actions", act_builder.end());

    // Push each of the sections
    reply->push( { w_video, w_image, w_rat, w_expandable,  w_gallery,
                   w_header, w_summary, w_actions, w_revtitle, w_reviews });
}
