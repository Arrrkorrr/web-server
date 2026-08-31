#include "routes.hpp"

#include "pages/pages.hpp"
#include "../utils/files/files.hpp"
#include "../utils/logs/logs.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

/*
    Register all pages into the routes map.

    Tasks:
        1) Add new paths to the routes map.
        2) Link a function for each new route.

    Parameters (variable_name / type / description):
        No parameters.

    Returns (type + description):
        No object returned.
*/
void Routes::register_routes()
{
    ////////////////// 1) //////////////////
    routes["/"] = []() -> std::string
    {
        ////////////////// 2) //////////////////
        return Pages::page_home();
    };

    routes["/home"] = []() -> std::string
    {
        return Pages::page_home();
    };
}



/*
    Generate a valid HTML response to return to the client.

    Tasks:
        1) Declare all HTTP codes "supported".
        2) Generate the HTML response with all information provided.

    Parameters (variable_name / type / description):
        - http_code     / int    / Code to return (200 on success, 404 on not found -> https://grokipedia.com/page/List_of_HTTP_status_codes).
        - page_content  / string / HTML page to return.
        - response_type / string / Type of the content returned (htm, png, text..).

    Returns (type + description):
        A string containing the response to send back to the client.
*/
std::string Routes::generate_valid_html
(
    const int         &http_code,
    const std::string &page_content,
    const std::string &response_type
)
{
    ////////////////// 1) //////////////////
    std::map<int, std::string> codes
    {
        { 200, "OK" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 500, "Internal Server Error" }
    };

    ////////////////// 2) //////////////////
    const std::string html_response =
        "HTTP/1.1 " + std::to_string(http_code) +  " " + codes[http_code] +
        "\r\nContent-Type: " + response_type +
        "\r\nContent-Length: " + std::to_string(page_content.size()) +
        "\r\nConnection: close\r\n\r\n" + page_content;

    return html_response;
}



/*
    Generate a server response to the client request.

    Tasks:
        1) Verify the request length.
        2) Take the request as a route query.
            a. Try to find the request in the routes map.
            b. Format the response and return it.
        3) If it's not a route, we take the request as a file query.
            a. Verify that the client is not trying to get higher in the tree.
            b. Verify that the client is not trying to access a hidden file.
            c. Verify that the file does exist.
            d. Declared all authorized file extensions.
            e. Verify that the client is not requesting access to an unauthorized file extension.
        4) Process the file request.
            a. Try to read file binaries.
            b. Format the response and return it.

    Parameters (variable_name / type / description):
        - path / string / Path requested by the client.

    Returns (type + description):
        A string containing the server response.
*/
std::string Routes::generate_server_response
(
    const std::string &path,
    const int         &max_request_length
)
{
    ////////////////// 1) //////////////////
    const int length = path.length();

    if (length > max_request_length)
    {
        Logs::log("Access denied -> Request is too long (" + std::to_string(length) + " > " + std::to_string(max_request_length) + ").");
        return generate_valid_html(403, "403 - Forbidden", "text/html; charset=utf-8");
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const auto route = routes.find(path);

    if (route != routes.end())
    {
        ///////// b. /////////
        const std::string html_page = route -> second();
        return generate_valid_html(200, html_page, "text/html; charset=utf-8");
    }

    ////////////////// 3) //////////////////
    ///////// a. /////////
    if (path.find("..") != std::string::npos)
    {
        Logs::log("Access denied -> Client requested to get higher in the tree.");
        return generate_valid_html(403, "403 - Forbidden", "text/plain; charset=utf-8");
    }

    ///////// b. /////////
    if (path.find("/.") != std::string::npos)
    {
        Logs::log("Access denied -> Client requested access to a hidden file.");
        return generate_valid_html(403, "403 - Forbidden", "text/plain; charset=utf-8");
    }

    ///////// c. /////////
    const std::string file_path = "website/" + path;
    const bool file_exists = std::filesystem::exists(file_path);
    const bool is_valid_file = std::filesystem::is_regular_file(file_path);

    if (!file_exists || !is_valid_file)
    {
        Logs::log("Request failed -> Page not found \"" + path + "\".");
        return generate_valid_html(404, "404 - Not Found", "text/plain; charset=utf-8");
    }

    ///////// d. /////////
    std::unordered_map<std::string, std::string> supported_formats =
    {
        { ".css", "text/css; charset=utf-8" },
        { ".gif", "image/gif" },
        { ".gz", "application/gzip" },
        { ".html", "text/html; charset=utf-8" },
        { ".ico", "image/x-icon" },
        { ".jpg", "image/jpeg" },
        { ".jpeg", "image/jpeg" },
        { ".js", "text/javascript; charset=utf-8" },
        { ".json", "application/json" },
        { ".mkv", "video/mkv" },
        { ".mp3", "audio/mpeg" },
        { ".mp4", "video/mp4" },
        { ".ogg", "application/ogg" },
        { ".pdf", "application/pdf" },
        { ".png", "image/png" },
        { ".svg", "image/svg" },
        { ".txt", "text/plain" },
        { ".webm", "video/webm" },
        { ".xml", "application/xml" },
        { ".zip", "application/zip" }
    };


    ///////// e. /////////
    const std::string file_extension = std::filesystem::path(file_path).extension().string();
    const auto search = supported_formats.find(file_extension);

    if (search == supported_formats.end())
    {
        Logs::log("Request failed -> Unsupported file extension \"" + file_extension + "\".");
        return generate_valid_html(404, "404 - Not Found", "text/plain; charset=utf-8");
    }

    ////////////////// 4) //////////////////
    ///////// a. /////////
    const std::vector<char> file_content = Files::read_binary_file(file_path);

    if (file_content.empty())
    {
        Logs::log("Request failed -> Failed to read file binaries.");
        return generate_valid_html(500, "500 - Internal Server Error", "text/plain; charset=utf-8");
    }

    ///////// b. /////////
    const std::string data(file_content.begin(), file_content.end());
    return generate_valid_html(200, data, search -> second);
}
