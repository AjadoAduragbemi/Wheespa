
#include <regex>
#include "wheespa_client.hpp"

enum{
	STDIN,
	STDOUT,
	STDERR
};

namespace wheespa{

	namespace wheespa_client{
		
		void WheespaClient::start(){

			FD_ZERO(&m_rfds);
			FD_ZERO(&m_wfds);
			
			m_si->connect(m_opts.address, m_opts.port);
			m_sock_fd = m_si->getFD(wheespa_socket::FD::CONN);

			m_si->close(wheespa_socket::FD::CONN);
			/*FD_SET(m_sock_fd, &m_rfds);
			FD_SET(STDIN, &m_rfds);
			FD_SET(STDOUT, &m_wfds);
						
			int max_fd = m_sock_fd + 1;
			
			while(true){
				if(select(max_fd, &m_rfds, &m_wfds, nullptr, nullptr) == -1){
					std::cerr << "[!] select failed." << std::endl;
					break;
				}else{

				}
			}*/
		}

	}

}