#ifndef CLIENT_H_
#define CLIENT_H_

#include <config.h>

#include <atomic>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <core/net/http/request.h>
#include <core/net/uri.h>

#include <QJsonDocument>

namespace Json {
    class Value;
}

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


    virtual FilmRes query_films(const std::string &movie_or_tv, const std::string &query,
                                int querytype, const std::string &department,
                                const std::string &pagenum, const std::string &lang);


    /**
     * Cancel any pending queries (this method can be called from a different thread)
     */
    virtual void cancel();

    virtual Config::Ptr config();

//protected:
    virtual void get(const core::net::Uri::Path &path,
             const core::net::Uri::QueryParameters &parameters,
             QJsonDocument &root, std::string &apiroot);
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

#endif // CLIENT_H_
