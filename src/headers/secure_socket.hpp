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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef SECURE_SOCKET_HPP
#define SECURE_SOCKET_HPP

#include "socket_interface.hpp"
#include "openssl/ssl.h"
#include "openssl/err.h"

#pragma comment(lib, "libssl")

namespace iosocket{

	enum ProtocolMethod{
		PROTOCOL_SERVER_METHOD,
		PROTOCOL_CLIENT_METHOD,
		PROTOCOL_METHOD
	};

	class SecureSocket : public SocketInterface {
		SSL_CTX* m_ssl_ctx;
		SSL* m_ssl;
		ProtocolMethod m_ssl_method;
		std::string m_pemfile;

		void initialize();
		int initializeSSL_CTX();

	public:
		SecureSocket() {}
		
        SecureSocket(const int family,
					 const int sock_type,
					 const int protocol,
					 std::string& certificate,
					 ProtocolMethod ssl_method = ProtocolMethod::PROTOCOL_METHOD) : m_ssl_method(ssl_method),
																					m_pemfile(certificate){
			setFamily(family);
			setSockType(sock_type);
			setProtocol(protocol);
			
			initialize();
        }
		
		~SecureSocket(){
			SSL_shutdown(m_ssl);
			ERR_free_strings();
			EVP_cleanup();
			
			SSL_CTX_free(m_ssl_ctx);
			SSL_free(m_ssl);
		}

		int accept();

		int connect(const std::string&,
					const std::string&);

		std::string recv(const uint16_t, size_t&) const;
        
		int send(std::string) const;
		
		int shutdown(int) const;
	};

}

#endif
