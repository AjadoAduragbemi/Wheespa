
#include "handle_received.hpp"
#include "binary_tree.cc"

namespace wheespa{
    
    namespace wheespa_server{
        int HandleReceived::handle(WheespaServerTree& tree){
            int rc = -1;
            switch(m_conn_type){
                case ConnectType::ADMINISTRATE:
                    rc = handleAdmin(tree);
                    break;
                case ConnectType::INTERNAL:
                    rc = handleInternal(tree);
                    break;
                case ConnectType::EXTERNAL:
                    break;
                case ConnectType::LIST_ONLINE:
                    rc = handleListOnline(tree);
                    break;
                case ConnectType::INVALID:
                    m_user->data.sock_stream->write("WHEESPA_INVALID_CONNECT_TYPE");
            }
            
            return rc;
        }
        
        
        int HandleReceived::handleInternal(WheespaServerTree& tree) const{
            const Node<WheespaConnected>* found;
            
            if(m_user->data.registered) found = tree.find(m_result[2]);
            else found = tree.find(m_user->data.connect_to);
            
            if(found && !found->overwrite){
                std::ostringstream to_send;
                to_send << m_user->data.key << ": " << m_result[3] << "::[" << std::time(0) << "]"; 
                found->data.sock_stream->write(to_send.str());
            }else{
                m_user->data.sock_stream->write("WHEESPA_USER_OFFLINE");
                return -1;
            }
            return 0;
        }
        
        
        int HandleReceived::handleAdmin(WheespaServerTree& tree){
            if(!m_user->data.is_admin){
                m_user->data.sock_stream->write("WHEESPA_UNAUTHORIZED");
                return -1;
            }else{
                switch(m_admin_cmd){
                    case AdminCommand::ACCEPT_REGISTRATION:{
                        std::string tmp = m_result[4];
                        int id = std::strtol(tmp.c_str(), nullptr, 10);
                        std::ostringstream oss_query;
                        oss_query << "Insert into Registered(W_USER, W_PASS) "
                                  << "Select W_USER, W_PASS from PendingRegistrations where W_ID = " << id
                                  << "; Delete from PendingRegistrations where W_ID = " << id;
                        m_db.execQuery(oss_query.str());
                        break;
                    }
                    case AdminCommand::ADD_ADMIN:{
                        std::string tmp = m_result[4];
                        int id = std::strtol(tmp.c_str(), nullptr, 10);
                        std::ostringstream oss_query;
                        oss_query << "Insert into Administrators(W_USER, W_PASS) "
                                  << "Select W_USER, W_PASS from Registered where W_ID = " << id;
                        m_db.execQuery(oss_query.str());
                        break;
                    }
                    case AdminCommand::GET_ADMINS:{
                        m_db.setTableStructure("Administrators");
                        std::ostringstream oss;
                        oss << m_db.getTableStructure();
                        m_user->data.sock_stream->write(oss.str());
                        break;
                    }
                    case AdminCommand::GET_PENDING:{
                        m_db.setTableStructure("PendingRegistrations");
                        std::ostringstream oss;
                        oss << m_db.getTableStructure();
                        m_user->data.sock_stream->write(oss.str());
                        break;
                    }
                    case AdminCommand::GET_REGISTERED:{
                        m_db.setTableStructure("Registered");
                        std::ostringstream oss;
                        oss << m_db.getTableStructure();
                        m_user->data.sock_stream->write(oss.str());
                        break;
                    }
                    case AdminCommand::REMOVE_ADMIN:{
                        std::string tmp = m_result[4];
                        int id = std::strtol(tmp.c_str(), nullptr, 10);
                        m_db.deleteRecord("Administrators", "W_ID = " + id);
                        break;
                    }
                    case AdminCommand::REMOVE_REGISTERED:{
                        std::string tmp = m_result[4];
                        int id = std::strtol(tmp.c_str(), nullptr, 10);
                        m_db.deleteRecord("Registered", "W_ID = " + id);
                        break;
                    }
                }
            }
            return 0;
        }
        
        
        int HandleReceived::handleExternal(WheespaServerTree& tree) const{
            
            return 0;
        }
        
        
        int HandleReceived::handleListOnline(WheespaServerTree& tree) const{
            if(!m_user->data.registered){
                m_user->data.sock_stream->write("WHEESPA_NOT_REGISTERED");
                return -1;
            }
            
            WheespaServerTree in_tree = tree;
            in_tree.setDataArray();
            std::list<WheespaConnected> onlineUsers = in_tree.getDataArray();
            std::ostringstream oss;
            oss << onlineUsers.size();
            
            m_user->data.sock_stream->write(oss.str());
            for(WheespaConnected user : onlineUsers){
                m_user->data.sock_stream->write(user.key);
            }
            
            return 0;
        }
    }
}