/*
 * Copyright (C) 2016 Ajado Aduragbemi Joseph <agbemi19@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#include "socket_interface.hpp"

#include<iostream>

namespace iosocket{
    
    int SocketInterface::getFD(FD fd) const{
		return (fd == FD::SOCK) ? sock_fd : conn_fd;
    }
    

	int SocketInterface::getFamily() const{
		return m_family;
	}
    
    void SocketInterface::setFamily(int family){ m_family = family; }
    void SocketInterface::setSockType(int sock_type){ m_sock_type = sock_type; }
    void SocketInterface::setProtocol(int protocol){ m_protocol = protocol; }
    
	int SocketInterface::setHostInfo(const std::string& addr,
                                     const std::string& port,
                                     bool to_bind,
                                     const int proto){
		addrinfo hint;
		memset(&hint, 0, sizeof(addrinfo));
        
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = m_sock_type;
		hint.ai_protocol = proto;
        if(to_bind) hint.ai_flags = AI_PASSIVE;
        
        int rc  = getaddrinfo((addr == "") ? nullptr : addr.c_str(),
                              port.c_str(),
                              &hint,
                              &host_info);
        
        if(rc == ADDRINFONOTSET) std::cerr << gai_strerror(errno) << std::endl;
        
        return rc;
	}

	void SocketInterface::cleanUpOnEnd(const bool clean_up){ m_clean_up = clean_up; }

	int SocketInterface::shutdown(const int how) const{ return ::shutdown(conn_fd, how); }
    
	int SocketInterface::socket(){
		#ifdef WIN32
			WSADATA wsa_data;
			if(WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
				throw WSAException("WSAStartup failed");
			}
		#endif

        sock_fd = ::socket(m_family,
                           m_sock_type,
                           m_protocol);
        
        if(sock_fd < 0) return SOCKET_BAD;
        
        return SOCKET_OK;
    }


    int SocketInterface::bind(PAddrInfo aI) const{
		return ::bind(sock_fd,
					  aI->ai_addr,
					  aI->ai_addrlen);
    }
    
    
    int SocketInterface::bind(const std::string& addr,
							  const std::string& port){
		if(setHostInfo(addr, port,true) == ADDRINFONOTSET){
			BindException(gai_strerror(errno));
		}
        
        m_family = host_info->ai_family;
        
        if(socket() != SOCKET_OK) throw SocketException(strerror(errno));
        
        return bind(host_info);
    }
    
    
    int SocketInterface::accept(){
        
        socklen_t len = sizeof(sockaddr_in6);
        conn_fd = ::accept(sock_fd,
						   &client_addr,
						   &len);
        
        return conn_fd;
    }
    
    
    int SocketInterface::close(FD fd) const{
		#ifdef WIN32
			return closesocket((fd==FD::SOCK) ? sock_fd : conn_fd);
		#else
			return ::close((fd==FD::SOCK) ? sock_fd : conn_fd) ;
		#endif
    }
    
            
    sockaddr SocketInterface::getClient() const{
        return client_addr;
    }
    
    
    int SocketInterface::connect(const std::string& hname,
								 const std::string& port){
		
		if(setHostInfo(hname, port) != ADDRINFOSET) return SOCKET_BAD;
        
        m_family = host_info->ai_family;
        
        if(socket() != SOCKET_OK) throw SocketException(strerror(errno));
        
        conn_fd = sock_fd;

        return ::connect(conn_fd,
						 host_info->ai_addr,
						 host_info->ai_addrlen);
    }


	int SocketInterface::setSockOpt(const uint32_t fam, int level, int option, socklen_t val){
		return ::setsockopt(fam,
							level,
							option,
							(const char*)&val,
							sizeof(int));
	}


	int SocketInterface::listen(const uint16_t& q_size){
		return ::listen(sock_fd, static_cast<int>(q_size));
	}


	std::string SocketInterface::recv(const uint16_t& size){
        size_t t_len = 0;
        return recv(size, t_len);
    }


	std::string SocketInterface::getHostName() const{
		char *name_c = new char[INET_ADDRSTRLEN];

		std::string name_s = (gethostname(name_c, INET_ADDRSTRLEN) == 0) ? std::string(name_c) : "";

		delete[] name_c;

		return name_s;
	}


	void* SocketInterface::getInAddr(const PSockAddr sa) const{
		return (sa->sa_family == AF_INET) ? static_cast<void*>(&((sockaddr_in*) sa)->sin_addr) :
											static_cast<void*>(&((sockaddr_in6*) sa)->sin6_addr);
	}

	uint16_t SocketInterface::getPort(const PSockAddr sa) const{
		return (sa->sa_family == AF_INET) ? htons(((sockaddr_in*) sa)->sin_port) :
											htons(((sockaddr_in6*) sa)->sin6_port);
	}

	PHostent SocketInterface::gethostbyname(std::string name) const{
		return ::gethostbyname(name.c_str());
	}
    
    const PAddrInfo SocketInterface::getHostInfo() const{
        return host_info;
    }

}
