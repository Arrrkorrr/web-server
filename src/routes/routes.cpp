#include "routes.hpp"

#include "pages/pages.hpp"
#include "../utils/utils.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/*
    Register all declared routes into our unordered routes.

    Tasks:
        1) Add new routes to the unordered routes.
        2) Link a function to the routes.

    Parameters:
        No parameters.

    Returns:
        No object returned.
*/
void Routes::register_routes()
{
    routes["/"] = []() -> std::string
    {
        return Pages::page_home();
    };

    routes["/home"] = []() -> std::string
    {
        return Pages::page_home();
    };
}



/*
    Make a valid HTML response to return to the client.

    Tasks:
        1) Make the response using the data provided.

    Parameters:
        - http_code     / int    / Code to return (200 on success, 404 on not found -> https://grokipedia.com/page/List_of_HTTP_status_codes).
        - page_content  / string / HTML page to return.
        - response_type / string / Type of the content returned (htm, png, text..).

    Returns:
        A string containing the response to give to the client.
*/
std::string make_valid_html_response
(
    const int &http_code,
    const std::string &page_content,
    const std::string &response_type
)
{
    const std::string html_response =
        "HTTP/1.1 " + std::to_string(http_code) +  " " + (http_code == 200 ? "OK" : "Not Found") +
        "\r\nContent-Type: " + response_type +
        "\r\nContent-Length: " + std::to_string(page_content.size()) +
        "\r\nConnection: close\r\n\r\n" + page_content;

    return html_response;
}



/*
    Makes a valid HTTP response to the user, depending on the request.

    Tasks:
        1) Verify function parameters.
        2) Try to find the route into our list of defined routes.
            -> If found, we return the page requested.
            -> If not, we continue and try to find a file corresponding to the path.
        3) We deny access if the path contains the two dots to get higher in the tree or hidden files.
        4) Check that the path targets a valid file.
        5) Read the binaries of the targeted path.
        6) Determine the type the file with the supported formats map.
        7) Return to the client the file content.

    Parameters:
        - path / string / Path defined in the request URL (e.g. 127.0.0.1/home -> "/home").

    Returns:
        A string containing the built response.
*/
std::string Routes::make_server_response
(
    const std::string &path
)
{
    const auto route = routes.find(path);

    if (route != routes.end())
    {
        const std::string html_page = route -> second();
        return make_valid_html_response(200, html_page, "text/html; charset=utf-8");
    }

    if (path.find("..") != std::string::npos)
    {
        std::cerr << "Access denied: Client requested to get higher in the tree!\n";
        return make_valid_html_response(403, "Forbidden", "text/plain; charset=utf-8");
    }

    if (path.find("/.") != std::string::npos)
    {
        std::cerr << "Access denied: Client requested access to a hidden file!";
        return make_valid_html_response(403, "Forbidden", "text/plain; charset=utf-8");
    }

    const std::string file_path = "website/" + path;
    const bool file_exists = std::filesystem::exists(file_path);
    const bool is_valid_file = std::filesystem::is_regular_file(file_path);

    if (!file_exists || !is_valid_file)
    {
        std::cerr << "Warning: Page not found (" << path << ")!\n";
        return make_valid_html_response(404, "Not Found!", "text/plain; charset=utf-8");
    }

    const std::string file_extension = std::filesystem::path(file_path).extension().string();
    const std::vector<char> file_content = Utils::Files::read_binary_file(file_path);

    if (file_content.empty())
    {
        std::cerr << "Warning: Failed to read file binaries!";
        return make_valid_html_response(500, "Internal Server Error", "text/plain; charset=utf-8");
    }

    std::unordered_map<std::string, std::string> supported_formats =
    {
        { ".css", "text/css; charset=utf-8" },
        { ".gif", "image/gif" },
        { ".html", "text/html; charset=utf-8" },
        { ".ico", "image/x-icon" },
        { ".jpg", "image/jpeg" },
        { ".jpeg", "image/jpeg" },
        { ".js", "text/javascript; charset=utf-8" },
        { ".json", "application/json" },
        { ".pdf", "application/pdf" },
        { ".mkv", "video/mkv" },
        { ".mp3", "audio/mpeg" },
        { ".mp4", "video/mp4" },
        { ".ogg", "application/ogg" },
        { ".png", "image/png" },
        { ".svg", "image/svg" },
        { ".webm", "video/webm" },
        { ".xml", "application/xml" },
        { ".zip", "application/zip" }
    };

    const std::string data(file_content.begin(), file_content.end());
    const auto type = supported_formats.find(file_extension);

    if (type == supported_formats.end())
    {
        std::cerr << "Warning: Unsupported file extension requested (" << file_extension << ")!\n";
        return make_valid_html_response(404, "Not Found!", "text/plain; charset=utf-8");
    }

    return make_valid_html_response(200, data, type -> second);
}
