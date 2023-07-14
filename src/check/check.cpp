/*! \file    check.cpp
 *  \brief   Main program for Y's unit tests.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

// From SpicaCpp.
#include "UnitTestManager.hpp"

#include "check.hpp"

int main( int argc, char **argv )
{
    std::ostream *output = &std::cout;
    std::ofstream output_file;

    if( argc == 2 ) {
        output_file.open( argv[1] );
        if( !output_file ) {
            std::cerr << "Unable to open " << argv[1] << " for output!\n";
            return EXIT_FAILURE;
        }
        output = &output_file;
    }

    UnitTestManager::register_suite( EditBuffer_tests, "EditBuffer" );
    UnitTestManager::register_suite( EditList_tests, "EditList" );

    UnitTestManager::execute_suites( *output, "Y Unit Tests" );
    return UnitTestManager::test_status( );
}
