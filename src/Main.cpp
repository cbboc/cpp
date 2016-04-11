
#include "NAHCCompetitor.hpp"
#include "RandomCompetitor.hpp"
#include "SAHHCompetitor.hpp"

 #include "cbboc/CBBOC.hpp"

 #include <cstdlib>

//////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {

	try {
		// RandomCompetitor competitor;
		NAHCCompetitor competitor;
		// SAHHCompetitor competitor( TrainingCategory::SHORT );
		CBBOC::run( competitor );
	}
	catch( std::exception& ex ) {
		std::cerr << "caught std exception in main, what=" << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch( ... ) {
		std::cerr << "caught non-std exception in main" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "All done." << std::endl;
	return EXIT_SUCCESS;
}

// End ///////////////////////////////////////////////////////////////
