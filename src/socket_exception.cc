#include "socket_exception.hpp"

namespace iosocket{

	const char* SocketException::what() const throw(){
		return (m_what != "") ? m_what.c_str() : "Unknown exception";
	}


	void SocketException::defineWhatType(){
		switch (m_type)
		{
			case ExceptionType::SocketException:
				m_what.insert(0, "SocketException: ");
				break;
			case ExceptionType::ConnectionClosedException:
				m_what.insert(0, "ConnectionClosedException: ");
				break;
			case ExceptionType::WSAException:
				m_what.insert(0, "WSAException: ");
				break;
			case ExceptionType::BindException:
				m_what.insert(0, "BindException: ");
				break;
			default:
				m_what.insert(0, "UnknownException: ");
				break;
		}
	}

}