#ifndef CBBOC_CBBOC_HPP
#define CBBOC_CBBOC_HPP

//////////////////////////////////////////////////////////////////////

#include "jsoncons/jsoncons.hpp"
#include "jsoncons/json.hpp"

#include "CBBOCUtil.hpp"
#include "Competitor.hpp"
#include "ProblemClass.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <typeinfo>

//////////////////////////////////////////////////////////////////////

class CBBOC {

	static const long BASE_TIME_PER_INSTANCE_IN_MILLIS = 60 * 1000L;

	friend class ObjectiveFn;

	static std::vector< std::string >
	read_text_file( const std::string& path ) {

		std::ifstream ifs( path );

		std::vector< std::string > tokens;
		std::copy( std::istream_iterator< std::string >( ifs ),
				std::istream_iterator<std::string>(),
				std::back_inserter( tokens ) );

		return tokens;
	}

public:

	static void run( Competitor& client )	{

		const std::string root = "./resources/";
		const std::string problemClassFile = root + "classFolder.txt";

		const std::vector< std::string > lines = read_text_file( problemClassFile );
		if( lines.empty() )
			throw std::runtime_error( "bad format for classFolder.txt" );

		const std::string problemClassName = lines.front();
		const std::string problemFolder( root + problemClassName );
		ProblemClass problemClass( problemFolder, client.getTrainingCategory() );
		
		long long actualTestingTime = -1, actualTrainingTime = -1;

		switch( client.getTrainingCategory() ) {
			case TrainingCategory::NONE : {
				actualTestingTime = testClient( client, problemClass.getTestingInstances() );
				std::clog << "actualTestingTime:" << actualTestingTime << std::endl;
			} break;
			case TrainingCategory::SHORT :
			case TrainingCategory::LONG : {
				actualTrainingTime = trainClient( client, problemClass.getTrainingInstances() );
				std::clog <<"actualTrainingTime:" << actualTrainingTime << std::endl;
				
				actualTestingTime = testClient( client, problemClass.getTestingInstances() );
				std::clog <<"actualTestingTime:" << actualTestingTime << std::endl;	
			} break;
			default : 
				throw std::logic_error( "Bad training category in CBBOC.run()" );
		}

		///////////////////////////

		const std::time_t now = std::time(NULL);
		char buffer[ 1024 ];
		std::strftime( buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", // "%T",
			std::localtime(&now));

		const std::string timestamp( buffer );

		const std::string className = classname( client );
		OutputResults results( className, timestamp, problemClassName, problemClass, actualTrainingTime, actualTestingTime );

		std::string outputPath = problemFolder + "/results/";
		outputPath += "CBBOCresults-" + className + "-" + problemClassName + "-" + timestamp + ".json";
		std::ofstream out( outputPath );
		out << results.toJSonString() << std::endl;

		///////////////////////////

		std::cout << results.toJSonString() << std::endl;
		
		// ResultStats stats = resultStats( results.getTestingResults() ); 
		// stats.output( std::cout );
	}

private:
	
	static long long& trainingEndTime() {
		static long long trainingEndTime_ = -1;
		return trainingEndTime_;
	}

	static long long& testingEndTime() {
		static long long testingEndTime_ = -1;
		return testingEndTime_;
	}
	
	////////////////////////////////
	
	struct EvaluationsExceededException : public std::runtime_error {
		EvaluationsExceededException() : std::runtime_error( "Evaluations exceeded" ) {}
	};
	
	struct TimeExceededException : public std::runtime_error {
		TimeExceededException() : std::runtime_error( "Time exceeded" ) {}	
	};
	
	////////////////////////////////

	template < typename T >
	static std::string classname( const T& x ) {
		std::string result = typeid( x ).name();
		auto it = result.begin();
		for( ; std::isdigit( static_cast< int >( *it ) ) && it != result.end(); ++it )
			;

		result.erase( result.begin(), it );

		return result;
	}

	////////////////////////////////

	static long long trainClient( Competitor& client, std::vector< ObjectiveFn >& p ) {
		
		const long long startTime = system_current_time_millis();		
		// const long long maxTime = getTrainingTimeInMillis( client.getTrainingCategory() );
		const long maxTime = BASE_TIME_PER_INSTANCE_IN_MILLIS * p.size() * getMultiplier( client.getTrainingCategory() );
		trainingEndTime() = startTime + maxTime;
		
		try {
			client.train( p, maxTime );
		}
		catch( EvaluationsExceededException& ) {
			// Intentionally empty
		}
		catch( TimeExceededException& ) {
			// Intentionally empty
		}
		
		const long long endTime = system_current_time_millis();
		return endTime - startTime;
	}

