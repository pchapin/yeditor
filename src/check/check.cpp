/*! \file    check.cpp
 *  \brief   Main program for Y's unit tests.
 *  \author  Peter C. Chapin <chapinp@acm.org>
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include "check.hpp"
#include "UnitTestManager.hpp"

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

    UnitTestManager::register_test( EditBuffer_tests, "EditBuffer" );
    UnitTestManager::register_test( EditList_tests, "EditList" );
    UnitTestManager::execute_tests( *output, "Y Unit Tests" );
    return UnitTestManager::test_status( );
}
