
#ifndef CBBOC2015_PROBLEMCLASS_HPP
#define CBBOC2015_PROBLEMCLASS_HPP

//////////////////////////////////////////////////////////////////////

#include "CBBOC2015Util.hpp"
#include "ObjectiveFn.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////

class ProblemClass {
	std::vector< ObjectiveFn > training, testing;
	const TrainingCategory trainingCategory;
public:

	ProblemClass( const std::string& root, TrainingCategory _trainingCategory )
	: trainingCategory( _trainingCategory ) {

		const std::string trainingFilesInventory = root + "/trainingFiles.txt";
		const std::string testingFilesInventory = root + "/testingFiles.txt";

		std::vector< std::string > trainingFiles = readInstances( trainingFilesInventory );
		std::vector< std::string > testingFiles = readInstances( testingFilesInventory );

		///////////////////////////

		int numTrainingEvaluationsMultiplier = 0;
		switch( trainingCategory ) {
			case NONE : numTrainingEvaluationsMultiplier = 0; break;
			case SHORT : numTrainingEvaluationsMultiplier = 1; break;
			case LONG :	numTrainingEvaluationsMultiplier = 10; break;
		}

		std::vector< ProblemInstance > trainingInstances;
		switch( trainingCategory ) {
			case NONE :
				// Intentionally Empty
			break;
			case SHORT :
				for( size_t i=0; i<trainingFiles.size(); ++i ) {
					std::ifstream ifs( root + "/" + trainingFiles[ i ] );
					trainingInstances.push_back( ProblemInstance( ifs ) );
				}
			break;
			case LONG :
				for( size_t i=0; i<trainingFiles.size(); ++i ) {
					std::ifstream ifs( root + "/" + trainingFiles[ i ] );
					trainingInstances.push_back( ProblemInstance( ifs ) );
				}
			break;
		}

		long totalTrainingEvaluations = 0;
		for( size_t i=0; i<trainingInstances.size(); ++i ) {
			const ProblemInstance& p = trainingInstances[ i ];
			totalTrainingEvaluations += p.getMaxEvalsPerInstance();
		}

		std::shared_ptr< long > sharedTrainingEvaluations( new long( totalTrainingEvaluations * numTrainingEvaluationsMultiplier ) );
		for( size_t i=0; i<trainingInstances.size(); ++i )
			training.push_back( ObjectiveFn( trainingInstances[ i ], ObjectiveFn::TimingMode::TRAINING, sharedTrainingEvaluations ) );

		///////////////////////////

		std::vector< ProblemInstance > testingInstances;
		for( size_t i=0; i<testingFiles.size(); ++i ) {
			std::ifstream ifs( root + "/" + testingFiles[ i ] );
			testingInstances.push_back( ProblemInstance( ifs ) );
		}

		for( size_t i=0; i<testingInstances.size(); ++i ) {
			const ProblemInstance& p = testingInstances[ i ];
			std::shared_ptr< long > individualTestingEvaluations( new long( p.getMaxEvalsPerInstance() ) );
			testing.push_back( ObjectiveFn( p, ObjectiveFn::TimingMode::TESTING, individualTestingEvaluations ) );
		}
	}

	///////////////////////////////
	
	std::vector< ObjectiveFn >& getTrainingInstances() { return training; }
	std::vector< ObjectiveFn >& getTestingInstances() { return testing; }

	TrainingCategory getTrainingCategory() const { return trainingCategory; }
	const std::vector< ObjectiveFn >& getTrainingInstances() const { return training; }
	const std::vector< ObjectiveFn >& getTestingInstances() const { return testing; }
	
	///////////////////////////////	
	
private:
	
	static std::vector< std::string >
	readInstances( const std::string& testingFilesTxt ) {

		std::ifstream ifs( testingFilesTxt );
		std::vector< std::string > tokens;
		std::copy( std::istream_iterator< std::string >( ifs ),
			std::istream_iterator<std::string>(),
			std::back_inserter( tokens ) );

		if( tokens.empty() )
			throw std::runtime_error( "bad file format in readInstances: " +  testingFilesTxt);

		const std::string& str = tokens.front();
		const int numInstances = std::atoi( str.c_str() );
		if( tokens.size() != static_cast< size_t >( numInstances + 1 ) )
			throw std::runtime_error( "bad numInstances in readInstances" );

		tokens.erase( tokens.begin() );
		return tokens;
	}

	///////////////////////////////

	friend std::ostream& operator <<( std::ostream& s, const ProblemClass& x ) {
		using namespace cbboc_std_io;
		s << "cbboc.ProblemClass@" << &x << "[";
		s << "training=" << x.getTrainingInstances();
		return s << ",testing=" << x.getTestingInstances() << ")";
	}
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////

