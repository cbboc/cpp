#ifndef CBBOC_OBJECTIVEFN_HPP
#define CBBOC_OBJECTIVEFN_HPP

#include "ProblemInstance.hpp"

#include <limits>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////////////

class ObjectiveFn {
public:

	enum TimingMode {
		TRAINING, TESTING
	};

	friend std::ostream& operator <<( std::ostream& s, TimingMode x ) {
		switch( x ) {
			case TRAINING : return s << "TRAINING"; break;
			case TESTING : return s << "TESTING"; break;
			default : return s << "<<invalid TimingMode value>>"; break;
		}
	}

private:

	ProblemInstance instance;
	TimingMode timingMode;
	std::shared_ptr< long > remainingEvaluations;
	std::unique_ptr< std::pair< long, double > > remainingEvaluationsAtBestValue;
	
	///////////////////////////////
	
public:

	ObjectiveFn( ProblemInstance instance_, TimingMode timingMode_, std::shared_ptr< long > remainingEvaluations_ ) 
	: instance( instance_ ), timingMode( timingMode_ ), remainingEvaluations( remainingEvaluations_ ) {}

	///////////////////////////////
	
	double value( const std::vector< bool >& candidate );
	// Implementation moved to CBBOC2015.hpp to allow `header file only' compilation.

	///////////////////////////////

	std::pair< long, double > getRemainingEvaluationsAtBestValue() const {
		if( remainingEvaluationsAtBestValue == nullptr )
			return std::make_pair( -1L, -1.0 );
		else
			return *remainingEvaluationsAtBestValue;
	}
	
	int getNumGenes() const { return instance.getNumGenes(); }	
	long getRemainingEvaluations() const { return *remainingEvaluations; }
	
	///////////////////////////////	
	
	friend std::ostream& operator <<( std::ostream& s, const ObjectiveFn& x ) {
		s << "ObjectiveFn(numGenes:" << x.getNumGenes(); 
		s << ",remainingEvaluations: " << x.getRemainingEvaluations();
		if( x.remainingEvaluationsAtBestValue != nullptr ) {
			s << ",remainingEvaluationsAtBestValue: " << x.remainingEvaluationsAtBestValue->first; 
			s << ",bestValue: " << x.remainingEvaluationsAtBestValue->second;
		}
		// if( x.remainingEvaluationsAtBestValue != nullptr )
		else {
			s << ",remainingEvaluationsAtBestValue: -1";
			s << ",bestValue: -1.0";
		}
		return s << ",trainingCategory: " << x.timingMode << ")";
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
