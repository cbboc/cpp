#ifndef CBBOC_SAHHCOMPETITOR_HPP
#define CBBOC_SAHHCOMPETITOR_HPP

#include "cbboc/CBBOCUtil.hpp"
#include "cbboc/Competitor.hpp"
#include "cbboc/ObjectiveFn.hpp"
#include "cbboc/TrainingCategory.hpp"
#include "cbboc/RNG.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <limits>

//////////////////////////////////////////////////////////////////////

/**
 * Baseline learning strategy: Simulated Annealing hyper-heuristic 
 * which learns the start and end values for the annealing schedule.  
 */

class SAHHCompetitor : public Competitor {

	double saScheduleLowerBound = 0.0;
	double saScheduleUpperBound = std::numeric_limits< double >::max();
	TrainingCategory trainingCategory;
	
	///////////////////////////////

public:
	
	SAHHCompetitor( TrainingCategory _trainingCategory )
	: trainingCategory( _trainingCategory ) {
		assert( invariant() );
	}

	virtual ~SAHHCompetitor() {}

	///////////////////////////////

	virtual TrainingCategory getTrainingCategory() const { return trainingCategory; }

	///////////////////////////////

	bool invariant() const {
		return saScheduleLowerBound < saScheduleUpperBound;
	}

private:

	static double mean( const std::vector< double >& v ) {
		double sum = std::accumulate(std::begin(v), std::end(v), 0.0);
		double m =  sum / v.size();
		return m;
	}

	static double variance( const std::vector< double >& v ) {
		const double m = mean( v );

		double accum = 0.0;
		for( size_t i=0; i<v.size(); ++i )
		    accum += ( v[i] - m) * ( v[i] - m);

		return accum / (v.size()-1);
	}

	///////////////////////////////
	
	static std::pair< double, double >
	WhiteTemperatureRangeForSA( const std::vector< double >& fitnessTrajectory ) {
		if( fitnessTrajectory.empty() )
			throw std::invalid_argument( "empty fitness trajectory in SAHHCompetitor.WhiteTemperatureRangeForSA" );
		
		/**
		 * @see:
		 * @inproceedings{white:1984,
		 *  address = {Port Chester, NY},
		 *  author = {White, S. R.},
		 *  booktitle = {Proceeedings of the IEEE International Conference on Computer Design (ICCD) '84},
		 *  pages = {646--651},
		 *  title = {Concepts of Scale in Simulated Annealing},
		 *  year = {1984}
		 * }
		 */		
		double minDifference = std::numeric_limits< double >::max();
		for( size_t i=1; i<fitnessTrajectory.size(); ++i )	{
			const double diff = fitnessTrajectory[ i ] - fitnessTrajectory[ i - 1 ];
			const double delta = std::abs( diff );
			if( delta < minDifference )
				minDifference = delta;
		}

		const double var = variance( fitnessTrajectory );
		return std::make_pair( minDifference, std::sqrt( var ) );
	}
	
	///////////////////////////////

	static std::vector< bool > randomHamming1Neighbour( const std::vector< bool >& incumbent ) {
		std::vector< bool > neighbour( incumbent );
		const int randomIndex = RNG::nextInt( neighbour.size() );
		neighbour[ randomIndex ] = !neighbour[ randomIndex ];
		return neighbour;
	}
	
	///////////////////////////////
	
	static std::vector< double >
	fitnessTrajectoryOfRandomWalk( ObjectiveFn& f, long numSteps ) {
		
		std::vector< bool > incumbent = random_bitvector( f.getNumGenes() );
		// ^ randomBitvector is in CBBOC2015Util.hpp
		
		std::vector< double > result;
		for( long i=0; i<numSteps; ++i ) {
			const std::vector< bool > incoming = randomHamming1Neighbour( incumbent );
			result.push_back( f.value( incoming ) );
			incumbent = incoming;
		}

		return result;
	}
	
	///////////////////////////////

public:
	
