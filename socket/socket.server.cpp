#include "socket.hpp"

#include "../utils/logs/logs.hpp"

#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

/*
    Create and deploy a socket server.

    Tasks:
        1) Try to initialize the socket server. For Windows machines, initialize WSA first.
        2) Try to bind the socket server to the configured address and port.
        3) Try to make the socket server listen the configured port.

    Parameters (variable_name / type / description):
        - address       / string        / Targeted IP address or domain name to deploy the server to.
        - port          / int           / Port to listen.
        - server_socket / int or SOCKET / Output of the socket server creation.

    Returns (type + description):
        A boolean confirming whether the socket server was created or not.
*/
bool Socket::create_socket_server
(
    const std::string &address,
    const int         &max_retries,
    const int         &port,
    socket_type       &socket_server
)
{
    ////////////////// 1) //////////////////
    #ifdef _WIN32
        WSADATA win_socket;
        const bool initialization = WSAStartup(MAKEWORD(2, 2), &win_socket) == 0;

        if (!initialization)
        {
            Logs::log("Warning: Failed to initialize WSA.");
            return false;
        }
    #endif

    socket_server = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_server == INVALID_SOCKET)
    {
        Logs::log("Warning: Failed to initialize the socket server.");
        return false;
    }

    ////////////////// 2) //////////////////
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    const bool binding = bind(socket_server, (struct sockaddr*) &server_address, sizeof(server_address));

    if (binding == SOCKET_ERROR)
    {
        Logs::log("Warning: Failed to bind the server socket to the address and port.");
        return false;
    }

    ////////////////// 3) //////////////////
    const bool listening = listen(socket_server, max_retries);

    if (listening == SOCKET_ERROR)
    {
        Logs::log("Warning: Failed to listen port " + std::to_string(port) + ".");
        return false;
    }

    Logs::log("Socket server up and listening at http://" + address + ":" + std::to_string(port) + ".");
    Logs::log("WARNING: If the web server is not actually listening, do not run the server using administrator privileges.");
    Logs::log("         Instead, explicitly allow the web server to use the configured port on your machine.\n\nNote: [Ctrl+C] to shutdown.");

    return true;
}
