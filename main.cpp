#include "src/app.hpp"

#include <iostream>
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
    Main function of the program.

    Tasks:
        1) Parse the config file.
        2) Verify the retrieved data.
        3) Start a socket server.
        4) Create a thread for each request to treat.

    Parameters:
        No parameters.

    Returns:
        An integer representing a success or error code.
*/
int main()
{
    std::cout << "The web server is booting up..\n";

    App::Routes::register_routes();
    std::map<std::string, std::string> config = App::Utils::Parsers::parse_config_file("socket.config");

    const std::string address = config["ADDRESS"];
    std::string port = config["PORT"];
    std::string max_retries = config["MAX_RETRIES"];

    if (!Utils::Text::is_an_integer(port))
    {
        std::cerr << "Warning: The retrieved port from the config file is not valid! Defaulted to 8080.\n";
        port = "8080";
    }

    if (!Utils::Text::is_an_integer(max_retries))
    {
        std::cerr << "Warning: The retrieved max retries value from the config file is not valid! Defaulted to 5.\n";
        max_retries = "5";
    }

    socket_type server_socket;
    const bool socket_creation = App::Socket::create_socket_server(address, std::stoi(max_retries), std::stoi(port), server_socket);

    if (!socket_creation)
    {
        std::cerr << "Failed to create the socket server!\n";
        exit(-1);
    }

    while (true)
    {
        struct sockaddr_in client_address;

        socklen_t request_size = sizeof(client_address);
        socket_type client = accept(server_socket, (struct sockaddr*) &client_address, &request_size);

        if (client == INVALID_SOCKET)
        {
            std::cerr << "Warning: Failed to accept the client!\n";
            continue;
        }

        std::thread client_thread(App::Socket::handle_request, client);
        client_thread.detach();
    }

    std::cout << "Web server stopped successfully!\n";
    return 0;
}
