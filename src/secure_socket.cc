
#include "secure_socket.hpp"

namespace iosocket{
	
	void SecureSocket::initialize(){
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
	}


	void SecureSocket::initializeCTXServer(){

		switch(m_ssl_proto_version){
		
		case PROTOCOL_TLSv1:
			m_ssl_ctx = SSL_CTX_new(TLSv1_method());
			break;
		
		case PROTOCOL_TLSv1_1:
			m_ssl_ctx = SSL_CTX_new(TLSv1_1_server_method());
			break;
		
		case PROTOCOL_TLSv1_2:
			m_ssl_ctx = SSL_CTX_new(TLSv1_2_server_method());
			break;
		
		default:
			/* Nothing yet.*/;
		}
	}


	void SecureSocket::LoadCertificate(const std::string& file){
		
		if(SSL_CTX_use_certificate_file(m_ssl_ctx,
										file.c_str(),
										SSL_FILETYPE_PEM) <= 0) ERR_print_errors_fp(stderr);;
		
		if(SSL_CTX_use_PrivateKey_file(m_ssl_ctx,
									   file.c_str(),
									   SSL_FILETYPE_PEM) <= 0) ERR_print_errors_fp(stderr);;
	}


	int SecureSocket::accept(){

		initializeCTXServer();
		
		LoadCertificate(m_certificate);
		
		socklen_t len = sizeof(sockaddr_in6);
		
        conn_fd = ::accept(sock_fd,
						   &client_addr,
						   &len);
		
		m_ssl = SSL_new(m_ssl_ctx);
		
		SSL_set_fd(m_ssl, conn_fd);
		
		if(::SSL_accept(m_ssl) <= 0){
			ERR_print_errors_fp(stderr);
			SSL_shutdown(m_ssl);
			close(FD::CONN);
			conn_fd = -1;
		}

		return conn_fd;
	}

	
	int SecureSocket::connect(const std::string& hname, const std::string& port){
		if(setHostInfo(hname, port) != ADDRINFOSET) return CONN_FAILED;

		conn_fd = sock_fd;

        int rc = ::connect(this->sock_fd,
						   host_info->ai_addr,
						   host_info->ai_addrlen);
		return rc;
	}


	std::string SecureSocket::recv(const uint16_t size, size_t& byte_rcvd) const{ 
		#if __cplusplus < 201103L
			char *msg_buf = new char[size];
			memset(msg_buf, 0, size);
		#else
			char *msg_buf = new char[size]{0};
		#endif
        
        std::string msg("");
		
		if(m_ssl != nullptr) byte_rcvd = SSL_read(m_ssl, msg_buf, size);
		else byte_rcvd = 0;

		if(byte_rcvd == 0) throw ConnectionClosedException("connection closed by peer");
		
        if(byte_rcvd != -1){
            msg.assign(msg_buf, byte_rcvd);
		}else{
			ERR_print_errors_fp(stderr);
		}
		
		delete[] msg_buf;

				
        /* strip newline from message */
        if(!msg.empty() && *msg.rbegin() == '\n')
            msg.pop_back();
        
        
        return msg;
	}


	const int SecureSocket::send(std::string msg) const {
		if(msg.empty() || *msg.rbegin() != '\n'){
            msg.push_back('\n');
        }
		
		return SSL_write(m_ssl, msg.c_str(), msg.length());
	}

}