	////////////////////////////////

	struct Result {
		const long remainingEvaluations;
		const long remainingEvaluationsWhenBestReached;
		const double bestValue;

		Result( long _remainingEvaluations, long _remainingEvaluationsWhenBestReached, double _bestValue )
		: remainingEvaluations( _remainingEvaluations ),
		  remainingEvaluationsWhenBestReached( _remainingEvaluationsWhenBestReached), bestValue( _bestValue ) {}
	};

	class OutputResults {
		std::string competitorName;
		std::string competitorLanguage;
		std::string problemClassName;
		TrainingCategory trainingCategory;
		std::string datetime;
		std::vector< Result > trainingResults;
		long trainingWallClockUsage;
		std::vector< Result > testingResults;
		long testingWallClockUsage;

		///////////////////////////

		public:

		OutputResults( const std::string& _competitorName, const std::string& _datetime,
			const std::string& _problemClassName,
			const ProblemClass& problemClass,
			long _trainingWallClockUsage,
			long _testingWallClockUsage )
		: competitorName( _competitorName ),
		  competitorLanguage( "C++" ),
		  problemClassName( _problemClassName ),
		  trainingCategory( problemClass.getTrainingCategory() ),
		  datetime( _datetime ),
		  trainingWallClockUsage( _trainingWallClockUsage ),
		  testingWallClockUsage( _testingWallClockUsage )
		{
			for( size_t i=0; i<problemClass.getTrainingInstances().size(); ++i ) {
				const ObjectiveFn& o = problemClass.getTrainingInstances()[ i ];
				const auto p = o.getRemainingEvaluationsAtBestValue();
				trainingResults.push_back( Result( o.getRemainingEvaluations(), p.first, p.second ) );
			}

			for( size_t i=0; i<problemClass.getTestingInstances().size(); ++i ) {
				const ObjectiveFn& o = problemClass.getTestingInstances()[ i ];
				const auto p = o.getRemainingEvaluationsAtBestValue();
				testingResults.push_back( Result( o.getRemainingEvaluations(), p.first, p.second ) );
			}
		}

		const std::vector< Result >& getTestingResults() const { return testingResults; }

		///////////////////////////

		std::string toJSonString() const {

			jsoncons::json result;

			jsoncons::json training( jsoncons::json::an_array );
			for( size_t i=0; i<trainingResults.size(); ++i ) {
				const Result& r = trainingResults[ i ];
				jsoncons::json elem;
				elem["remainingEvaluations"] = r.remainingEvaluations;
				elem["remainingEvaluationsWhenBestReached"] = r.remainingEvaluationsWhenBestReached;
				elem["bestValue"] = r.bestValue;
				training.add( elem );
			}

			jsoncons::json testing( jsoncons::json::an_array );
			for( size_t i=0; i<testingResults.size(); ++i ) {
				const Result& r = testingResults[ i ];
				jsoncons::json elem;
				elem["remainingEvaluations"] = r.remainingEvaluations;
				elem["remainingEvaluationsWhenBestReached"] = r.remainingEvaluationsWhenBestReached;
				elem["bestValue"] = r.bestValue;
				testing.add( elem );
			}

			result["competitorName"] = competitorName;
			result["competitorLanguage"] = competitorLanguage;
			result["problemClassName"] = problemClassName;
			std::ostringstream ostc;
			// ostc << trainingCategory;
			ostc << numericCode( trainingCategory );
			const std::string trainingCategoryStr( ostc.str() );
			result["trainingCategory"] = trainingCategoryStr;
			result["datetime"] = datetime;
			result["trainingResults" ] = std::move( training );
			result["trainingWallClockUsage"] = trainingWallClockUsage;
			result["testingResults" ] = std::move( testing );
			result["testingWallClockUsage"] = testingWallClockUsage;

			std::ostringstream os;
			os << pretty_print( result ) << std::endl;
			return os.str();
		}
	};

	///////////////////////////////

