#ifndef WHEESPA_OPTS
#define WHEESPA_OPTS

#include <iostream>
#include "wheespa_socket.hpp"
#include "optionparser.h"

#define OPTGOOD		  0
#define OPTBAD 		 -1
#define NOCLIENTADDR -2

namespace wheespa{

	class WheespaOpts{
		public:
			bool verbose, listen;
			std::string certfile;
			std::string logfile;
			int listen_queue_size;
			int family;
			std::string address, port, dbasefile;
			
			WheespaOpts(){
				port = "2748";
				family = AF_INET;
				listen_queue_size = 1024;
				dbasefile = "wbase.db";
				verbose = false;
			}
			
			int getOpts(int, char**);
	};

}


#endif
