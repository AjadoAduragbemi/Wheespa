#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include "socket_interface.hpp"


namespace iosocket{
		
	class SocketStream {
		const PSocketInterface m_sock;
		
	public:
		
		SocketStream(const PSocketInterface sp) : m_sock(sp){}

		int write(const std::string&) const;

		std::string read(const uint16_t) const;
		std::string read(const uint16_t, const uint16_t) const;
		void flush() const;
		PSocketInterface getSocketInterface();
		void close() const;
	};
}

#endif