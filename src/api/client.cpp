#include <api/client.h>

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <QVariantMap>
#include <iostream>

namespace http = core::net::http;
namespace net = core::net;

using namespace api;
using namespace std;

Client::Client(Config::Ptr config) :
    config_(config), cancelled_(false) {
}

void Client::get(const net::Uri::Path &path,
                 const net::Uri::QueryParameters &parameters, QJsonDocument &root) {
    // Create a new HTTP client
    auto client = http::make_client();

    // Start building the request configuration
    http::Request::Configuration configuration;

    // Build the URI from its components
    net::Uri uri = net::make_uri(config_->apiroot, path, parameters);
    configuration.uri = client->uri_to_string(uri);

    // Give out a user agent string
    configuration.header.add("User-Agent", config_->user_agent);

    // Build a HTTP request object from our configuration
    auto request = client->head(configuration);

    try {
        // Synchronously make the HTTP request
        // We bind the cancellable callback to #progress_report
        auto response = request->execute(
                    bind(&Client::progress_report, this, placeholders::_1));

        // Check that we got a sensible HTTP status code
        if (response.status != http::Status::ok) {
            throw domain_error(response.body);
        }
        // Parse the JSON from the response
        root = QJsonDocument::fromJson(response.body.c_str());
    } catch (net::Error &) {
        //TODO: handle net error
    }
}

Client::FilmRes Client::query_films(const string& query, int querytype) {
    QJsonDocument root;

    /** Build a URI and get the contents */
    if(querytype == 0){
        get( { "search", "movie"}, { { "query", query }, { "api_key", config_->api_key } }, root);
        /** <root>/search/movie?query=<query>&api_key=<api_key>&language=it */
    }else if(querytype == 1){
        get( { "discover", "movie"}, { { "api_key", config_->api_key } }, root);
        /** <root>/discover/movie?api_key=4149363c46a16a04a1d48ad3098197b0 */
    }else if(querytype == 2){
        get( { "movie", "upcoming"}, {{ "api_key", config_->api_key } }, root);
        /** <root>/movie/upcoming?api_key=<api_key> */
    }

    // declare a list of films
    FilmRes result;

    QVariantMap variant = root.toVariant().toMap();
    for (const QVariant &i : variant["results"].toList()) {
        QJsonDocument info;

        QVariantMap item = i.toMap();

        //convert image urls
        std::string imgtmp1 = "http://image.tmdb.org/t/p/w300" + item["backdrop_path"].toString().toStdString();
        std::string imgtmp2 = "http://image.tmdb.org/t/p/w154" + item["poster_path"].toString().toStdString();

        //get additional film info TODO:optimize
        //get( { "movie", item["id"].toString().toStdString()}, { { "api_key", config_->api_key }, { "append_to_response", "trailers" } }, info);
        /** <root>/movie/<filmid>?api_key=<api_key>&append_to_response=trailers */
        //QVariantMap infoitem = info.toVariant().toMap();
        //QVariantMap traileritem;
        //if(querytype != 2) //WHY DO I NEED THIS!?!?!
        //    traileritem = infoitem["trailers"].toMap()["youtube"].toList().first().toMap();
        //end additional film info

        // We add each result to our list
        result.films.emplace_back(
            Film {
                imgtmp1,
                item["id"].toUInt(),
                item["original_title"].toString().toStdString(),
                item["release_date"].toString().toStdString(),
                imgtmp2,
                item["popularity"].toDouble(),
                item["title"].toString().toStdString(),
                item["vote_average"].toDouble(),
                item["vote_count"].toUInt(),
                "infoitem[].toString().toStdString()",
                "https://www.youtube.com/watch?v=",// + traileritem["source"].toString().toStdString(),
                "infoitem[].toString().toStdString()",
                "infoitem[].toString().toStdString()"
            }
        );
    }
    return result;
}


http::Request::Progress::Next Client::progress_report(
        const http::Request::Progress&) {

    return cancelled_ ?
                http::Request::Progress::Next::abort_operation :
                http::Request::Progress::Next::continue_operation;
}

void Client::cancel() {
    cancelled_ = true;
}

Config::Ptr Client::config() {
    return config_;
}
