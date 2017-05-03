
#include "handle_received.hpp"
#include "ctime"

namespace wheespa{
    
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
            default:
                m_user->data.sock_stream->write("WHEESPA_INVALID_CONNECT_TYPE");
        }
        
        return rc;
    }
    
    
    int HandleReceived::handleInternal(WheespaServerTree& tree){
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
        }
        
        
        return 0;
    }
}