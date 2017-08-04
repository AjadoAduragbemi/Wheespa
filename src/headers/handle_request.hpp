#ifndef WHEESPA_HANDLE_REQUEST_HPP
#define WHEESPA_HANDLE_REQUEST_HPP

#include <iostream>
#include <regex>
#include "wheespa_socket.hpp"
#include "wheespa_base.hpp"
#include "wheespa_tree.hpp"
#include "sha.hpp"

namespace wheespa{
	
	namespace wheespa_server{

		enum class ClientRequest{
			AdminLogin,
			AnonymousConnect,
			ChangePassword,
			CheckPending,
			InvalidRequest,
			Login,
			Register
		};			

		class HandleRequest{

			wheespa_socket::SocketStream m_ss;
			ClientRequest m_req;
			wheespa_base::FlexDB w_db;
			WheespaConnected m_conn;
			
			std::string request;

			bool login(std::string&);
			bool registerUser();
			bool anonymousConnect(std::string&, std::string&);
			bool adminLogin(std::string&);
			bool checkExists(const std::string&);
			bool checkLoggedIn();
			bool authenticate(const std::string&, const std::string&);
			
			static int anonymous_count;
			
			template<typename... Args>
			void unpack<>();

		public:

			HandleRequest(const wheespa_socket::SocketStream& ss,
						  const std::smatch& mr,
						  std::shared_ptr<std::regex> s_rex,
						  const std::string& dbfn) : m_ss(ss),
													 w_db(dbfn){
				
			}
			void handle();
			const WheespaConnected& getConnected();
			
			
		};

	}

}

#endif
