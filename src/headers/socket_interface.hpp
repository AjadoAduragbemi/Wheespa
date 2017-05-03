#ifndef SOCKET_INTERFACE_HPP
#define SOCKET_INTERFACE_HPP

#include "iosocket.hpp"

namespace iosocket{

    typedef struct addrinfo* PAddrInfo;
    typedef struct sockaddr* PSockAddr;
	typedef struct hostent*  PHostent;
	
	enum class FD{
		SOCK,
		CONN
	};

	class SocketInterface{

		int m_family,
            m_sock_type,
			m_protocol;
		bool m_clean_up;

	protected:
        int sock_fd, conn_fd;
        struct sockaddr client_addr;
		PAddrInfo host_info;

		int setHostInfo(const std::string&,
						const std::string&,
						bool = false,
						const int = IPPROTO_TCP);
		
		void setFamily(int);
		void setSockType(int);
		void setProtocol(int);
		
	public:

		SocketInterface() : sock_fd(0), 
							conn_fd(0),
							m_clean_up(false),
							m_family(0),
							m_sock_type(0),
							m_protocol(0),
							client_addr() {}

		~SocketInterface(){
			#ifdef WIN32
				if(m_clean_up) WSACleanup();
			#endif

			freeaddrinfo(host_info);
		}

		int socket();
		
		int bind(const std::string&,
				 const std::string&);
		
		int bind(PAddrInfo aI) const;
        
		virtual int accept();
        
        virtual int connect(const std::string&,
							const std::string&);
        
		int listen(const uint16_t&);
        
        virtual const int send(std::string) const = 0;
        
        virtual std::string recv(const uint16_t, size_t&) const = 0;
        
		int close(FD fd) const;
		
		void cleanUpOnEnd(bool);

		std::string getHostName() const;
		
		int getFD(FD) const;

		int setSockOpt(const uint32_t, int, int, socklen_t = 1);

        sockaddr getClient() const;

		std::string recv(const uint16_t&);
		
		void* getInAddr(const PSockAddr) const;

		PHostent gethostbyname(std::string) const;

		uint16_t getPort(const PSockAddr sa) const;

		int getFamily() const;
		
		int shutdown(int how) const;
		
		const PAddrInfo getHostInfo() const;
	};

	
	typedef SocketInterface* PSocketInterface;
}
#endif
