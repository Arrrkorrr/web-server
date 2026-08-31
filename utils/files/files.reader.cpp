#include "files.hpp"

#include "../logs/logs.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

/*
    Read the content of a file.

    Tasks:
        1) We do some verification.
            a. Verify that the path contains a file name in it.
            b. Verify that the file exists.
        2) Proceed to reading the file.
            a. Try to open the file in read-only mode.
            b. Register all file lines one by one.
            c. Free the file and return the file content.

    Parameters (variable_name / type / description):
        - file_path / string / Path to the file.

    Returns (type + description):
        A vector list of strings containing each line of the file.
*/
std::vector<std::string> Files::read_file
(
    const std::string &file_path
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    std::vector<std::string> output;
    const bool has_filename = std::filesystem::path(file_path).has_filename();

    if (!has_filename)
    {
        Logs::log("Warning: File reading failed -> Path provided (" + file_path + ") is not valid.");
        return output;
    }

    ///////// b. /////////
    const bool file_exists(std::filesystem::exists(file_path));

    if (!file_exists)
    {
        Logs::log("Warning: File reading failed -> No such file or directory.");
        return output;
    }

    ////////////////// 2) //////////////////
    ///////// a. //////////
    std::ifstream file (file_path);
    const bool is_opened = file.is_open();

    if (!is_opened)
    {
        Logs::log("Warning: File reading failed -> Failed to open file (" + file_path + ") for reading.");
        return output;
    }

    ///////// b. /////////
    std::string line;

    while (std::getline(file, line))
        output.push_back(line);

    ///////// c. /////////
    file.close();
    return output;
}



/*
    Read the content of a file as binaries.

    Tasks:
        1) We do some verification.
            a. Verify that the path contains a file name in it.
            b. Verify that the file exists.
        2) Proceed to reading the file.
            a. Try to open the file in binary read-only mod.
            b. Verify that file is not empty.
            c. Try to read the file.
            d. Free the file and return the file binaries.

    Parameters (variable_name / type / description):
        - file_path / string / Path to the file.

    Returns (type + description):
        A vector list of char containing the file binaries.
*/
std::vector<char> Files::read_binary_file
(
    const std::string &file_path
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    std::vector<char> output;
    const bool has_filename = std::filesystem::path(file_path).has_filename();

    if (!has_filename)
    {
        Logs::log("Warning: Binary file reading failed -> Path provided (" + file_path + ") is not valid.");
        return output;
    }

    ///////// b. /////////
    const bool file_exists(std::filesystem::exists(file_path));

    if (!file_exists)
    {
        Logs::log("Warning: Binary file reading failed -> No such file or directory.");
        return output;
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    std::ifstream file (file_path, std::ios::ate | std::ios::binary);
    const bool is_opened = file.is_open();

    if (!is_opened)
    {
        Logs::log("Warning: Binary file reading failed -> Failed to open file (" + file_path + ") for reading.");
        return output;
    }

    ///////// b. /////////
    const size_t file_size = static_cast<size_t>(file.tellg());

    if (file_size < 1)
    {
        Logs::log("Warning: Binary file reading failed -> Failed to read file (" + file_path + ") as it is empty.");
        return output;
    }

    ///////// c. /////////
    output.resize(file_size);
    file.seekg(0);

    if (!file.read(output.data(), file_size))
    {
        Logs::log("Warning: Binary file reading failed -> Failed to read file (" + file_path + ").");
        output.clear();
    }

    ///////// d. /////////
    file.close();
    return output;
}
