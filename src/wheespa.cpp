
#include <iostream>
#include "wheespa_server.hpp"
#include "wheespa_client.hpp"

int main(int argc, char* argv[]){
	
	try{
		wheespa::WheespaOpts opts;
		
		if(opts.getOpts(argc, argv) != OPTGOOD) return -1;
		
		if(opts.listen){
			wheespa::wheespa_server::WheespaServer w_serv(opts);
			w_serv.start();
		}else{
			wheespa::wheespa_client::WheespaClient w_cli(opts);
		}
		
	}catch(const std::exception& ex){
		std::cerr << ex.what() << std::endl;
	}
	
	return 0;
}
