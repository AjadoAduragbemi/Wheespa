#ifndef SECURE_SOCKET_HPP
#define SECURE_SOCKET_HPP

#include "socket_interface.hpp"
#include "openssl/ssl.h"
#include "openssl/err.h"

#pragma comment(lib, "libssl")

namespace iosocket{

	enum ProtocolVersion{
		PROTOCOL_TLSv1,
		PROTOCOL_TLSv1_1,
		PROTOCOL_TLSv1_2
	};

	class SecureSocket : public SocketInterface {
		SSL_CTX* m_ssl_ctx;
		SSL* m_ssl;
		ProtocolVersion m_ssl_proto_version;
		std::string m_certificate;

		void initialize();
		void initializeCTXServer();
		void initializeCTXClient();
		void LoadCertificate(const std::string&);
		void SSL_accept();
		void SSL_Shutdown();

	public:
		SecureSocket() {}
		
        SecureSocket(const int family,
					 const int sock_type,
					 const int protocol,
					 std::string& certificate,
					 ProtocolVersion proto_version = PROTOCOL_TLSv1) : m_ssl_proto_version(proto_version),
					 												   m_certificate(certificate){
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
        
		const int send(std::string) const;

	};

}

#endif
