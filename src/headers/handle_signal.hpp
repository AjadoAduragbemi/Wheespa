#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <signal.h>

namespace wheespa{
    
    class SignalHandler{
        
        static void handler(int);
        
        struct sigaction s_action;
    public:
        SignalHandler(){
            s_action.sa_flags = 0;
            s_action.sa_handler = handler;
            sigemptyset(&s_action.sa_mask);
        }
        
        static int r_signal;
        
        void sigaction(int);
    };
    
}

#endif