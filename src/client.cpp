#include <client.h>

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <QVariantMap>
#include <iostream>
#include <iomanip>

namespace http = core::net::http;
namespace net = core::net;

using namespace std;

Client::Client(Config::Ptr config) :
    config_(config), cancelled_(false) {
}

void Client::get(const net::Uri::Path &path, const net::Uri::QueryParameters &parameters,
                 QJsonDocument &root, std::string &apiroot) {
    // Create a new HTTP client
    auto client = http::make_client();

    // Start building the request configuration
    http::Request::Configuration configuration;

    // Build the URI from its components
    net::Uri uri = net::make_uri(apiroot, path, parameters);
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
    }
}

Client::FilmRes Client::query_films(const string& movie_or_tv, const string& query,
                                    int querytype, const string& department,
                                    const string& pagenum, const string& lang) {
    QJsonDocument root;

    /** Build a URI and get the contents */
    if(querytype == 0){        //search
        get( { "search", movie_or_tv}, {{"query", query}, {"api_key", config_->moviedb_key}, {"search_type", "ngram"}, {"language", lang}}, root, config_->moviedbroot);
        /** <root>/search/movie?query=<query>&api_key=<api_key>&language=it */
    }else if(querytype == 1){  //featured category
        get( { "discover", movie_or_tv}, { { "api_key", config_->moviedb_key }, {"with_genres", department}, {"language", lang}, {"page", pagenum} }, root, config_->moviedbroot);
        /** <root>/discover/movie?api_key=<api-key> */
    }else if(querytype == 2){  //other categories
        get( {movie_or_tv, query}, {{ "api_key", config_->moviedb_key }, {"with_genres", department}, {"language", lang} }, root, config_->moviedbroot);
        /** <root>/movie/upcoming?api_key=<api_key> */
    }

    FilmRes result;

    QVariantMap variant = root.toVariant().toMap();
    for (const QVariant &i : variant["results"].toList()) {
        QVariantMap item = i.toMap();
        std::string posterimg, backdropimg;
        if(movie_or_tv == "person")
            posterimg = item["profile_path"].toString().toStdString();
        else{
            posterimg = item["poster_path"].toString().toStdString();
            backdropimg = item["backdrop_path"].toString().toStdString();
        }
        if(posterimg.empty())
            posterimg = "http://www.atmos.washington.edu/~carey/images/notFound.png";
        else
            posterimg = "http://image.tmdb.org/t/p/w300" + posterimg;
        if(backdropimg.empty() && movie_or_tv != "person")
            backdropimg = posterimg;
        else
            backdropimg = "http://image.tmdb.org/t/p/w300" + backdropimg;
        // We add each result to our list
        std::string title = item["title"].toString().toStdString();
        if(title == "")
            title = item["name"].toString().toStdString();

        result.films.emplace_back(
            Film {
                backdropimg,
                item["id"].toUInt(),
                item["original_title"].toString().toStdString(),
                item["release_date"].toString().toStdString(),
                posterimg,
                item["popularity"].toDouble(),
                title,
                item["vote_average"].toDouble(),
                item["vote_count"].toUInt()
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
