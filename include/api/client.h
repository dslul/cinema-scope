#ifndef API_CLIENT_H_
#define API_CLIENT_H_

#include <api/config.h>

#include <atomic>
#include <deque>
#include <map>
#include <string>
#include <core/net/http/request.h>
#include <core/net/uri.h>

#include <QJsonDocument>

namespace api {

/**
 * Provide a nice way to access the HTTP API.
 *
 * We don't want our scope's code to be mixed together with HTTP and JSON handling.
 */
class Client {
    public:

    /**
    * film info, including the artist.
    */
    struct Film {
        std::string backdrop_path;  //most-voted image path
        unsigned int id;
        std::string original_title; //usually the english title
        std::string release_date;   //format: yyyy-mm-dd
        std::string poster_path;    //poster image path
        double popularity;
        std::string title;          //title according to language
        double vote_average;
        unsigned int vote_count;    //number of votes
        //additional info
        std::string overview;
        std::string youtubeurl;
        std::string status;
        std::string tagline;
        //backdrop images
    };

    /**
    * A list of Film objects.
    */
    typedef std::deque<Film> FilmList;

    /**
    * Film results.
    */
    struct FilmRes {
        FilmList films;
    };

    Client(Config::Ptr config);

    virtual ~Client() = default;


    virtual FilmRes query_films(const std::string &query, int querytype);

    /**
     * Cancel any pending queries (this method can be called from a different thread)
     */
    virtual void cancel();

    virtual Config::Ptr config();

//protected:
    virtual void get(const core::net::Uri::Path &path,
             const core::net::Uri::QueryParameters &parameters,
             QJsonDocument &root);
    /**
     * Progress callback that allows the query to cancel pending HTTP requests.
     */
    core::net::http::Request::Progress::Next progress_report(
            const core::net::http::Request::Progress& progress);

    /**
     * Hang onto the configuration information
     */
    Config::Ptr config_;

    /**
     * Thread-safe cancelled flag
     */
    std::atomic<bool> cancelled_;
};

}

#endif // API_CLIENT_H_
