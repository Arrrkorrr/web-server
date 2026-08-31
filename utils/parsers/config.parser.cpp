#include "parsers.hpp"

#include "../logs/logs.hpp"
#include "../text/text.hpp"

#include <fstream>
#include <map>
#include <string>

/*
    Parse a config file.
    Warning: If 2 or more lines in the config file declare the same key, the most recent declaration found will overwrite all others.

    Tasks:
        1) Try to open the file.
        2) Proceed to parsing the config file.
            a. Check each line of the file.
            b. Verify that the line is valid and is not a comment.
            c. Try to find the equal sign position in the line. Format: key=value.
            d. Register what is before the equal sign as key and what is after as value.
            e. Register the value in the map identified by the key.
        3) Free the file and return the config.

    Parameters:
        - file_path / string / Path to the config file to parse.

    Returns:
        A map containing the parsed keys and values.
*/
std::map<std::string, std::string> Parsers::parse_config_file
(
    const std::string &file_path
)
{
    ////////////////// 1) //////////////////
    std::ifstream file(file_path);
    std::map<std::string, std::string> output;

    if (!file.is_open())
    {
        Logs::log("Warning: Failed to open config file for parsing -> " + file_path + ".");
        return output;
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    std::string line;

    while (getline(file, line))
    {
        ///////// b. /////////
        if (line.empty() || line[0] == '#')
            continue;

        ///////// c. /////////
        const size_t equal_sign_position = line.find("=");

        if (equal_sign_position == std::string::npos)
        {
            Logs::log("Warning: Failed to read malformed line -> \"" + line + "\".");
            continue;
        }

        ///////// d. /////////
        const std::string key = Text::trim(line.substr(0, equal_sign_position));
        const std::string value = Text::trim(line.substr(equal_sign_position + 1));

        if (key.empty())
        {
            Logs::log("Warning: Skipped empty key in config file.");
            continue;
        }

        ///////// e. //////////
        output[key] = value;
    }

    ////////////////// 3) //////////////////
    file.close();
    Logs::log("Config file \"" + file_path + "\" parsed -> " + std::to_string(output.size()) + " valid keys were loaded.");

    return output;
}
