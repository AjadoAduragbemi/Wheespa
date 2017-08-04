#ifndef WHEESPA_CLIENT_HPP
#define WHEESPA_CLEINT_HPP

#include <vector>
#include <thread>
#include "wheespa_base.hpp"
#include "wheespa_socket.hpp"
#include "wheespa_tree.hpp"
#include "wheespa_opts.hpp"

namespace wheespa{

	namespace wheespa_client{
		
		class WheespaClient{

			fd_set m_rfds, m_wfds;
			
			std::vector<std::thread> m_vth;
			const WheespaOpts m_opts;
			int m_sock_fd;
			wheespa_socket::PSocketInterface m_si;

		public:
			WheespaClient(const WheespaOpts& m_opts) :  m_opts(m_opts),
														m_sock_fd(0),
														m_si(new wheespa_socket::Socket(m_opts.family, SOCK_STREAM)){}

			void start();

			~WheespaClient(){
				m_si->cleanUpOnEnd(true);
				delete m_si;
			}

		};
		
	}

}

#endif
