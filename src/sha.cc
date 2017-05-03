
#include "sha.hpp"

template<const  int d_len> std::string wheespa::SHA<d_len>::getOutput() const{
	return m_out;
}


template<const  int d_len> void wheespa::SHA<d_len>::preferredAlgorithmImpl(){
	switch(d_len){

	case SHA_DIGEST_LENGTH:
		::SHA1(reinterpret_cast<const byte*>(m_in.c_str()), m_in.length(), m_tmp);
		break;
	case SHA224_DIGEST_LENGTH:
		::SHA224(reinterpret_cast<const byte*>(m_in.c_str()), m_in.length(), m_tmp);
		break;
	case SHA256_DIGEST_LENGTH:
		::SHA256(reinterpret_cast<const byte*>(m_in.c_str()), m_in.length(), m_tmp);
		break;
	case SHA384_DIGEST_LENGTH:
		::SHA384(reinterpret_cast<const byte*>(m_in.c_str()), m_in.length(), m_tmp);
		break;
	case SHA512_DIGEST_LENGTH:
		::SHA512(reinterpret_cast<const byte*>(m_in.c_str()), m_in.length(), m_tmp);
		break;
	default:
		return;
	}

	m_out = std::string(reinterpret_cast<char*>(m_tmp), d_len);
	
}


template<const  int d_len> void wheespa::SHA<d_len>::HexEncodeOutput(){
	std::ostringstream ss;
	ss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase;

	for(auto a : m_out){
		ss << (uint16_t(a) & 0xff) << std::setw(2);
	}

	m_out = ss.str();
}


template std::string wheespa::SHA<SHA_DIGEST_LENGTH>::getOutput() const;
template void wheespa::SHA<SHA_DIGEST_LENGTH>::preferredAlgorithmImpl();
template void wheespa::SHA<SHA_DIGEST_LENGTH>::HexEncodeOutput();