	virtual void train( std::vector<ObjectiveFn>& trainingSet, long long maxTimeInMilliseconds ) {

		const ObjectiveFn& fn0 = trainingSet.front();
		const long evalPerCase = fn0.getRemainingEvaluations() / trainingSet.size();
		// ^ `remaining evaluations' for training are shared across all instances.

		std::vector< double > saScheduleLowerBounds, saScheduleUpperBounds;
		for( size_t i=0; i<trainingSet.size(); ++i ) {
			ObjectiveFn& fn = trainingSet[ i ];
			const std::vector< double > fitnessTrajectory = fitnessTrajectoryOfRandomWalk( fn, evalPerCase );
			const std::pair< double, double > saScheduleBounds = WhiteTemperatureRangeForSA( fitnessTrajectory );
			saScheduleLowerBounds.push_back( saScheduleBounds.first );
			saScheduleUpperBounds.push_back( saScheduleBounds.second );
		}
		
		saScheduleLowerBound = mean( saScheduleLowerBounds );
		saScheduleUpperBound = mean( saScheduleUpperBounds );
		assert( invariant() );		
	}

	///////////////////////////////

private:
	
	static bool isNaN( double x ) { return x != x; }

	static bool SAAccept( double lastValue, double currentValue, double temperature ) {
		if( isNaN( temperature ) || temperature < 0 )
			throw std::invalid_argument( "Non-negative temperature expected" );

		// assumes maximising...
		if( currentValue > lastValue )
			return true;			
		else if( temperature == 0 )
			return currentValue >= lastValue;
		else {
			assert( currentValue <= lastValue );
			assert( temperature > 0 );
			
			const double diffDivT = ( currentValue - lastValue ) / temperature;
			assert( diffDivT <= 0 );
			
			const double p = std::exp( diffDivT );
			assert( !isNaN( p ) );
			return RNG::nextDouble() < p;
		}
	}	
	
	///////////////////////////////	

	static bool implies( bool a, bool  b ) { return a ? b : true; }

	static double linear_interpolate( double inputValue, double inputLimit1, double inputLimit2,
		double outputLimit1, double outputLimit2 ) {

		assert( implies( inputLimit1 <= inputLimit2, inputLimit1 <= inputValue && inputValue <= inputLimit2 ) );
		assert( implies( inputLimit1 >= inputLimit2, inputLimit2 <= inputValue && inputValue <= inputLimit1 ) );
		assert( implies( inputLimit1 == inputLimit2, outputLimit1 == outputLimit2 ) );
		assert( implies( inputLimit1 == inputLimit2, inputValue == inputLimit1 ) );

		const double t = ( inputValue - inputLimit1 ) / ( inputLimit2 - inputLimit1 );
		assert( 0.0f <= t );
		assert( t <= 1.0f );

		// The following uses the minimum number of operators:
		double result = outputLimit2;
		result *= t;
		double r1 = outputLimit1;
		r1 *= ( 1 - t );
		result += r1;

		assert( implies( outputLimit1 <= outputLimit2,
			outputLimit1 - FLT_EPSILON <= result
			&& result <= outputLimit2 + FLT_EPSILON ) );

		assert( implies( outputLimit1 >= outputLimit2,
			outputLimit2 - FLT_EPSILON <= result &&
			result <= outputLimit1 + FLT_EPSILON ) );
		return result;
	}

public:

	virtual void test( ObjectiveFn& testCase, long long maxTimeInMilliseconds ) {

		std::vector< bool > incumbent = random_bitvector( testCase.getNumGenes() );
		double lastValue = testCase.value( incumbent ); 
		
		const long numEvaluations = testCase.getRemainingEvaluations();
		for( int i=0; i<numEvaluations; ++i ){
			
			const std::vector< bool > incoming = randomHamming1Neighbour( incumbent );
			const double value = testCase.value( incoming );
			
			// linear annealing schedule...
			const double temperature = linear_interpolate(
					i, 0, numEvaluations - 1, 
					saScheduleUpperBound, saScheduleLowerBound ); 
			if( SAAccept( lastValue, value, temperature ) ) {
				incumbent = incoming;
				lastValue = value;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
