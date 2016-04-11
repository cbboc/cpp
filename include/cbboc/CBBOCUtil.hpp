#ifndef CBBOC_CBBOCUTIL_HPP
#define CBBOC_CBBOCUTIL_HPP

#include "cbboc/RNG.hpp"

#include <cstdio>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////

#define CBBOC_INSPECT( x ) std::clog << #x << ": " << x << std::flush << std::endl
#define CBBOC_WHERE std::clog << std::string( __FILE__ ) << " " << __LINE__ << std::flush << std::endl

//////////////////////////////////////////////////////////////////////

inline long long system_current_time_millis() {
	using namespace std::chrono;
	milliseconds ms = duration_cast< milliseconds >( high_resolution_clock::now().time_since_epoch() );
	return ms.count();
}

inline std::vector< bool >
random_bitvector( int length ) {

	std::bernoulli_distribution dist;

	std::vector< bool > result;
	for( int i=0; i<length; ++i )
		result.push_back( dist( RNG::instance() ) );

	return result;
}

/***
// Get the appropriate string for separating directories on this file system
const std::string directory_separator =
#ifdef _WIN32
                                        "\\";
#else
                                        "/";
#endif
inline
const std::string& directory_separator() {
	static std::string result_ = "/";
	return result_;
}

***/

//////////////////////////////////////////////////////////////////////

namespace cbboc_std_io {

///////////////////////////////////

template<typename T>
std::ostream& operator <<( std::ostream& out, const std::vector<T>& v ) {
    out << "[";
    for( size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if( i != v.size() - 1 ) 
            out << ",";
    }
    return out << "]";
}

///////////////////////////////////

} // namespace cbboc_std_io {

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
