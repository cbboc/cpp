#ifndef CBBOC_TRAINING_CATEGORY_HPP
#define CBBOC_TRAINING_CATEGORY_HPP

#include <stdexcept>
#include <iostream>

//////////////////////////////////////////////////////////////////////

enum TrainingCategory {
	NONE, SHORT, LONG
};

/***
inline long getTrainingTimeInMillis( TrainingCategory c ) {
	switch( c ) {
		case NONE : return 0; 
		case SHORT : return 60 * 1000L;
		case LONG : return 120 * 1000L;
		default : throw std::invalid_argument( "Bad TrainingCategory value" );
	}
}
***/
// NONE( 0 ), SHORT( 1 ), LONG( 10 );

inline long getMultiplier( TrainingCategory c ) {
	switch( c ) {
		case NONE : return 0;
		case SHORT : return 1L;
		case LONG : return 10L;
		default : throw std::invalid_argument( "Bad TrainingCategory value" );
	}
}

inline int numericCode(TrainingCategory c ) {
	switch( c ) {
		case NONE : return 0;
		case SHORT : return 1;
		case LONG : return 2;
		default : throw std::invalid_argument( "Bad TrainingCategory value" );
	}
}

inline std::ostream& operator <<( std::ostream& s, TrainingCategory c ) {
	switch( c ) {
		case NONE : return s << "NONE";
		case SHORT : return s << "SHORT";
		case LONG : return s << "LONG";
		default : return s << "<<invalid TrainingCategory value>>";
	}
}

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
