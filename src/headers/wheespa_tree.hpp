#ifndef WHEESPA_TREE_HPP
#define WHEESPA_TREE_HPP

#include "binary_tree.hpp"
#include "wheespa_socket.hpp"
#include<memory>

namespace wheespa{

	typedef struct Connected{
		std::string key;
		std::shared_ptr<const wheespa_socket::SocketStream> sock_stream;
		bool is_admin;
		bool registered;
		std::string connect_to;		//anonymous users can only connect to one registered user or an address
		
		Connected() : sock_stream(nullptr),
					  is_admin(false),
					  registered(false),
					  key(""){}
		~Connected(){}
		
	} WheespaConnected;
	
	typedef BinarySearchTree<wheespa::WheespaConnected, std::string> WheespaServerTree;
}

#endif

