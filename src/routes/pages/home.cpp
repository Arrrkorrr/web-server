#include "pages.hpp"

#include "../routes.hpp"
#include "../../utils/utils.hpp"

#include <string>
#include <vector>

/*
    Demo home page that replaces some test tags with test data.

    Tasks:
        1) Declare the keys we wish to replace with the data into the page.
        2) Read the page.html page content.
        3) Replace the custom tags in the html page with the desired data.

    Parameters:
        No parameters.

    Returns:
        A string containing the html page to display to the client.
*/
std::string Pages::page_home()
{
    const std::vector<BackendData> backend_data
    {
        { "hello", "Welcome to the default page!" },
        { "presentation", "This basic web server, embedding a socket server, enables C++ developers to easily develop a back-end for their website projects." },
        { "custom_tag", "To dynamically modify, the page, you have a custom tag that you can use in your HTML page: <++ ++>. More information <a href=\"https://github.com/ArchorByte/web-server/blob/master/README.md\">here</a>." },
        { "github", "<a href=\"https://github.com/ArchorByte/web-server\">Link to the GitHub repository</a>" }
    };

    std::vector<std::string> html_page = Utils::Files::read_file("./website/page.html");
    const std::string rendering = Routes::replace_custom_tags(backend_data, html_page);

    return rendering;
}
