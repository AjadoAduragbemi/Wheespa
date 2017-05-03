
#include <iostream>
#include "wheespa_server.hpp"

int main(int argc, char* argv[]){
	
	try{
		wheespa::WheespaOpts opts;
		
		if(opts.getOpts(argc, argv) != OPTGOOD) return -1;
		
		wheespa::wheespa_server::WheespaServer whServ(opts);
		whServ.start();
		
	}catch(const std::exception& ex){
		std::cerr << ex.what() << std::endl;
	}
	
	return 0;
}
