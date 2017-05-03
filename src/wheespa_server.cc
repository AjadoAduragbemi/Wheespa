
#include <mutex>
#include <typeinfo>
#include <csignal>
#include <regex>
#include "wheespa_server.hpp"
#include "binary_tree.cc"

using namespace wheespa;

void wheespa_server::WheespaServer::prepareSocket(){
	if(m_si->bind(m_opts.address, m_opts.port) == BIND_FAILED) wheespa_socket::BindException(strerror(errno));
	m_si->setSockOpt(m_si->getFD(wheespa_socket::FD::SOCK), SOL_SOCKET, SO_REUSEADDR);
	m_si->listen(m_opts.listen_queue_size);
	if(m_opts.verbose) std::cout << "[+] listening on port " << m_si->getPort(m_si->getHostInfo()->ai_addr) << "...\n";
	m_serv_fd = m_si->getFD(wheespa_socket::FD::SOCK);
}


void wheespa_server::WheespaServer::prepareDatabase(){
	m_db.open(m_opts.dbasefile);
	m_db.execQuery("PRAGMA foreign_keys = '1'");
	
	std::vector<std::string> arr;
	#if _MSC_VER
		arr.push_back("W_ID integer primary key not NULL");
		arr.push_back("W_USER char(32) unique not NULL");
		arr.push_back("W_PASS char(40) not NULL");
		arr.push_back("W_LAST_LOGIN long");
	#else
		arr =
		{
			"W_ID integer primary key not NULL",
			"W_USER char(32) unique not NULL",
			"W_PASS char(40) not NULL",
			"W_LAST_LOGIN long",
		};
	#endif

	if(!m_db.createTable("Registered", arr)) /*m_db.printErrMsg()*/;
	if(!m_db.createTable("PendingRegistrations", arr)) /*m_db.printErrMsg()*/;
	arr.clear();

	#if _MSC_VER
		arr.push_back("ID integer primary key not NULL");
		arr.push_back("W_USER char(32) unique not NULL");
		arr.push_back("Foreign Key(W_USER)  REFERENCES Registered(W_USER) ON DELETE Cascade ON UPDATE Cascade");
	#else
		arr =
		{
			"ID integer primary key not NULL",
			"W_USER char(32) unique not NULL",
			"Foreign Key(W_USER) REFERENCES Registered(W_USER) ON DELETE Cascade ON UPDATE Cascade"
		};
	#endif
	
	if(!m_db.createTable("Administrators", arr)) /*m_db.printErrMsg()*/;
	
	SHA<SHA_DIGEST_LENGTH> default_admin_pass("wh33spaAdm1n");
	if(!m_db.insert("Registered",
					"W_ID, W_USER, W_PASS",
					{"1", "wAdministrator", default_admin_pass.getOutput()})) /*m_db.printErrMsg()*/;
	if(!m_db.insert("Administrators", "ID, W_USER", {"1", "wAdministrator"})) /*m_db.printErrMsg()*/;
	
	arr.clear();
	
	m_db.close();
}


void wheespa_server::WheespaServer::start(){

	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(m_serv_fd, &rfd);

	int max_conn_fd = m_serv_fd + 1;
	std::mutex mx;
	auto foo = [this](){ m_si->close(wheespa_socket::FD::SOCK); };
	
	static WheespaServerTree w_connected;
	
	m_vth.push_back(std::thread([](){
		//static bool p = true;
		while(true){
			
			w_connected.traverse([](Node<WheespaConnected>* node){
				try{
					std::string recvd = node->data.sock_stream->read(2048, 0);
					if(recvd.length() > 0){
						HandleReceived h_recvd(recvd, node);
						h_recvd.handle(w_connected);
					}
				}catch(const std::exception& e){
					std::cout << "[-] " << node->data.key << " disconnected." << std::endl;
					node->data.sock_stream->close();
					node->overwrite = true;
				}
			});
			/* Wait a byte :D */
			std::this_thread::sleep_for(std::chrono::milliseconds(256));	//consider CPU
		}
	}));
	
	while(true){
			
		if(select(max_conn_fd, &rfd, nullptr, nullptr, nullptr) == -1){
			std::cerr << "[!] select failed." << std::endl;
			break;
		}else{
				
			if(FD_ISSET(m_serv_fd, &rfd)){

				if(!m_si->accept()) {
					std::cerr << "[!] An error occured while trying to accept a connection." << std::endl;
					continue;
				}
				
				m_vth.push_back(std::thread([&mx, this](wheespa_socket::PSocketInterface t_si){
					
					wheespa_socket::PSocketInterface sI;
					
					try{
						
						if(typeid(*t_si) == typeid(wheespa_socket::SecureSocket)){
							sI = new wheespa_socket::SecureSocket;
							std::memcpy(sI, t_si, sizeof(wheespa_socket::SecureSocket));
						}else{
							sI = new wheespa_socket::Socket;
							std::memcpy(sI, t_si, sizeof(wheespa_socket::Socket));
						}
						
						HandleRequest h_request(wheespa_socket::SocketStream(sI), m_opts.dbasefile);
						h_request.handle();
						
						if(h_request.getConnected().sock_stream.get() != nullptr){
							mx.lock();
								if(w_connected.find(h_request.getConnected().key) == nullptr){
									h_request.getConnected().sock_stream->write("WHEESPA_CONNECT_SUCCESS");
									w_connected.insert(h_request.getConnected());
									if(m_opts.verbose)
										std::cout << "[+] " << h_request.getConnected().key << " is now connected." << std::endl;
								}else{
									h_request.getConnected().sock_stream->write("WHEESPA_CURRENTLY_LOGGED_IN");
									h_request.getConnected().sock_stream->close();
								}
							mx.unlock();
						}else{
							sI->close(wheespa_socket::FD::CONN);
						}
						
					}catch(const std::exception& ex){
						mx.lock();
							std::cerr << ex.what() << std::endl;
						mx.unlock();
						sI->close(wheespa_socket::FD::CONN);
						//delete sI;
					}
				}, m_si));
					
			}
		}
	}

}

