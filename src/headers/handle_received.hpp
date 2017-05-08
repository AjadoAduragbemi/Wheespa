#ifndef WHEESPA_HANDLE_RECEIVED
#define WHEESPA_HANDLE_RECEIVED

#include <regex>
#include <sstream>
#include <list>
#include "wheespa_tree.hpp"
#include "wheespa_base.hpp"

namespace wheespa{
    namespace wheespa_server{
        class HandleReceived{
            
            std::smatch m_result;
            Node<WheespaConnected>* m_user;
            wheespa_base::FlexDB m_db;
            
            enum class ConnectType{
                ADMINISTRATE, INTERNAL, LIST_ONLINE, EXTERNAL, INVALID
            }m_conn_type;
            
            enum class AdminCommand{
                GET_PENDING,             //show all pending registrations
                GET_REGISTERED,
                GET_ADMINS,
                ADD_ADMIN,
                ACCEPT_REGISTRATION,    //accept a pending registration
                REMOVE_ADMIN,
                REMOVE_REGISTERED
            }m_admin_cmd;
            
            int handleInternal(WheespaServerTree&) const;
            int handleAdmin(WheespaServerTree&);
            int handleExternal(WheespaServerTree&) const;
            int handleListOnline(WheespaServerTree&) const;
            
        public:
            HandleReceived(const std::string& recvd, Node<WheespaConnected>* user_node, std::string& db_fname){
                m_user = user_node;
                m_db.open(db_fname);
                
                bool match_found = std::regex_match(recvd, m_result, std::regex("([A-Z_]+)::([a-zA-Z0-9.:_]*):::(.*)::::([0-9]+)"));
                
                if(match_found){
                    if(m_result[1] == "WHEESPA_ADMINISTRATE"){
                        m_conn_type = ConnectType::ADMINISTRATE;
                        if(m_result[2] == "GET_PENDING") m_admin_cmd = AdminCommand::GET_PENDING;
                        else if(m_result[2] == "GET_REGISTERED") m_admin_cmd = AdminCommand::GET_REGISTERED;
                        else if(m_result[2] == "GET_ADMINS") m_admin_cmd = AdminCommand::GET_ADMINS;
                        else if(m_result[2] == "ADD_ADMIN") m_admin_cmd = AdminCommand::ADD_ADMIN;
                        else if(m_result[2] == "ACCEPT_REGISTRATION") m_admin_cmd = AdminCommand::ACCEPT_REGISTRATION;
                        else if(m_result[2] == "REMOVE_ADMIN") m_admin_cmd = AdminCommand::REMOVE_ADMIN;
                        else if(m_result[2] == "REMOVE_REGISTERED") m_admin_cmd = AdminCommand::REMOVE_REGISTERED;
                        else m_user->data.sock_stream->write("WHEESPA_INVALID_COMMAND");
                    }else if(m_result[1] == "WHEESPA_INTERNAL") m_conn_type = ConnectType::INTERNAL;
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
}

#endif