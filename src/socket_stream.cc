
#include "socket_stream.hpp"
#include <thread>

namespace iosocket{

	int SocketStream::write(const std::string& msg) const{

		if(m_sock->getFD(FD::CONN) <= 0) return STREAM_SEND_BAD;
        
		/* wait a byte :D */
		std::this_thread::sleep_for(std::chrono::milliseconds(256));
		
		return m_sock->send(msg);
	}


	std::string SocketStream::read(const uint16_t msg_size) const{
		size_t recv_len;
		std::string msg = m_sock->recv(msg_size, recv_len);
		return msg;
	}
	
	//Not valid for SecureSockets
	void SocketStream::flush() const{

		fd_set rfds;
		timeval t_out;
		
		int conn_fd = m_sock->getFD(FD::CONN);
		
		FD_ZERO(&rfds);
		FD_SET(conn_fd, &rfds);

		t_out.tv_sec = 0;
		t_out.tv_usec = 10000; /* t_out = 0.01secs */
		
		/* Clear buffer by reading it all then deleting */
		/* This is just to make sure the buffer is empty after a read */
		/* The only portable way i could think of now */
		if(select(conn_fd+1, &rfds, NULL, NULL, &t_out) != -1){

			if(FD_ISSET(conn_fd, &rfds)){
				
				socklen_t buf_len = 0, optlen = sizeof(int);
				getsockopt(conn_fd, SOL_SOCKET, SO_SNDBUF, (char*)&buf_len, &optlen);

				char *msg_buf = new char[buf_len];
				memset(msg_buf, 0, buf_len);
				
				::recv(conn_fd, msg_buf, buf_len, 0);
				
				delete[] msg_buf;
			}
		}
	}
	
	// ttl should be in seconds
	std::string SocketStream::read(const uint16_t size, const uint16_t ttl) const{
		size_t recv_len;
		std::string msg;
		
		fd_set rfds;
		
		int conn_fd = m_sock->getFD(FD::CONN);
		
		FD_ZERO(&rfds);
		FD_SET(conn_fd, &rfds);
		
		timeval t_out;
		t_out.tv_sec = ttl;
		t_out.tv_usec = 10000;
		
		if(select(conn_fd+1, &rfds, NULL, NULL, &t_out) != -1){

			if(FD_ISSET(conn_fd, &rfds)){
				
				msg = read(size);
			}
		}
		return msg;
	}
	
	PSocketInterface SocketStream::getSocketInterface(){
		return m_sock;
	}
	
	void SocketStream::close() const{
		m_sock->close(FD::CONN);
	}
}
