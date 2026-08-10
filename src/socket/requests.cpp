#include "socket.hpp"

#include "../routes/routes.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

/*
    Handle an HTTP request.

    Tasks:
        1) Read the data of the request made by the client with a buffer.
        2) Convert it to string to make it easier to work with.
        3) Get the first line that contains the essential request information.
        4) Retrieve the data from it (method, HTTP version, and requested path).
        5) Log it, and reply a simple hello world.

    Parameters:
        - client / int or SOCKET / Request data provided by the client.

    Returns:
        No object returned.
*/
void Socket::handle_request
(
    const socket_type &client
)
{
    char buffer[2048];

    #ifdef _WIN32
        const int bytes_read = recv(client, buffer, sizeof(buffer) - 1, 0);
    #else
        const int bytes_read = read(client, buffer, sizeof(buffer) - 1);
    #endif

    if (bytes_read <= 0)
    {
        const std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";

        #ifdef _WIN32
            send(client, response.c_str(), response.size(), 0);
            closesocket(client);
        #else
            write(client, response.c_str(), response.size());
            close(client);
        #endif

        std::cerr << "Warning: Ignored empty request!\n";
        return;
    }

    const std::string request(buffer, bytes_read);
    const size_t first_line_end = request.find("\r\n");

    if (first_line_end == std::string::npos)
    {
        const std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";

        #ifdef _WIN32
            send(client, response.c_str(), response.size(), 0);
            closesocket(client);
        #else
            write(client, response.c_str(), response.size());
            close(client);
        #endif

        std::cerr << "Warning: Ignored bad request!\n";
        return;
    }

    const std::string first_line = request.substr(0, first_line_end);
    std::istringstream stream(first_line);

    std::string method, path, version;
    stream >> method >> path >> version;

    std::cout << "New request detected: " << method << " " << path << " " << version << "\n";
    const std::string response = Routes::make_server_response(path);

    #ifdef _WIN32
        send(client, response.data(), response.size(), 0);
        closesocket(client);
    #else
        write(client, response.data(), response.size());
        close(client);
    #endif
}
