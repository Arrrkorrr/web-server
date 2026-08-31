#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    using socket_type = SOCKET;
#else
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1

    using socket_type = int;
#endif

namespace Socket
{
    ////////////////////////
    ///// requests.cpp /////
    ////////////////////////

    void handle_request
    (
        const socket_type &client,
        const int         &max_request_length
    );

    //////////////////////////////
    ///// socket.handler.cpp /////
    //////////////////////////////

    bool create_socket_server
    (
        const std::string &address,
        const int &max_retries,
        const int &port,
        socket_type &server_socket
    );
}

#endif
