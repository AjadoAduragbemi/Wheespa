#include "handle_request.hpp"
namespace wheespa{
	int wheespa_server::ParseRequest::anonymous_count = 0;
		bool wheespa_server::ParseRequest::handle(){
		if(rt_map.find(m_req_type) == rt_map.end()){
			m_ss.write("::WHEESPA_INVALID_REQUEST_TYPE");
			return false;
		}
		
		bool ret;
		switch(getValue(rt_map, m_req_type)){
			case RequestType::AdminLogin:
				ret = adminLogin();
				break;
			case RequestType::AnonymousConnect:
				ret = anonymousConnect();
				break;
			case RequestType::DisconnectLogin:
				ret = disconnectLogin();
				break;
			case RequestType::Login:
				ret = login();
				break;
			case RequestType::Register:
				ret = registerUser();
				break;
		}
		
		if(ret && getValue(rt_map, m_req_type) != RequestType::Register){
			m_conn.sock_stream = std::make_shared<wheespa_socket::SocketStream>(m_ss.getSocketInterface());
		}
		
		return ret;
	}
	
	bool wheespa_server::ParseRequest::adminLogin(){
		bool auth = login();
		if(auth){
			auth = false;
			std::string query = "Select * from Administrators where W_USER = '" + getValue(m_param, "username") + "'";
			w_db.execQuery(query, 
					   [](void* arg, int col_cnt, char** rows, char** cols){
							bool *p = static_cast<bool*>(arg);
							*p = true;
							return 0;
						}, 
						&auth);
			m_conn.is_admin = auth;
		}
		return auth;
	}
	
	bool wheespa_server::ParseRequest::anonymousConnect(){
		if((m_param.find("alias") == m_param.end()) || (m_param.find("connectto") == m_param.end())){
			m_ss.write("::WHEESPA_INVALID_REQUEST_TYPE_PARAMETERS");
			return false;
		}
		
		std::ostringstream oss;
		oss << getValue(m_param, "alias") << std::setw(4) << std::setfill('0') << ++anonymous_count;
		
		m_conn.connect_to = getValue(m_param, "connectto");
		m_conn.key = getValue(m_param, "alias");
		
		return true;
	}
	
	
	bool wheespa_server::ParseRequest::disconnectLogin(){
		return (disconnect_requested = login());
	}
	
	
	bool wheespa_server::ParseRequest::login(){
		if((m_param.find("username") == m_param.end()) || (m_param.find("password") == m_param.end())){
			m_ss.write("::WHEESPA_INVALID_REQUEST_TYPE_PARAMETERS");
			return false;
		}
		
		bool user_exists = false;
		if(getValue(m_param, "password").length() < 6){			m_ss.write("::WHEESPA_INVALID_PASSWORD");		}else{			SHA<SHA_DIGEST_LENGTH> sha(getValue(m_param, "password"));			user_exists = authenticate(getValue(m_param, "username"), sha.getOutput());
			m_conn.registered = user_exists;
			m_conn.key = getValue(m_param, "username");		}
		
		return user_exists;	}
	
	bool wheespa_server::ParseRequest::registerUser(){ 
		if((m_param.find("username") == m_param.end()) ||
		   (m_param.find("password") == m_param.end()) ||
		   (m_param.find("email") == m_param.end()))
		{
			m_ss.write("::WHEESPA_INVALID_REQUEST_TYPE_PARAMETERS");
			return false;
		}
		
		bool registered;
		std::smatch tmp_mr;
		
		if(getValue(m_param, "username").length() > 32){
			m_ss.write("::WHEESPA_INVALID_USERNAME");
		}else if(getValue(m_param, "password").length() < 6){
			m_ss.write("::WHEESPA_INVALID_PASSWORD");
		}else if(!std::regex_match(m_param.find("email")->second, tmp_mr, std::regex(R"([\w]+@[\w\.]+(\.[a-z]+)?)"))){
			m_ss.write("::WHEESPA_INVALID_EMAIL");
		}else if(checkExists(getValue(m_param, "username"))){
			m_ss.write("::WHEESPA_USER_ALREADY_EXIST");
		}else{
			//https://stackoverflow.com/questions/35102371/how-to-get-random-salt-from-openssl-as-stdstring			
			if( w_db.insert("PendingRegistrations",
							"W_USER, W_PASS, W_MAIL, W_KEY",
							{	getValue(m_param, "username"),
								SHA<SHA_DIGEST_LENGTH>(getValue(m_param, "password")).getOutput(),
								getValue(m_param, "email"),
								""
							})){
				registered = true;
			}else /*w_db.printErrMsg()*/;
		}
		
		return registered;
	}
	
	
	bool wheespa_server::ParseRequest::authenticate(const std::string& user, const std::string& pass){		std::string query = "Select * from Registered where "							"W_USER = \"" + user + "\" and W_PASS = \"" + pass + "\"";
		
		bool exists = false;
		w_db.execQuery(query, [](void* arg, int col_cnt, char** rows, char** cols){			bool *p = static_cast<bool*>(arg);			*p = true;			return 0;		}, &exists);
		
		return exists;	}
	
	
	bool wheespa_server::ParseRequest::checkExists(const std::string& user){		bool exists = false;		
		std::string query = "Select R.W_USER, P.W_USER from Registered R, PendingRegistrations P "							"where R.W_USER = \"" + user + "\" OR P.W_USER = \"" + user + "\"";
		
		w_db.execQuery(query, 					   [](void* arg, int col_cnt, char** rows, char** cols){						  	bool *p = static_cast<bool*>(arg);							*p = true;							return 0;						}, 						&exists);
		
		return exists;	}
	
	
	const WheespaConnected& wheespa_server::ParseRequest::getConnected(){	return m_conn; }
	
	bool wheespa_server::ParseRequest::requestedDisconnect(){ return disconnect_requested; }
}