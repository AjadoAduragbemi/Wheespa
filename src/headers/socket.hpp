
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socket_interface.hpp"

namespace iosocket{
    
	class Socket : public SocketInterface{
        
	public:
		Socket() {}
		
        Socket(int family, int sock_type, int protocol=0){
            setFamily(family);
			setSockType(sock_type);
			setProtocol(protocol);
        }
        
        int send(std::string) const;
        
        std::string recv(const uint16_t, size_t&) const;
        
    };
    
}

#endif
