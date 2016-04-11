
#ifndef CBBOC_COMPETITOR_HPP
#define CBBOC_COMPETITOR_HPP

#include "ObjectiveFn.hpp"
#include "TrainingCategory.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////

// class ObjectiveFn;

class Competitor {
public:
	Competitor() {}

	virtual ~Competitor() {}

	//////////////////////////////////

	virtual TrainingCategory getTrainingCategory() const = 0;
	
	virtual void train( std::vector< ObjectiveFn >& trainingSet, long long maxTimeInMilliseconds ) = 0;
	
	virtual void test( ObjectiveFn& testCase, long long maxTimeInMilliseconds ) = 0;

protected:
	Competitor( const Competitor& rhs ) {}
	Competitor& operator =( const Competitor& rhs ) { return *this; }
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////

