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
			bool verbose,
				 listen,
				 as_admin,
				 as_anonymous;
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
				certfile = "certficates/wdomain.inclprivkey.pem";
			}
			
			int getOpts(int, char**);
	};

}


#endif
