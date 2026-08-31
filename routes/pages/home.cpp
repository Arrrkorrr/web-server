#include "pages.hpp"

#include "../routes.hpp"
#include "../../utils/files/files.hpp"

#include <string>
#include <vector>

/*
    Demonstration home page.

    Tasks:
        1) Declare all tags that we wish to replace with some data.
        2) Read the page.html file content.
        3) Replace the tags in the html page with the desired data and return it.

    Parameters (variable_name / type / description):
        No parameters.

    Returns (type + description):
        A string containing the HTML page to send back to the client.
*/
std::string Pages::page_home()
{
    ////////////////// 1) //////////////////
    const std::vector<BackendData> backend_data
    {
        { "hello", "Welcome to the default page!" },
        { "presentation", "This basic web server, embedding a socket server, enables C++ developers to easily develop a back-end for their website projects." },
        { "custom_tag", "To dynamically modify a page, you have a new custom tag that you can use directly in your HTML page. More information <a href=\"https://github.com/Arrrkorrr/web-server/blob/master/README.md\">here</a>." },
        { "github", "<a href=\"https://github.com/Arrrkorrr/web-server\">Link to the GitHub repository</a>." }
    };

    ////////////////// 2) //////////////////
    std::vector<std::string> html_page = Files::read_file("./website/page.html");

    ////////////////// 3) //////////////////
    const std::string rendering = Routes::replace_custom_tags(backend_data, html_page);
    return rendering;
}