	static long long testClient( Competitor& client, std::vector< ObjectiveFn >& fns ) {
		const long long startTime = system_current_time_millis();		

		for( size_t i=0; i<fns.size(); ++i ) {
			try {
				const long long now = system_current_time_millis();
				testingEndTime() = now + BASE_TIME_PER_INSTANCE_IN_MILLIS;

				ObjectiveFn& o = fns[ i ];
				client.test( o, BASE_TIME_PER_INSTANCE_IN_MILLIS );
			}
			catch( EvaluationsExceededException& ) {
				// Intentionally empty
			}
			catch( TimeExceededException& ) {
				// Intentionally empty
			}
		}
		
		const long long endTime = system_current_time_millis();
		return endTime - startTime;
	}
	
	class ResultStats {
			
		const double bestValueMean_, bestValueSD_;
		const double remainingEvaluationsWhenBestReachedMean_, remainingEvaluationsWhenBestReachedSD_;
		
	public:			
		ResultStats( double bestValueMean, double bestValueSD, double remainingEvaluationsWhenBestReachedMean, double remainingEvaluationsWhenBestReachedSD ) 
		: bestValueMean_(bestValueMean ), 
		  bestValueSD_(bestValueSD),
		  remainingEvaluationsWhenBestReachedMean_(remainingEvaluationsWhenBestReachedMean), 
		  remainingEvaluationsWhenBestReachedSD_(remainingEvaluationsWhenBestReachedSD) {}
			
		void output(std::ostream& os) const {
			os << "bestValueMean: " << bestValueMean_ << '\n';
			os << "bestValueSD: " << bestValueSD_ << '\n';
			os << "remainingEvaluationsWhenBestReachedMean: " << remainingEvaluationsWhenBestReachedMean_ << '\n';
			os << "remainingEvaluationsWhenBestReachedSD: " << remainingEvaluationsWhenBestReachedSD_ << '\n';
		}
	};
	
	static double mean(const std::vector<double>& v) {	
		double sum = std::accumulate(v.begin(), v.end(), 0.0);
		double mean = sum / v.size();
		return mean;
	}

	static double sd(const std::vector<double>& v) {	
		double mn = mean(v);
		double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
		double stdev = std::sqrt(sq_sum / v.size() - mn * mn);
		return stdev;
	}
	
	static ResultStats resultStats( const std::vector< Result >& results ) {
			
		std::vector< double > bestValues, remainingEvaluationsWhenBestReached;
		for( int i=0; i<results.size(); ++i ) {
			bestValues.push_back( results[ i ].bestValue );
			remainingEvaluationsWhenBestReached.push_back( results[ i ].remainingEvaluationsWhenBestReached );
		}
			
		const double bestValueMean = mean( bestValues );
		const double bestValueSD = sd( bestValues );
		const double remainingEvaluationsWhenBestReachedMean = mean( remainingEvaluationsWhenBestReached );
		const double remainingEvaluationsWhenBestReachedSD = sd( remainingEvaluationsWhenBestReached );
		return ResultStats( bestValueMean, bestValueSD, remainingEvaluationsWhenBestReachedMean, remainingEvaluationsWhenBestReachedSD );
	}
};

//////////////////////////////////////////////////////////////////////

inline
double ObjectiveFn::value( const std::vector< bool >& candidate ) {

	// Included in CBBOC.hpp in order to resolve the 
	// dependency between ObjectiveFn and CBBOC to be resolved
	// in a way that allows `header file only' compilation.

	const long long timeNow = system_current_time_millis();
	if( timingMode == TimingMode::TRAINING ) {
		if( timeNow > CBBOC::trainingEndTime() )
			throw CBBOC::TimeExceededException();
	}
	else if( timingMode == TimingMode::TESTING ) {
		if( timeNow > CBBOC::testingEndTime() )
			throw CBBOC::TimeExceededException();
	}
	else {
		throw std::logic_error( "invalid timingMode in ObjectiveFn.value" );
	}
		
	///////////////////////////
		
	if( *remainingEvaluations <= 0 ) {
		throw CBBOC::EvaluationsExceededException();
	}
	else {
		const double value = instance.value( candidate );
		--*remainingEvaluations;

		// We are maximizing...
		if( remainingEvaluationsAtBestValue == nullptr )
			remainingEvaluationsAtBestValue.reset( new std::pair< long, double >( getRemainingEvaluations(), value ) );
		else if( value > remainingEvaluationsAtBestValue->second ) {
			*remainingEvaluationsAtBestValue = std::make_pair( getRemainingEvaluations(), value );
		}

		return value;
	}
}

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
