
#include "wheespa_opts.hpp"


option::ArgStatus NonEmpty(const option::Option& option, bool msg){
	if (option.arg != 0 && option.arg[0] != 0)
 		return option::ARG_OK;
 
	if (msg) std::cerr << "option requires a non-empty argument -- " << option.name << std::endl;
	return option::ARG_ILLEGAL;
}


option::ArgStatus Unknown(const option::Option& option, bool msg){
	if (msg) std::cerr << "Unknown option -- '" << option.name << "'\n";
	return option::ARG_ILLEGAL;
}


option::ArgStatus Numeric(const option::Option& option, bool msg){
	char* endptr = 0;
	if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
	if (endptr != option.arg && *endptr == 0)
		return option::ARG_OK;

	if (msg) std::cerr << "option requires a numeric argument -- " << option.name << std::endl;
	return option::ARG_ILLEGAL;
}

enum optIndex{UNKNOWN,
			  VERBOSE,
			  PORT,
			  CERTFILE,
			  LOGFILE,
			  BASEFILE,
			  QUEUESIZE,
			  LISTEN,
			  AS_ADMIN,			//login as an administrator
			  AS_ANONYMOUS,		//login as an anonymous user
			  HELP};

const option::Descriptor desc[] = {{UNKNOWN, 0, "", "", Unknown, 
									"USAGE: wserv [options] ADDRESS\n\noptions: "},
								   {HELP, 0, "h", "help", option::Arg::None,
									" -h, --help             print this help message."},
								   {VERBOSE, 0, "v", "verbose", option::Arg::None, 
									" -v, --verbose          be verbose."},
								   {CERTFILE, 0, "", "cert", NonEmpty, 
									"     --cert=CERTFILE    ssl certificate to use."},
								   {LOGFILE, 0, "", "logfile", NonEmpty, 
									"     --logfile=LOGFILE  file to log errors."},
								   {BASEFILE, 0, "", "database", NonEmpty, 
									"     --database=FILE    file to store and retrieve authentication details."},
								   {QUEUESIZE, 0, "q", "size", Numeric, 
									" -q, --size=QUEUESIZE   listening queue size."},
								   {LISTEN, 0, "l", "listen", option::Arg::None, 
									" -l, --listen           listen for inbound connections."},
								   {PORT, 0, "p", "port", Numeric, 
									" -p, --port=PORT        port to listen on (default=8890)."},
								   {0,0,0,0,0,0}};


int wheespa::WheespaOpts::getOpts(int argc, char* argv[]){
	argc -= (argc > 0);
	argv += (argc > 0);
	
	option::Stats stats(desc, argc, argv);
	
	option::Option options[stats.options_max];
	option::Option buffer[stats.buffer_max];
	
	option::Parser parse(desc, argc, argv, options, buffer);
	
	//if(parse.error()) return -1;
	if(options[HELP] || parse.error()){
		option::printUsage(std::cout, desc);
		return OPTBAD;
	}
	
	for(int i = 0; i < parse.optionsCount(); i++){
		switch(buffer[i].index()){
			case VERBOSE:
				verbose = true;
				break;
			case CERTFILE:
				certfile = buffer[i].arg;
				break;
			case LOGFILE:
				logfile = buffer[i].arg;
				break;
			case BASEFILE:
				dbasefile = buffer[i].arg;
				break;
			case QUEUESIZE:
				listen_queue_size = std::strtol(buffer[i].arg, nullptr, 10);
				break;
			case PORT:
				port = buffer[i].arg;
				break;
			case LISTEN:
				listen = true;
				break;
			case AS_ADMIN:
				as_admin = true;
				break;
			case AS_ANONYMOUS:
				as_anonymous = true;
				break;
		}
	}
		
	if(parse.nonOptionsCount()) address = parse.nonOption(0);
	
	if(address.compare("") == 0 && !listen){
		std::cerr << "[!] ADDRESS cannot be empty in client mode." << std::endl;
		return NOCLIENTADDR;
	}
	
	return OPTGOOD;
}
