
#include <mutex>
#include <typeinfo>
#include <csignal>
#include <regex>
#include <cassert>
#include "wheespa_server.hpp"
#include "handle_signal.hpp"
#include "binary_tree.cc"

using namespace wheespa;

void wheespa_server::WheespaServer::prepareSocket(){
	if(m_si->bind(m_opts.address, m_opts.port) == BIND_FAILED) wheespa_socket::BindException(strerror(errno));
	m_si->listen(m_opts.listen_queue_size);
	if(m_opts.verbose) std::cout << "[+] listening on port " << m_si->getPort(m_si->getHostInfo()->ai_addr) << "...\n";
	m_serv_fd = m_si->getFD(wheespa_socket::FD::SOCK);
}


void wheespa_server::WheespaServer::prepareDatabase(){
	wheespa_base::FlexDB db;
	db.open(m_opts.dbasefile);
	db.execQuery("PRAGMA foreign_keys = '1'");
	
	std::vector<std::string> arr;
	#if _MSC_VER
		arr.push_back("W_ID integer primary key not NULL");
		arr.push_back("W_USER char(32) unique not NULL");
		arr.push_back("W_PASS char(40) not NULL");
		arr.push_back("W_MAIL char(64) not NULL");
		arr.push_back("W_KEY char(16)");
	#else
		arr =
		{
			"W_ID integer primary key not NULL",
			"W_USER char(32) unique not NULL",
			"W_PASS char(40) not NULL",
			"W_MAIL char(64) not NULL",
			"W_KEY char(16)"
		};
	#endif

	if(!db.createTable("Registered", arr)) 				db.printErrMsg();
	if(!db.createTable("PendingRegistrations", arr)) 	db.printErrMsg();
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
	
	if(!db.createTable("Administrators", arr)) db.printErrMsg();
	
	SHA<SHA_DIGEST_LENGTH> default_admin_pass("wh33spaAdm1n");
	if(!db.insert("Registered",
				  "W_ID, W_USER, W_PASS, W_MAIL, W_KEY",
				  {"1", "wAdministrator", default_admin_pass.getOutput(), "x@y.z", ""})) 	/*db.printErrMsg()*/;
	if(!db.insert("Administrators", "ID, W_USER", {"1", "wAdministrator"})) 				/*db.printErrMsg()*/;
	
	arr.clear();
	
	db.close();
}


void wheespa_server::WheespaServer::start(){

	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(m_serv_fd, &rfd);

	int max_conn_fd = m_serv_fd + 1;
	std::mutex mx;
	//auto foo = [this](){ m_si->close(wheespa_socket::FD::SOCK); };
	SignalHandler sig_handler;
	sig_handler.sigaction(SIGTERM);
	sig_handler.sigaction(SIGINT);
	
	static WheespaServerTree w_connected;
	static std::string db_filename = m_opts.dbasefile;
	
	
	std::thread th1([](){
		while(SignalHandler::r_signal == 0){
			w_connected.traverse([](Node<WheespaConnected>* node){
				try{
					std::string recvd = node->data.sock_stream->read(2048, 0);
					if(recvd.length() > 0){
						HandleReceived hr(recvd, node, db_filename);
						hr.handle(w_connected);
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
	});
	
	try{
		std::shared_ptr<char> thread_count;
		std::regex rex(R"(^(?:\[([A-Z_]+)\])::((?:[a-z]+=[\w@\.!%<>,~`]+:::){2,4})$)");
		std::shared_ptr<std::regex> shared_rex = std::make_shared<std::regex>(std::regex(R"(([a-z]+)=([\w@\.!%<>,~`]+):::)"));
		
		while(!SignalHandler::r_signal){
			
			if(select(max_conn_fd, &rfd, nullptr, nullptr, nullptr) != -1){
					
				if(FD_ISSET(m_serv_fd, &rfd)){
	
					if(m_si->accept() <= 0) {
						continue;
					}
					
					wheespa_socket::PSocketInterface sI;
					if(typeid(*m_si) == typeid(wheespa_socket::SecureSocket)){
						sI = new wheespa_socket::SecureSocket;
						std::memcpy(sI, m_si, sizeof(wheespa_socket::SecureSocket));
					}else{
						sI = new wheespa_socket::Socket;
						std::memcpy(sI, m_si, sizeof(wheespa_socket::Socket));
					}
					
					m_vth.push_back(std::thread([&mx, shared_rex, rex,  this](wheespa_socket::PSocketInterface t_si){
						try{
							int retries = 0;
							bool handled_properly = false;
							wheespa_socket::SocketStream ss(t_si);
							ss.write("WBANNER");
							while(!handled_properly && retries++ < 3){
								std::string request = ss.read(1024, 90);
								std::smatch match_result;
								bool is_valid_request = std::regex_match(request, match_result, rex);
								if(is_valid_request){
									ParseRequest preq(ss, match_result[1], match_result[2], shared_rex, m_opts.dbasefile);
									if((handled_properly = preq.handle()) && preq.getConnected().sock_stream.get() != nullptr){
										const Node<WheespaConnected>* node_tmp = w_connected.find(preq.getConnected().key);
										if(node_tmp == nullptr || (node_tmp != nullptr && node_tmp->overwrite)){
											mx.lock();
											w_connected.insert(preq.getConnected());
											mx.unlock();
											ss.write("::WHEESPA_CONNECTION_SUCCESS");
											return;
										}else{
											ss.write("::WHEESPA_CONNECTION_EXISTS");
											if(preq.requestedDisconnect()){
												mx.lock();
												w_connected.remove(preq.getConnected().key);
												mx.unlock();
											}
										}
									}
								}else{
									handled_properly = true;
								}
							}
						}catch(const std::exception& ex){
							std::cerr << ex.what() << std::endl;
						}
						t_si->shutdown(0);
						ss.close();
					}, sI));
					
				}
			}
		}
	}catch(const std::exception& ex){
		std::cerr << ex.what() << std::endl;
	}
	
	th1.join();
	
	m_si->shutdown(2);
	m_si->close(wheespa_socket::FD::SOCK);
	return;
}

