#include "socket.hpp"

#include <iostream>
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
        1) Verify function parameters.
        2) Try to initialize WSA for Windows systems.
        3) Try to initialize the socket.
        4) Try to bind the socket to an address.
        5) Try to make the socket listen the specified address on the specified port.

    Parameters:
        - address       / string        / IP address or domain name to run the socket on.
        - port          / int           / Port to listen.
        - server_socket / int or SOCKET / Variable that will be modified to contain the created socket server.

    Returns:
        A boolean confirming whether we successfully created the socket server or not.
*/
bool Socket::create_socket_server
(
    const std::string &address,
    const int &max_retries,
    const int &port,
    socket_type &server_socket
)
{
    #ifdef _WIN32
        WSADATA win_socket;
        const bool initialization = WSAStartup(MAKEWORD(2, 2), &win_socket) == 0;

        if (!initialization)
        {
            std::cerr << "Failed to initialize WSA!\n";
            return false;
        }
    #endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET)
    {
        std::cerr << "Failed to initialize the web server socket!\n";
        return false;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    const bool binding = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    if (binding == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind the server address and the server socket!\n";
        return false;
    }

    const bool listening = listen(server_socket, max_retries);

    if (listening == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen port " << port << "!\n";
        return false;
    }

    std::cout << "Socket server up and listening at http://" << address << ":" << port << ".\nWarning: If the web server do not appear as listening, do not run the server using administrator permissions but explicitly allow the web_server executable to use the port on your machine.\n\nNote: [Ctrl+C] to shutdown.\n";
    return true;
}
