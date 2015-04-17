#ifndef CBBOC2015_PROBLEMINSTANCE_HPP
#define CBBOC2015_PROBLEMINSTANCE_HPP

#include "CBBOC2015Util.hpp"

#include <cassert>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

//////////////////////////////////////////////////////////////////////

class ProblemInstance {
	size_t numGenes;
	int maxEvalsPerInstance;
	int K;
	std::vector< std::pair< std::vector< int >, std::vector< double > > > data;

	///////////////////////////////

	static bool allValidSize( const std::vector< std::pair< std::vector< int >, std::vector< double > > >& data, int k ) {

		const long expectedSize1 = k + 1;
		const long expectedSize2 = 1 << ( k + 1 );

		for( size_t i=0; i<data.size(); ++i ) {
			const std::pair< std::vector< int >, std::vector< double > >& p = data[ i ];
			const long size1 = p.first.size();
			const long size2 = p.second.size();
			if( size1 != expectedSize1 || size2 != expectedSize2 )
				return false;
		}
		
		return true;
	}

public:
	
	ProblemInstance( std::basic_istream< char >& is )
	: numGenes( 0 ), maxEvalsPerInstance( 0 ), K( 0 ) {
		
		is >> numGenes;
		is >> maxEvalsPerInstance;
		is >> K;
			
		for( size_t i=0; i<numGenes; ++i ) {
			int idummy;
			std::vector< int > iarray;
			for( int j=0; j<K + 1; ++j ) {
				is >> idummy;
				iarray.push_back( idummy );
			}

			double ddummy;
			const int numFks = 1 << ( K + 1 );
			std::vector< double > darray;
			for( int j=0; j<numFks; ++j ) {
				is >> ddummy;
				darray.push_back( ddummy );
			}
				
			data.push_back( std::make_pair( iarray, darray ) );
		}
		
		assert( invariant() );
	}

	///////////////////////////////

	size_t getNumGenes() const { return numGenes;	}
	int getMaxEvalsPerInstance() const { return maxEvalsPerInstance; }

	///////////////////////////////
	
	double value( const std::vector< bool >& candidate ) const {
		if( candidate.size() != getNumGenes() )
			throw std::invalid_argument( "Bad argument to ProblemInstance.value" ); // "candidate of length " + getNumGenes() + " expected, found " + candidate.length );
		
		double total = 0.0;
		for( size_t i=0; i<getNumGenes(); ++i ) {

			const std::vector< int >& varIndices = data[ i ].first;
			int fnTableIndex = 0;
			for( size_t j=0; j<varIndices.size(); ++j ) {
				fnTableIndex <<= 1;
				fnTableIndex |= candidate[ varIndices[ j ] ] ? 1 : 0;
			}
			
			total += data[ i ].second[ fnTableIndex ];
		}
		
		return total;
	}
	
	///////////////////////////////	

	friend std::ostream& operator <<( std::ostream& s, const ProblemInstance&  x ) {
		s << "ProblemInstance[";		
		s << "numGenes=" << x.numGenes;
		s << ",maxEvalsPerInstance=" << x.maxEvalsPerInstance;
		s << ",K=" << x.K;
		
		s << ",data=[\n";		
		for( size_t i=0; i<x.data.size(); ++i ) {
			
			using namespace cbboc_std_io;

			s << "(" << x.data[ i ].first;
			s << "," << x.data[ i ].second;
			s << ")\n";			
		}
		return s << "]]";	
	}
	
	///////////////////////////////	
	
	bool invariant() const {
		return getNumGenes() > 0 && 
			getMaxEvalsPerInstance() > 0 &&
			K > 0 && 
			data.size() == getNumGenes() &&
			allValidSize( data, K );
	}

	///////////////////////////////	
	
	/*****
	public static void main( String [] args ) throws IOException {
		String root = System.getProperty( "user.dir" );
		// String path = root + "/resources/" + "00000.txt";
		String path = root + "/resources/" + "toy.txt";
		
		InputStream is = new FileInputStream( path );
	
		ProblemInstance prob = new ProblemInstance( is );
		System.out.println( prob );
		
		boolean [] candidate = new boolean [ prob.getNumGenes() ];
		System.out.println( prob.value( candidate ) );
		
		System.out.println( "All done" );		
	}
	*****/
};

//////////////////////////////////////////////////////////////////////

#endif

// End ///////////////////////////////////////////////////////////////
