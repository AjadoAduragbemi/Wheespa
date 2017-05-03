#ifndef WHEESPA_SHA_HPP
#define WHEESPA_SHA_HPP

#include "openssl/sha.h"
#include <memory>
#include <sstream>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "libcrypto")

namespace wheespa{

	typedef unsigned char byte;

	template<const int d_len>
	class SHA{
		
		const std::string m_in;
		std::string m_out;
		byte *m_tmp;
		
		void HexEncodeOutput();
		void preferredAlgorithmImpl();
	public:
			
		SHA(const std::string& s, bool hex_out = true) : m_in(s),
														 m_tmp(new byte[d_len]),
														 m_out(){
			preferredAlgorithmImpl();
			if(hex_out) HexEncodeOutput();
		}

		~SHA(){ delete[] m_tmp; }
		
		std::string getOutput() const;
		
	};

}
 
#endif