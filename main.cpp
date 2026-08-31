#include "config/tweaks.hpp"
#include "socket/socket.hpp"
#include "routes/routes.hpp"
#include "utils/files/files.hpp"
#include "utils/logs/logs.hpp"
#include "utils/miscellaneous/miscellaneous.hpp"
#include "utils/parsers/parsers.hpp"

#include <csignal>
#include <filesystem>
#include <map>
#include <string>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

/*
    Main function of the project.

    Tasks:
        1) Handle the logs file.
            a. Remove the old fsb.logs file if it exists.
            b. Create a new fsb.logs file if the log file is turned on.
        2) Register the server routes to use.
        3) Load the configuration.
            a. Parse the "socket.config" file.
            b. Load the address, port and max retries settings if possible. Otherwise, we default to port 8080 and/or 5 max retries.
        4) Try to create the socket to listen requests to a certain address on a certain port.
        5) Set some signal handlers.
            a. We ignore EPIPE errors that may trigger when the server sends data back but the client closed the connection before.
            b. We handle [Ctrl+C] to cleanly close the web server instead of killing the process.
        6) Handle requests made to the server.
            a. Wait for a client request and try to retrieve some information.
            b. Handle the request in a separate thread in order to free the loop and accept other client requests.

    Parameters (variable_name / type / description):
        No parameters.

    Returns (type + description):
        An integer containing the exit code of the program.
*/
int main()
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    if (std::filesystem::exists("server.logs"))
        std::filesystem::remove("server.logs");

    ///////// b. /////////
    if constexpr (Tweaks::ENABLE_LOGS_FILE)
        Files::create_new_empty_file("server.logs");

    ////////////////// 2) //////////////////
    Logs::log("The web server is booting up..");
    Routes::register_routes();

    ////////////////// 3) //////////////////
    ///////// a. /////////
    std::map<std::string, std::string> config = Parsers::parse_config_file("server.config");

    ///////// b. /////////
    const std::string address = config["ADDRESS"];
    const std::string conf_port = config["PORT"];
    const std::string conf_max_retries = config["MAX_RETRIES"];
    const std::string conf_max_length = config["MAX_REQUEST_LENGTH"];

    int port, max_retries, max_request_length;

    if (!Miscellaneous::is_an_integer(conf_port))
    {
        Logs::log("Warning: Configured port is not valid -> " + conf_port +  ". Defaulted to 8080.");
        port = 8080;
    }
    else port = std::stoi(conf_port);

    if (!Miscellaneous::is_an_integer(conf_max_retries))
    {
        Logs::log("Warning: Configured max retries is not valid -> " + conf_max_retries + ". Defaulted to 5.");
        max_retries = 5;
    }
    else max_retries = std::stoi(conf_max_retries);

    if (!Miscellaneous::is_an_integer(conf_max_length))
    {
        Logs::log("Warning: Configured max request length is not valid -> " + conf_max_length + ". Defaulted to 100.");
        max_request_length = 100;
    }
    else max_request_length = std::stoi(conf_max_length);

    ////////////////// 4) //////////////////
    static socket_type server_socket;
    const bool socket_creation = Socket::create_socket_server(address, max_retries, port, server_socket);

    if (!socket_creation)
        Logs::crash_log("Failed to create the socket server.\nPlease, verify the address (" + address + "), server permissions to listen port " + std::to_string(port) + ", and verify no other instance/program is already listening.");

    ////////////////// 5) //////////////////
    ///////// a. /////////
    std::signal(SIGPIPE, SIG_IGN);

    ///////// b. /////////
    std::signal(SIGINT, [](int)
    {
        shutdown(server_socket, SHUT_RD);
        Logs::log("\nThe web server did shut down.");
        exit(EXIT_SUCCESS);
    });

    ////////////////// 6) //////////////////
    while (true)
    {
        ///////// a. /////////
        struct sockaddr_in client_address;
        socklen_t request_size = sizeof(client_address);

        socket_type client = accept(server_socket, (struct sockaddr*) &client_address, &request_size);

        if (client == INVALID_SOCKET)
        {
            Logs::log("Warning: Failed to accept invalid client.");
            continue;
        }

        ///////// b. /////////
        std::thread client_thread(Socket::handle_request, client, max_request_length);
        client_thread.detach();
    }

    return 0;
}
