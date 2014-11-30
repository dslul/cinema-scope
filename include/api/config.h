#ifndef API_CONFIG_H_
#define API_CONFIG_H_

#include <memory>
#include <string>

namespace api {

struct Config {
    typedef std::shared_ptr<Config> Ptr;

    //The root of all API request URLs
    std::string moviedbroot { "https://api.themoviedb.org/3" };
    std::string traktroot { "http://api.trakt.tv" };

    //my developer keys
    std::string moviedb_key {""};
    std::string trakt_key {""};

    //The custom HTTP user agent string for this library
    std::string user_agent { "example-network-scope 0.1; (foo)" };
};

}

#endif /* API_CONFIG_H_ */

