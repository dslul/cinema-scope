#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>
#include <unity/scopes/Variant.h>
#include <QVariantMap>

#include <iostream>

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
    QJsonDocument root;
    std::string ytsource;
    //additional film info
    client_.get( { "movie", result["id"].get_string()}, { { "api_key", client_.config_->moviedb_key }, { "append_to_response", "trailers" }, {"language", result["lang"].get_string()} }, root, client_.config_->moviedbroot);
    /** <root>/movie/<filmid>?api_key=<api_key>&append_to_response=trailers */
    QVariantMap infoitem = root.toVariant().toMap();
    QVariant trailers = infoitem["trailers"].toMap()["youtube"];
    QVariantMap traileritem;
    if(trailers.toList().isEmpty())//ugliest workaround ever seen
        ytsource = "";
    else{
        traileritem = trailers.toList().first().toMap();
        ytsource = "https://www.youtube.com/watch?v=" + traileritem["source"].toString().toStdString();
    }
    //end additional film info

    sc::ColumnLayout layout1col(1), layout2col(2);

    // Single column layout
    layout1col.add_column( { "headerId", "videoId", "summaryId", "actionsId"});

    // Two column layout
    layout2col.add_column( { "videoId" });
    layout2col.add_column( { "headerId", "summaryId", "actionsId" });

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col });

    // Define the header section
    sc::PreviewWidget w_header("headerId", "header");
    // It has title and a subtitle properties
    w_header.add_attribute_mapping("title", "title");

    // Define the image section
    //sc::PreviewWidget w_art("imageId", "image");
    //w_art.add_attribute_mapping("source", "art");

    //video section
    sc::PreviewWidget w_video("videoId", "video");
    w_video.add_attribute_value("source", sc::Variant(ytsource));
    w_video.add_attribute_mapping("screenshot", "backdrop");

    //define the summary (storyline) section
    sc::PreviewWidget w_summary("summaryId", "text");
    w_summary.add_attribute_value("title", sc::Variant(infoitem["tagline"].toString().toStdString()));
    w_summary.add_attribute_value("text", sc::Variant(infoitem["overview"].toString().toStdString()));

    // Define the actions section
    sc::PreviewWidget w_actions("actionsId", "actions");
    sc::VariantBuilder builder;
    builder.add_tuple({
        {"id", sc::Variant("open")},
        {"label", sc::Variant("Find cinema")},
        {"uri", result["uri"]}
    });
    w_actions.add_attribute_value("actions", builder.end());

    // Push each of the sections
    reply->push( { w_video, w_header, w_summary, w_actions });
}
