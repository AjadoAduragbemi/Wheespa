
#include "secure_socket.hpp"

#define ACCEPT_OK 	1
#define CHECK_ERROR_SSL(ret, foo)   if(!ret){ \
										fprintf(stderr, "[!] %s():\n", foo); \
										ERR_print_errors_fp(stderr); \
										return -1; \
									}
namespace iosocket{
	
	void SecureSocket::initialize(){
		SSL_library_init();
		OpenSSL_add_ssl_algorithms();
		SSL_load_error_strings();
		
		initializeSSL_CTX();
	}


	int SecureSocket::initializeSSL_CTX(){

		switch(m_ssl_method){
		
		case PROTOCOL_METHOD:
			m_ssl_ctx = SSL_CTX_new(TLS_method());
			break;
		
		case PROTOCOL_SERVER_METHOD:
			m_ssl_ctx = SSL_CTX_new(TLS_server_method());
			break;
		
		case PROTOCOL_CLIENT_METHOD:
			m_ssl_ctx = SSL_CTX_new(TLS_client_method());
			break;
		
		default:
			/* Nothing yet.*/;
		}
		
		CHECK_ERROR_SSL(m_ssl_ctx, "SSL_CTX_new");
		
		SSL_CTX_use_certificate_file(m_ssl_ctx, m_pemfile.c_str(), SSL_FILETYPE_PEM);
		
		SSL_CTX_use_PrivateKey_file(m_ssl_ctx, m_pemfile.c_str(), SSL_FILETYPE_PEM);
		
		CHECK_ERROR_SSL(SSL_CTX_load_verify_locations(m_ssl_ctx, m_pemfile.c_str(), nullptr), "SSL_CTX_load_verify_locations");
		
		CHECK_ERROR_SSL(SSL_CTX_check_private_key(m_ssl_ctx), "SSL_CTX_check_private_key");
		
		SSL_CTX_set_verify(m_ssl_ctx, SSL_VERIFY_PEER, nullptr);
		SSL_CTX_set_verify_depth(m_ssl_ctx, 1);
		
		SSL_CTX_set_min_proto_version(m_ssl_ctx, TLS1_VERSION);
		SSL_CTX_set_options(m_ssl_ctx, SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
		
		return 0;
	}
	

	int SecureSocket::accept(){
		//socklen_t len = sizeof(sockaddr_in6);
		
        conn_fd = ::accept(sock_fd, nullptr, nullptr); //::accept(sock_fd, &client_addr, &len);
		
		m_ssl = SSL_new(m_ssl_ctx);
		
		CHECK_ERROR_SSL(m_ssl, "SSL_new");
		
		SSL_set_fd(m_ssl, conn_fd);
				
		if(::SSL_accept(m_ssl) <= 0){
			ERR_print_errors_fp(stderr);
			//SSL_shutdown(m_ssl);
			//close(FD::CONN);//return -1;
			//SSL_free(m_ssl);
			//CHECK_ERROR_SSL(0, "SSL_accept");
			return -1;
		}
		
		return ACCEPT_OK;
	}

	
	int SecureSocket::connect(const std::string& hname, const std::string& port){
		if(setHostInfo(hname, port) != ADDRINFOSET) return CONN_FAILED;

		conn_fd = sock_fd;

        int rc = ::connect(this->sock_fd, host_info->ai_addr, host_info->ai_addrlen);
		return rc;
	}


	std::string SecureSocket::recv(const uint16_t size, size_t& byte_rcvd) const{ 
		#if __cplusplus < 201103L
			char *msg_buf = new char[size];
			memset(msg_buf, 0, size);
		#else
			char *msg_buf = new char[size]{0};
		#endif
        //char msg_buf[size];
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


	int SecureSocket::send(std::string msg) const {
		if(msg.empty() || *msg.rbegin() != '\n'){
            msg.push_back('\n');
        }
		
		return SSL_write(m_ssl, msg.c_str(), msg.length());
	}
	
	
	int SecureSocket::shutdown(int how) const{
		if(m_ssl) return SSL_shutdown(m_ssl);
		return -1;
	}
}
