#ifndef CONFIG_H_
#define CONFIG_H_

#include <memory>
#include <string>

struct Config {
    typedef std::shared_ptr<Config> Ptr;

    //The root of all API request URLs
    std::string moviedbroot { "https://api.themoviedb.org/3" };
    std::string traktroot { "http://api.trakt.tv" };

    //my developer keys
    std::string moviedb_key {"4149363c46a16a04a1d48ad3098197b0"};
    std::string trakt_key {"0782efd56c92cc17272fb512749c7984"};

    //The custom HTTP user agent string for this library
    std::string user_agent { "example-network-scope 0.1; (foo)" };
};

#endif /* CONFIG_H_ */

