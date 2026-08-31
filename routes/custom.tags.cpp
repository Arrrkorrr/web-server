#include "routes.hpp"

#include "../utils/logs/logs.hpp"
#include "../utils/text/text.hpp"

#include <string>
#include <unordered_map>
#include <vector>

/*
    Replace all "<++ ... ++>" custom HTML tags.
    Note: If a tag does not have any data to be replaced with, it get deleted from the page.

    Tasks:
        1) Register all backend data in a map.
        2) Proceed to replacing all custom tags with the data.
            a. Define the custom tags formatting.
            b. Work on each line of the HTML page.
            c. Analyze each character of each line until we find the opening of a custom tag.
            d. Extract any data that was before the beginning of the tag. If we do not find any tag in the line, directly register the line and analyze another one.
            e. Try to find the closing of the tag.
            f. Extract the tag key. Tag format: "<++ key ++>".
            g. Replace the tag with the value associated in the map.
            h. Set the position to after the tag so we can keep anylizing the line if any characters are left.
            i. Once we finished processing the line, register it in the output.

    Parameters (variable_name / type / description):
        - data      / vector<BackendData> / All tags names and the data to replace them with.
        - html_page / vector<string>      / HTML file to parse.

    Returns (type + description):
        A string containing the rendered HTML page.
*/
std::string Routes::replace_custom_tags
(
    const std::vector<BackendData> &backend_data,
    std::vector<std::string>       &html_page
)
{
    ////////////////// 1) //////////////////
    std::unordered_map<std::string, std::string> tags;
    tags.reserve(backend_data.size());

    for (const BackendData &data : backend_data)
        tags[data.key] = data.value;

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const std::string tag_opening = "<++";
    const std::string tag_closing = "++>";

    ///////// b. /////////
    std::string output;

    for (const std::string &line : html_page)
    {
        ///////// c. /////////
        std::size_t position = 0;
        std::string processed_data;

        while (position < line.size())
        {
            ///////// d. /////////
            const std::size_t tag_start = line.find(tag_opening, position);

            if (tag_start == std::string::npos)
            {
                processed_data += line.substr(position);
                break;
            }

            ///////// e. /////////
            processed_data += line.substr(position, tag_start - position);
            const std::size_t tag_end = line.find(tag_closing, tag_start + tag_opening.size());

            if (tag_end == std::string::npos)
            {
                Logs::log("Warning: Failed to find the closing of the tag. Replacement aborted.");
                processed_data += line.substr(tag_start);
                break;
            }

            ///////// f. /////////
            std::string html_key = line.substr
            (
                tag_start + tag_opening.size(),
                tag_end - (tag_start + tag_opening.size())
            );

            ///////// g. /////////
            html_key = Text::trim(html_key);
            const auto value = tags.find(html_key);

            if (value != tags.end())
                processed_data += value -> second;

            ///////// h. /////////
            position = (tag_end + tag_closing.size());
        }

        ///////// i. /////////
        output += (processed_data + "\n");
    }

    return output;
}
