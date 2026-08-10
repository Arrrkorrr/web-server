#include "routes.hpp"
#include "../utils/utils.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/*
    Replace all <++ ... ++> custom HTML tags with all data provided.
    Warning: If a tag doesn't have any data to be replaced with, we simply delete it.

    Tasks:
        1) Verify function parameters.
        2) Convert the vector list containing the backend data into an unordered map to make browsing easier.
        3) Analyze each line of the HTML page.
            -> Reset our position in the line.
            -> Try to find the beginning of a custom tag (<++).
            -> If we didn't find any, we finished to analyze this line and register the rest. Otherwise, we keep going.
            -> Register the content without the beginning of the tag, and try to find the end of the tag (++>).
            -> If not found, we send a warning to the user and register all of it as plain text.
            -> We try to extract the key within the tag and trim it to avoid false errors.
            -> Try to find the key in the unordered map. If we did find it, we register the value associated to this key, to replace the tag.

    Parameters:
        - data      / vector<BackendData> / Keys in the .html file that are linked to some data.
        - html_page / vector<string>      / Content of the .html page to parse.

    Returns:
        The rendering of the HTML page.
*/
std::string Routes::replace_custom_tags
(
    const std::vector<BackendData> &backend_data,
    std::vector<std::string> &html_page
)
{
    if (backend_data.size() < 1)
    {
        std::cerr << "There is nothing to do!";
        return "";
    }

    if (html_page.size() < 1)
    {
        std::cerr << "Warning: Failed to work on an empty HTML page!\n";
        return "";
    }

    std::unordered_map<std::string, std::string> backend_data_map;
    backend_data_map.reserve(backend_data.size());

    for (const BackendData &data : backend_data)
        backend_data_map[data.key] = data.value;

    const std::string opening_tag = "<++";
    const std::string closing_tag = "++>";
    std::string output;

    for (const std::string &line : html_page)
    {
        if (line.empty())
            continue;

        std::size_t position = 0;
        std::string result;

        while (position < line.size())
        {
            const std::size_t tag_start = line.find(opening_tag, position);

            if (tag_start == std::string::npos)
            {
                result += line.substr(position);
                break;
            }

            result += line.substr(position, tag_start - position);
            const std::size_t tag_end = line.find(closing_tag, tag_start + opening_tag.size());

            if (tag_end == std::string::npos)
            {
                std::cerr << "Warning: Failed to find a closing tag, saved it as plain text.\n";
                result += line.substr(tag_start);
                break;
            }

            std::string html_key = line.substr
            (
                tag_start + opening_tag.size(),
                tag_end - (tag_start + opening_tag.size())
            );

            html_key = Utils::Text::trim(html_key);
            const auto value = backend_data_map.find(html_key);

            if (value != backend_data_map.end())
                result += value -> second;

            position = tag_end + closing_tag.size();
        }

        output += result + "\n";
    }

    return output;
}
