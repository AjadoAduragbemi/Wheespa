
#include "handle_request.hpp"

namespace wheespa{
	int wheespa_server::HandleRequest::anonymous_count = 0;
	
	void wheespa_server::HandleRequest::handle(){
		
		std::string wcr = m_ss.read(32);
		
		if(wcr.compare("WHEESPA_ADMIN_LOGIN") == 0) m_req = ClientRequest::AdminLogin;
		else if(wcr.compare("WHEESPA_ANONYMOUS") == 0) m_req = ClientRequest::AnonymousConnect;
		else if(wcr.compare("WHEESPA_CHANGE_PASSWORD") == 0) m_req = ClientRequest::ChangePassword;
		else if(wcr.compare("WHEESPA_CHECK_PENDING") == 0) m_req = ClientRequest::CheckPending;
		else if(wcr.compare("WHEESPA_REGISTER") == 0) m_req = ClientRequest::Register;
		else if(wcr.compare("WHEESPA_LOGIN") == 0) m_req = ClientRequest::Login;
		else m_req = ClientRequest::InvalidRequest;
		
		switch(m_req){
			case ClientRequest::AdminLogin:{
				int retries = 0;
				bool logged_in = false;

				while(retries < 3 && logged_in == false){
					std::string  username;
					if((logged_in = adminLogin(username))){
						m_conn.key = username;
						m_conn.registered = true;
						m_conn.is_admin = true;
						m_conn.sock_stream = std::make_shared<wheespa_socket::SocketStream>(m_ss.getSocketInterface());
					}else{
						m_ss.write("WHESSPA_INVALID_LOGIN");
					}
					++retries;
				}
				break;
			}
			case ClientRequest::AnonymousConnect:{
				std::string  user, pair;
				anonymousConnect(user, pair);
				m_conn.key = user;
				m_conn.sock_stream = std::make_shared<wheespa_socket::SocketStream>(m_ss.getSocketInterface());
				m_conn.connect_to = pair;
				break;
			}
			case ClientRequest::ChangePassword:{
				int retries = 0;
				bool logged_in = false;

				while(retries < 3 && logged_in == false){
					std::string  username;
					if((logged_in = login(username))){
						std::string newpasswd = m_ss.read(128);
						
					}else{
						m_ss.write("WHESSPA_INVALID_LOGIN");
					}
					++retries;
				}
				
				break;
			}
			case ClientRequest::CheckPending:
				break;
			
			case ClientRequest::Login:{
				int retries = 0;
				bool logged_in = false;

				while(retries < 3 && logged_in == false){
					std::string  username;
					if((logged_in = login(username))){
						m_conn.key = username;
						m_conn.registered = logged_in;
						m_conn.sock_stream = std::make_shared<wheespa_socket::SocketStream>(m_ss.getSocketInterface());
					}else{
						m_ss.write("WHESSPA_INVALID_LOGIN");
					}
					++retries;
				}
				
				break;
			}
			
			case ClientRequest::Register:
				if(registerUser()){
					m_ss.write("WHESSPA_REGISTER_SUCCESS");
				}
				break;
			
			default:
				m_ss.write("WHEESPA_INVALID_REQUEST");
		}
	}


	bool wheespa_server::HandleRequest::login(std::string &user){
		
		bool user_exists = false;
		std::string username = m_ss.read(128);
		std::string password = m_ss.read(128);

		size_t pos = username.find_first_of(" ,~`!@#$%^&*()_+=-|\\/><{}[]'\":;?");

		if(pos != -1){
			m_ss.write("WHEESPA_CONTAINS_BAD_CHAR");
		}else if(password.length() < 6){
			m_ss.write("WHEESPA_PASSWORD_LENGTH_BAD");
		}else{
			SHA<SHA_DIGEST_LENGTH> sha(password);
			user_exists = authenticate(username, sha.getOutput());
			user = username;
		}
		
		return user_exists;
	}


	bool wheespa_server::HandleRequest::authenticate(const std::string& user, const std::string& pass){
		
		std::string query = "Select W_ID from Registered where "
							"W_USER = \"" + user + "\" and W_PASS = \"" + pass + "\"";
		
		bool exists = false;
		w_db.execQuery(query, [](void* arg, int col_cnt, char** rows, char** cols){
			bool *p = static_cast<bool*>(arg);
			*p = true;
			return 0;
		}, &exists);
		
		return exists;
	}


	bool wheespa_server::HandleRequest::registerUser(){ 
		std::string username = m_ss.read(128);
		std::string password0 = m_ss.read(128);
		std::string password1 = m_ss.read(128);
		
		size_t pos = username.find_first_of(" ,~`!@#$%^&*()_+=-|\\/><{}[]'\":;?");
		
		if(pos != -1){
			m_ss.write("WHEESPA_CONTAINS_BAD_CHAR");
		}else if(password0.length() < 6){
			m_ss.write("WHEESPA_PASSWORD_LENGTH_BAD");
		}else if(username.compare("") == 0){
			m_ss.write("WHEESPA_EMPTY_USERNAME");
		}else if(password0.compare(password1) != 0){
			m_ss.write("WHEESPA_PASSWORD_DONT_MATCH");
		}else if(checkExists(username)){
			m_ss.write("WHEESPA_USERNAME_EXISTS");
		}else{
			std::vector<std::string> vca;
			vca.push_back(username);

			SHA<SHA_DIGEST_LENGTH> sha(password0);
			vca.push_back(sha.getOutput());

			return w_db.insert("PendingRegistrations", "W_USER, W_PASS", vca);
		}

		return false;
	}
	

	bool wheespa_server::HandleRequest::checkExists(const std::string& user){
		bool exists = false;

		std::string query = "Select R.W_USER, P.W_USER from Registered R, PendingRegistrations P "
							"where R.W_USER = \"" + user + "\" OR P.W_USER = \"" + user + "\"";

		w_db.execQuery(query, 
					   [](void* arg, int col_cnt, char** rows, char** cols){
						  	bool *p = static_cast<bool*>(arg);
							*p = true;
							return 0;
						}, 
						&exists);

		return exists;
	}
	
	bool wheespa_server::HandleRequest::anonymousConnect(std::string& user,std::string& pair){
		std::ostringstream oss;
		oss << "anonymous" << std::setw(3) << std::setfill('0') << ++anonymous_count;
		user = oss.str();
		pair = m_ss.read(128);
		return true;
	}
	
	
	bool wheespa_server::HandleRequest::adminLogin(std::string& username){
		bool auth = login(username);
		if(auth){
			auth = false;
			std::string query = "Select * from Administrators where W_USER = '" + username + "'";
			w_db.execQuery(query, 
					   [](void* arg, int col_cnt, char** rows, char** cols){
						  	bool *p = static_cast<bool*>(arg);
							*p = true;
							return 0;
						}, 
						&auth);
		}
		return auth;
	}
	
	
	const WheespaConnected& wheespa_server::HandleRequest::getConnected(){	return m_conn; }
}

