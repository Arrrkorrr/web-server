#include "socket.hpp"

#include "../routes/routes.hpp"
#include "../utils/logs/logs.hpp"

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
        1) Try to read the bytes of the request data.
        2) Parse the request data.
            a. Try to retrieve the end of the first line.
            b. Parse the first line information containing the method (GET, POST, etc), the path (/home for example), and HTTP version.
            c. Try to get the client IP address.
            d. Log the request and client information as a middleware.
        3) Handle the request.
            a. Prepare the server response depending on the routes server response.
            b. Send the reply to the client.

    Parameters (variable_name / type / description):
        - client / int or SOCKET / Request data provided by the client.

    Returns (type + description):
        No object returned.
*/
void Socket::handle_request
(
    const socket_type &client,
    const int         &max_request_length
)
{
    ////////////////// 1) //////////////////
    char buffer[2048];

    #ifdef _WIN32
        const int read_bytes = recv(client, buffer, sizeof(buffer) - 1, 0);
    #else
        const int read_bytes = read(client, buffer, sizeof(buffer) - 1);
    #endif

    if (read_bytes <= 0)
    {
        const std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";

        #ifdef _WIN32
            send(client, response.c_str(), response.size(), 0);
            closesocket(client);
        #else
            write(client, response.c_str(), response.size());
            close(client);
        #endif

        return Logs::log("Warning: Ignored empty request.");
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const std::string client_request(buffer, read_bytes);
    const size_t line_end = client_request.find("\r\n");

    if (line_end == std::string::npos)
    {
        const std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";

        #ifdef _WIN32
            send(client, error.c_str(), error.size(), 0);
            closesocket(client);
        #else
            write(client, error.c_str(), error.size());
            close(client);
        #endif

        return Logs::log("Warning: Ignored invalid request.");
    }

    ///////// b. /////////
    const std::string first_line = client_request.substr(0, line_end);
    std::istringstream stream(first_line);

    ///////// c. /////////
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    std::string ip;
    const bool address_available = (getpeername(client, (struct sockaddr*) &address, &length) == 0);

    if (address_available)
    {
        char output[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, output, INET_ADDRSTRLEN);
        ip = std::string(output) + " ";
    }

    ///////// d. /////////
    std::string method, path, version;
    stream >> method >> path >> version;

    Logs::log("New request -> " + ip + "" + method + " " + path + " (HTTP " + version + ").");

    ////////////////// 3) //////////////////
    ///////// a. /////////
    const std::string response = Routes::generate_server_response(path, max_request_length);

    ///////// b. /////////
    #ifdef _WIN32
        send(client, response.data(), response.size(), 0);
        closesocket(client);
    #else
        write(client, response.data(), response.size());
        close(client);
    #endif
}
