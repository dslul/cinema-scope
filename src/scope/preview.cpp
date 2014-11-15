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
            runtime, genres, networkstr, in_production, usr1str, usr2str;
    bool isMovie = (movie_or_tv == "movie") ? true : false;
    //movie and tv specific query strings
    QString trail1, trail2, trail3, networks;
    if(isMovie){trail1="trailers";trail2="youtube";trail3="source"; append="trailers";
        networks="production_companies";networkstr="Production companies: ";}
    else {trail1="videos",trail2="results",trail3="key";append="videos";networks="networks";networkstr="Networks: ";}
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
    if(isMovie){
        min = infoitem["runtime"].toInt();
    hours = floor(min/60); min = min - 60*hours;
    runtime = to_string(hours) + "h " + to_string(min) + "m";
    //other
    usr1str = infoitem["budget"].toString().toStdString();
    if(usr1str.empty() || usr1str == "0")
        usr1str = "Budget: unknown";
    else usr1str = "Budget: " + usr1str;
    usr2str = infoitem["revenue"].toString().toStdString();
    if(usr2str.empty() || usr2str == "0")
        usr2str = "Revenue: unknown";
    else usr2str = "Revenue: " + usr2str;
    } else{
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
    //end additional film info



    sc::ColumnLayout layout1col(1), layout2col(2);

    // Single column layout
    layout1col.add_column( { "headerId", "videoId", "ratingId", "genresId", "reldateId",
                             "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id",
                             "summaryId", "actionsId"});

    // Two column layout
    layout2col.add_column( { "videoId", "ratingId", "genresId", "reldateId",
                             "statusId", "runtimeId", "networksId", "usr1Id", "usr2Id"});
    layout2col.add_column( { "headerId", "summaryId", "actionsId" });

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col });

    // Define the header section
    sc::PreviewWidget w_header("headerId", "header");
    // It has title and a subtitle properties
    w_header.add_attribute_mapping("title", "title");

    //video section
    sc::PreviewWidget w_video("videoId", "video");
    w_video.add_attribute_value("source", sc::Variant(ytsource));
    w_video.add_attribute_mapping("screenshot", "backdrop");

    //rating section
    sc::PreviewWidget w_rat("ratingId", "reviews");
    sc::VariantBuilder rat_builder;
    rat_builder.add_tuple({{"rating", sc::Variant(floor(infoitem["vote_average"].toFloat()+0.5)/2)}});
    w_rat.add_attribute_value("reviews", rat_builder.end());



    //FIXME:
    sc::PreviewWidget w_genres("genresId", "text");
    w_genres.add_attribute_value("title", sc::Variant("Info"));
    w_genres.add_attribute_value("text", sc::Variant("Genres: " + genres));

    sc::PreviewWidget w_reldate("reldateId", "text");
    if(isMovie)
        w_reldate.add_attribute_value("text", sc::Variant("Release date: "+infoitem["release_date"].toString().toStdString()));
    else w_reldate.add_attribute_value("text", sc::Variant("First air date: "+infoitem["first_air_date"].toString().toStdString()));

    sc::PreviewWidget w_status("statusId", "text");
    w_status.add_attribute_value("text", sc::Variant("Status: "+infoitem["status"].toString().toStdString()));

    sc::PreviewWidget w_runtime("runtimeId", "text");
    if(isMovie)
        w_runtime.add_attribute_value("text", sc::Variant("Runtime: "+ runtime));
    else w_runtime.add_attribute_value("text", sc::Variant("In production: "+ in_production));

    sc::PreviewWidget w_networks("networksId", "text");
    w_networks.add_attribute_value("text", sc::Variant(networkstr));

    sc::PreviewWidget w_usr1("usr1Id", "text"); //budget OR number of seasons
    w_usr1.add_attribute_value("text", sc::Variant(usr1str));

    sc::PreviewWidget w_usr2("usr2Id", "text"); //revenue OR number of episodes
    w_usr2.add_attribute_value("text", sc::Variant(usr2str));

    //define the summary (storyline) section
    sc::PreviewWidget w_summary("summaryId", "text");
    w_summary.add_attribute_value("title", sc::Variant(infoitem["tagline"].toString().toStdString()));
    w_summary.add_attribute_value("text", sc::Variant(infoitem["overview"].toString().toStdString()));



    // Define the actions section
    sc::PreviewWidget w_actions("actionsId", "actions");
    sc::VariantBuilder act_builder;
    act_builder.add_tuple({
        {"id", sc::Variant("oncinema")},
        {"label", sc::Variant("Find cinema")},
        {"uri", result["uri"]}
    });
    act_builder.add_tuple({
        {"id", sc::Variant("homepg")},
        {"label", sc::Variant("Homepage")},
        {"uri", sc::Variant(infoitem["homepage"].toString().toStdString())}
    });
    w_actions.add_attribute_value("actions", act_builder.end());

    // Push each of the sections
    reply->push( { w_video, w_rat, w_genres, w_reldate, w_status, w_runtime, w_networks, w_usr1, w_usr2,
                   w_header, w_summary, w_actions });
}
