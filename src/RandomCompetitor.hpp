#ifndef RANDOM_COMPETITOR_HPP
#define RANDOM_COMPETITOR_HPP

//////////////////////////////////////////////////////////////////////

// #include "cbboc2015/CBBOC2015.hpp"
#include "cbboc2015/CBBOC2015Util.hpp"
#include "cbboc2015/Competitor.hpp"
#include "cbboc2015/ObjectiveFn.hpp"
#include "cbboc2015/ProblemClass.hpp"
#include "cbboc2015/TrainingCategory.hpp"
#include "cbboc2015/RNG.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////

class RandomCompetitor : public Competitor {

public:

	RandomCompetitor() {}

	virtual ~RandomCompetitor() {}

	///////////////////////////////

	virtual TrainingCategory getTrainingCategory() const { return TrainingCategory::NONE; }

	virtual void train( std::vector<ObjectiveFn>& trainingSet, long long maxTimeInMilliseconds ) {
		// this should never be called because we're in TrainingCategory.NONE 
		throw new std::logic_error( "Invalid call to RandomCompetitor.train()" );
	}

	virtual void test( ObjectiveFn& testCase, long long maxTimeInMilliseconds ) {

		// CBBOC_INSPECT( maxTimeInMilliseconds );
		// const long long startTime = system_current_time_millis();
		// while( system_current_time_millis() - startTime <= maxTimeInMilliseconds ) {
		for( ; ; ) {

			// const size_t randomIndex = dist( RNG::instance() );
			// assert( randomIndex >= 0 && randomIndex < testingSet.size() );
			const std::vector< bool > candidate = random_bitvector( testCase.getNumGenes() );
			// ^ randomBitvector is in CBBOC2015Util.hpp

			double value = testCase.value( candidate );

			// Useful strategies will obviously care about value...
			++value; // avoids spurious compiler warning about unused variable.
		}
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
