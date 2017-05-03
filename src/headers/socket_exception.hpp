#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include<exception>
#include<sstream>

namespace iosocket{
	

	class SocketException : public std::exception {

		std::string m_what;

	protected:
		enum class ExceptionType{
			SocketException,
			BindException,
			ConnectionClosedException,
			WSAException,
			SSLCertificateError
		} m_type;
		
		void defineWhatType();

		SocketException& operator=(SocketException const&);

	public:
		
		SocketException() : m_what(""){}
		SocketException(const std::string what, ExceptionType type) : m_what(what), m_type(type){
			defineWhatType();
		}

		SocketException(const std::string what) : m_what(what), m_type(ExceptionType::SocketException){
			defineWhatType();
		}

		const char* what() const throw();
	};


	class ConnectionClosedException : public SocketException {

	public:

		ConnectionClosedException(const std::string what) :
			SocketException(what, ExceptionType::ConnectionClosedException) {
			
		}

	};


	class WSAException : public SocketException {

	public:
		WSAException(std::string what) : 
			SocketException(what, ExceptionType::WSAException) {}
	};


	class BindException : public SocketException {

	public:
		BindException(std::string what) : 
			SocketException(what, ExceptionType::BindException) {}
	};
}

#endif