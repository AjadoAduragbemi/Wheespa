#ifndef WHEESPA_SERVER_HPP
#define WHEESPA_SERVER_HPP

#include <vector>
#include <thread>
#include <atomic>
#include "wheespa_base.hpp"
#include "wheespa_socket.hpp"
#include "wheespa_tree.hpp"
#include "wheespa_opts.hpp"
#include "handle_request.hpp"
#include "handle_received.hpp"
#include "sha.hpp"

namespace wheespa{

	namespace wheespa_server{
		
		class WheespaServer {

			int m_serv_fd,
				m_cli_fd;
			
			std::vector<std::thread> m_threads;
			std::vector<std::atomic<bool>*> m_futures;
			
			wheespa_socket::PSocketInterface m_si;

			const WheespaOpts m_opts;
			
			void prepareSocket();
			void prepareDatabase();
			
		public:

			WheespaServer(WheespaOpts& opts) : m_opts(opts),
											   m_serv_fd(0),
											   m_cli_fd(0),
											   m_si(new wheespa_socket::SecureSocket(AF_UNSPEC,
																					 SOCK_STREAM,
																					 IPPROTO_TCP,
																					 opts.certfile,
																					 wheespa_socket::ProtocolMethod::PROTOCOL_SERVER_METHOD)),
											   m_threads(), m_futures() {
				prepareDatabase();
				prepareSocket();
			}

			~WheespaServer(){
				m_si->cleanUpOnEnd(true);
				delete m_si;

				for(auto& th : m_threads) th.join();
			}

			void start();

		};

	}
	
}

#endif
