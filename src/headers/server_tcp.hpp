
#ifndef SERVER_TCP_HPP
#define SERVER_TCP_HPP

#include "socket.hpp"

namespace iosocket{

	class ServerTCP : public Socket {

		PAddrInfo serv_info;

	public:
		ServerTCP() : serv_info(nullptr){}
		
		ServerTCP(const int family,
					const unsigned int port,
					const std::string node_name = "") : serv_info(nullptr){
			//if(init(family, port, node_name) == SOCKET_BAD) this->~ServerTCP();
		}

		~ServerTCP(){ freeaddrinfo(serv_info); }
		
		int init(
			const int,
			const unsigned int,
			const std::string = "");
		
		int listen(const unsigned short, bool reuse);
		
		PSockAddr getConnection();
				
		PAddrInfo getServInfo() const;
	};
}

#endif
