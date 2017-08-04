
#include "handle_signal.hpp"

namespace wheespa{
    
    
    int SignalHandler::r_signal = 0;
    
    void SignalHandler::handler(int sig_num){
        r_signal = sig_num;
    }
    
    void SignalHandler::sigaction(int signal){
        ::sigaction(signal, &s_action, nullptr);
    }
    
}