/*
 * Copyright (C) 2016 Ajado Aduragbemi Joseph <agbemi19@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef WHEESPA_HANDLE_REQUEST_HPP
#define WHEESPA_HANDLE_REQUEST_HPP

#include <iostream>
#include <regex>
#include <map>
#include "wheespa_socket.hpp"
#include "wheespa_base.hpp"
#include "wheespa_tree.hpp"
#include "sha.hpp"

namespace wheespa{
	
	namespace wheespa_server{

		enum class RequestType{
			AdminLogin,
			AnonymousConnect,
			DisconnectLogin,
			Login,
			Register,
		};
		// i prefer to use this function if and only if
		// key is present in input map
		template<typename K, typename V>
		V getValue(const std::map<K, V>& m, const K& k){
			return (m.find(k) != m.end()) ? m.find(k)->second : throw std::runtime_error("no such key was found!");
		}
		
		template<typename K, typename V>
		V getValue(const std::map<K, V>& m, const char k[]){
			return getValue(m, std::string(k));
		}
		
		class ParseRequest{

			wheespa_socket::SocketStream m_ss;
			wheespa_base::FlexDB w_db;
			WheespaConnected m_conn;
			std::map<std::string, std::string> m_param;
			const std::map<std::string, RequestType> rt_map = { std::pair<std::string, RequestType>("WHEESPA_ADMIN_LOGIN", RequestType::AdminLogin),
																std::pair<std::string, RequestType>("WHEESPA_ANONYMOUS", RequestType::AnonymousConnect),
																std::pair<std::string, RequestType>("WHEESPA_DISCONNECT_LOGIN", RequestType::DisconnectLogin),
																std::pair<std::string, RequestType>("WHEESPA_LOGIN", RequestType::Login),
																std::pair<std::string, RequestType>("WHEESPA_REGISTER", RequestType::Register)
															  };
			std::string m_req_type;
			static int anonymous_count;
			bool disconnect_requested;
			
			bool adminLogin();
			bool anonymousConnect();
			bool authenticate(const std::string&, const std::string&);
			bool checkExists(const std::string&);
			bool checkLoggedIn();
			bool disconnectLogin();
			bool login();
			bool registerUser();
			
		public:

			ParseRequest(const wheespa_socket::SocketStream& ss,
						  const std::string& req_type,
						  const std::string& req_params,
						  const std::shared_ptr<std::regex> s_rex,
						  const std::string& dbfn) 	: m_ss(ss), m_req_type(req_type), w_db(dbfn){
				auto iter = std::sregex_iterator(req_params.begin(), req_params.end(), *s_rex.get());
				while(iter != std::sregex_iterator()){
					//m_param.insert( std::pair<const std::string, const std::string>((*iter)[1], (*iter)[2]) );
					m_param[(*iter)[1]] = (*iter)[2];
					iter++;
				}
			}
			
			bool handle();
			const WheespaConnected& getConnected();
			bool requestedDisconnect();
			
		};

	}

}

#endif
