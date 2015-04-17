#ifndef CBBOC2015_RNG_HPP
#define CBBOC2015_RNG_HPP

#include <cassert>
#include <random>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////

struct RNG {

	static std::mt19937& instance() {  
		static std::mt19937 instance_;
		return instance_; 
	}

	///////////////////////////////

	static bool nextBoolean() {
		std::bernoulli_distribution dist;
		return dist( instance() );
	}

	static int nextInt() {
		std::uniform_int_distribution<> dist;
		return dist( instance() );
	}

	static int nextInt( int lub ) {
		if( lub <= 0 )
			throw std::invalid_argument( "n must be positive" );

		std::uniform_int_distribution<> dist( 0, lub - 1 );
		const int result = dist( instance() );
		assert( result >= 0 && result < lub );
		return result;
	}

	static double nextDouble() {
		std::uniform_real_distribution<> dist;
		// ^ defaults to [0.0,1.0]
		const double result = dist( instance() );
		assert( result >= 0.0 && result <= 1.0 );
		return result;
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////

