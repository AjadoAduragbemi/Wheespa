#ifndef WHEESPA_HANDLE_RECEIVED
#define WHEESPA_HANDLE_RECEIVED

#include <regex>
#include <sstream>
#include "wheespa_tree.hpp"

namespace wheespa{
    
    class HandleReceived{
        
        std::smatch m_result;
        Node<WheespaConnected>* m_user;
        
        enum class ConnectType{
            ADMINISTRATE, INTERNAL, LIST_ONLINE, EXTERNAL, INVALID
        }m_conn_type;
        
        enum class AdminCommand{
            GET_PENDING,
            GET_ONLINE,
            GET_REGISTERED,
            GET_ADMINS,
            ADD_ADMIN,
            ACCEPT_REGISTRATION,
            REMOVE_REGISTERED
        }m_admin_cmd;
        
        int handleInternal(WheespaServerTree&);
        int handleAdmin(WheespaServerTree&);
        int handleExternal();
        int handleListOnline();
        
    public:
        HandleReceived(const std::string& recvd, Node<WheespaConnected>* user_node){
            m_user = user_node;
            bool match_found = std::regex_match(recvd, m_result, std::regex("([A-Z_]+)::([a-zA-Z0-9.:]*):::(.*)::::([0-9]+)"));
            if(match_found){
                if(m_result[1] == "WHEESPA_ADMINISTRATE") m_conn_type = ConnectType::ADMINISTRATE;
                else if(m_result[1] == "WHEESPA_INTERNAL") m_conn_type = ConnectType::INTERNAL;
                else if(m_result[1] == "WHEESPA_EXTERNAL")m_conn_type = ConnectType::EXTERNAL;
                else if(m_result[1] == "WHEESPA_LIST_ONLINE")m_conn_type = ConnectType::LIST_ONLINE;
                else m_conn_type = ConnectType::INVALID;
            }else{
                m_user->data.sock_stream->write("WHEESPA_FORMAT_ERROR");
            }
        }
        
        int handle(WheespaServerTree&);
    };
    
}

#endif