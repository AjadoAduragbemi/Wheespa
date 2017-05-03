
#include "socket.hpp"


namespace iosocket{
        
    const int Socket::send(std::string msg) const{
        if(msg.empty() || *msg.rbegin() != '\n'){
            msg.push_back('\n');
        }

        return ::send(conn_fd,
                      msg.c_str(),
                      msg.length(),
                      0);
    }
    
    
    std::string Socket::recv(const uint16_t size,
                             size_t& byte_rcvd) const{
		#if __cplusplus < 201103L
			char *msg_buf = new char[size];
			memset(msg_buf, 0, size);
		#else
			char *msg_buf = new char[size]{0};
		#endif
        
        std::string msg("");
		
        byte_rcvd = ::recv(conn_fd,
                           msg_buf,
                           size,
						   0);

		if(byte_rcvd == 0) throw ConnectionClosedException("connection closed by peer");
		
        if(byte_rcvd != -1){
            msg.assign(msg_buf, byte_rcvd);
		}
		
		delete[] msg_buf;
        
        /* strip newline from message */
        if(!msg.empty() && *msg.rbegin() == '\n')
            msg.pop_back();
        
        
        return msg;
    }
    
    
}
