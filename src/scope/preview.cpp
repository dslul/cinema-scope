#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>

#include <iostream>

namespace sc = unity::scopes;

using namespace std;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    sc::Result result = PreviewQueryBase::result();

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

    //TODO video section
    sc::PreviewWidget w_video("videoId", "video");
    w_video.add_attribute_mapping("source", "youtubeurl");
    w_video.add_attribute_mapping("screenshot", "backdrop");

    //define the summary (storyline) section
    sc::PreviewWidget w_summary("summaryId", "text");
    w_summary.add_attribute_mapping("title", "tagline");
    w_summary.add_attribute_mapping("text", "overview");

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
