#ifndef APP_ROUTES_HPP
#define APP_ROUTES_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

///////////////////////////////////////////////////
//////////////////// Structure ////////////////////
///////////////////////////////////////////////////

struct BackendData
{
    std::string key;
    std::string value;
};

////////////////////////////////////////////////
//////////////////// Routes ////////////////////
////////////////////////////////////////////////

using run = std::function<std::string()>;
inline std::unordered_map<std::string, run> routes;

///////////////////////////////////////////////////
//////////////////// Namespace ////////////////////
///////////////////////////////////////////////////

namespace Routes
{
    //////////////////////
    ///// routes.cpp /////
    //////////////////////

    void register_routes();

    std::string generate_valid_html
    (
        const int         &http_code,
        const std::string &page_content,
        const std::string &response_type
    );

    std::string generate_server_response
    (
        const std::string &path,
        const int         &max_request_length
    );

    ////////////////////////////
    ///// tags.handler.cpp /////
    ////////////////////////////

    std::string replace_custom_tags
    (
        const std::vector<BackendData> &backend_data,
        std::vector<std::string>       &html_page
    );
}

#endif
