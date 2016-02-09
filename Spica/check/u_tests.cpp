/*! \file    u_tests.cpp
    \brief   Main unit test driver program for the Spica C++ library.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "u_tests.hpp"
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

    UnitTestManager::register_test( BinomialHeap_tests, "BinomialHeap" );
    UnitTestManager::register_test( BoundedList_tests, "BoundedList" );
    UnitTestManager::register_test( Graph_tests, "Graph" );
    UnitTestManager::register_test( sort_tests, "sort" );

    // TODO: The following tests are interactive, which is not ideal. They're better than nothing.
    UnitTestManager::register_test( str_tests, "str" );
    UnitTestManager::register_test( Timer_tests, "Timer" );

    UnitTestManager::execute_tests( *output, "Spica C++ Tests" );
    return UnitTestManager::test_status( );
}

