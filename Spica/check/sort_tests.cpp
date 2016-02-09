/*! \file    sort_tests.cpp
    \brief   Exercise Spica's C++ sorting algorithms.
    \author  Peter C. Chapin <pcc482719@gmail.com>
*/

#include <cstring>
#include <iostream>
#include <functional>
#include <spica.hpp>
#include "u_tests.hpp"
#include "UnitTestManager.hpp"

// Test cases can contain no more than this many elements.
const int MAX_SIZE = 16;

struct sort_test_case {
  int   input[MAX_SIZE];    // The array to sort.
  int   expected[MAX_SIZE]; // The expected result.
  int   size;               // The number of elements in input[] I care about.
};

struct sort_test_case tests[] = {
    // An empty sequence
    { { 0 },
      { 0 }, 0 },

    // One element
    { { 1 },
      { 1 }, 1 },

    // Two elements
    { { 2, 1 },
      { 1, 2 }, 2 },

    // Three elements
    { { 2, 3, 1 },
      { 1, 2, 3 }, 3 },

    // Random elements, size 2^n
    { { 4, 2, 9, 5, 3, 8, 7, 0 },
      { 0, 2, 3, 4, 5, 7, 8, 9 }, 8 },

    // Random elements, size 2^n + 1
    { { 0, 8, 2, 1, 3, 9, 4, 6, 5 },
      { 0, 1, 2, 3, 4, 5, 6, 8, 9 }, 9 },

    // Random elements, size 2^n - 1
    { { 4, 2, 7, 3, 1, 9, 8 },
      { 1, 2, 3, 4, 7, 8, 9 }, 7 },

    // Already sorted
    { { 1, 3, 5, 7, 9 },
      { 1, 3, 5, 7, 9 }, 5 },

    // Already reverse sorted
    { { 9, 7, 5, 3, 1 },
      { 1, 3, 5, 7, 9 }, 5 },

    // One pair of duplicate elements
    { { 1, 5, 7, 5, 6 },
      { 1, 5, 5, 6, 7 }, 5 },

    // All duplicate elements
    { { 3, 3, 3, 3, 3 },
      { 3, 3, 3, 3, 3 }, 5 },

    // Some large numbers (interesting to Radix Sort)
    { { 0x00123456, 0x00FEDCBA, 0x00123456, 0x12121212, 0x00120012 },
      { 0x00120012, 0x00123456, 0x00123456, 0x00FEDCBA, 0x12121212 }, 5 }
};

// Radix Sort doesn't require a comparison. However, the test fixture does.
template< typename Bidirectional, typename SWO >
void radix_sort_wrapper( Bidirectional first, Bidirectional last, SWO )
{
    spica::radix_sort( first, last );
}

typedef void sort_function( int *, int *, std::less< int > );
sort_function *test_functions[] = {
    spica::bubble_sort,
    spica::selection_sort,
    spica::insertion_sort,
    spica::merge_sort,
    spica::quick_sort,
    radix_sort_wrapper,
    0
};

bool sort_tests( )
{
    int workspace[MAX_SIZE];
    int i = 0;
    while( test_functions[i] != 0 ) {
        for( size_t j = 0; j < sizeof( tests ) / sizeof( struct sort_test_case ); ++j ) {

            std::memcpy(workspace, tests[j].input, tests[j].size * sizeof( int ) );
            test_functions[i](workspace, workspace + tests[j].size, std::less< int >( ) );

            for( int k = 0; k < tests[j].size; ++k ) {
                UNIT_CHECK( workspace[k] == tests[j].expected[k] );
            }
        }
        ++i;
    }
    return true;
}

