#ifndef IOSOCKET_HPP
#define IOSOCKET_HPP

#include<cstdint>
#include<cstring>
#include<cstdlib>
#include<iostream>

#include "socket_exception.hpp"


#ifdef WIN32
	#include<ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include<sys/socket.h>
    #include<arpa/inet.h>
    #include<netinet/in.h>
    #include<netdb.h>
    #include<unistd.h>
#endif


#define SOCKET_OK   0
#define SOCKET_BAD -1

#define SOCKET_API

#define STREAM_SEND_BAD SOCKET_BAD
#define STREAM_RECV_BAD SOCKET_BAD

#define BIND_FAILED SOCKET_BAD
#define BIND_SUCCESS SOCKET_OK

#define CONN_FAILED SOCKET_BAD
#define CONN_OK SOCKET_OK

#define ADDRINFOSET SOCKET_OK
#define ADDRINFONOTSET SOCKET_BAD


#endif