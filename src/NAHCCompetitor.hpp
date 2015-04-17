#ifndef NAHC_COMPETITOR_HPP
#define NAHC_COMPETITOR_HPP

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

/**
 * Baseline metaheuristic strategy: Next Ascent Hillclimbing.
 *
 */

class NAHCCompetitor : public Competitor {
public:

	NAHCCompetitor() {}

	virtual ~NAHCCompetitor() {}

	///////////////////////////////

	virtual TrainingCategory getTrainingCategory() const { return TrainingCategory::NONE; }

	virtual void train( std::vector<ObjectiveFn>& trainingSet, long long maxTimeInMilliseconds ) {
		// this should never be called because we're in TrainingCategory.NONE
		throw new std::logic_error( "Invalid call to RandomCompetitor.train()" );
	}

	virtual void test( ObjectiveFn& testCase, long long maxTimeInMilliseconds ) {
		std::vector< bool > incumbent = random_bitvector( testCase.getNumGenes() );
		// ^ randomBitvector is in CBBOC2015Util.hpp

		double bestValue = testCase.value( incumbent ); 
		bool improved = false;
		do {
			const std::vector< std::vector< bool > > neighbors = hamming1Neighbours( incumbent );
			for( size_t i=0; i<neighbors.size(); ++i ) {

				const std::vector< bool >& neighbor = neighbors[ i ];
				const double value = testCase.value( neighbor );
				if( value > bestValue ) {
					improved = true;
					incumbent = neighbor;
					bestValue = value;
				}
			}
			
		} while( improved && testCase.getRemainingEvaluations() > 0 );
	}

	///////////////////////////////
	
private:
	static std::vector< std::vector< bool > >
	hamming1Neighbours( const std::vector< bool >& incumbent ) {
		std::vector< std::vector< bool > > result;
		for( size_t i=0; i<incumbent.size(); ++i ) {
			std::vector< bool > neighbour = incumbent;
			neighbour[ i ] = !neighbour[ i ];
			result.push_back( neighbour );
		}
		
		return result;
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
