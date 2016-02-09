/*! \file    BoundedList_tests.cpp
    \brief   Exercise spica::BoundedList.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <exception>
#include <iostream>
#include <stdexcept>

#include <spica.hpp>
#include "u_tests.hpp"
#include "UnitTestManager.hpp"

using namespace spica;

static void make_list( BoundedList<int> &my_list )
{
    for( int i = 0; i < 100; ++i ) {
        my_list.push_back( i );
    }
    UNIT_CHECK( my_list.size( ) == 100 );
}


static void constructor_test( )
{
    BoundedList<int> list_d(   0 );  // Degenerate case.
    BoundedList<int> list_0(   1 );
    BoundedList<int> list_1(  10 );
    BoundedList<int> list_2( 100 );

    UNIT_CHECK( list_d.size( ) == 0 );
    UNIT_CHECK( list_0.size( ) == 0 );
    UNIT_CHECK( list_1.size( ) == 0 );
    UNIT_CHECK( list_1.size( ) == 0 );

    try {
        list_d.push_back( 1 );
        UNIT_FAIL( "Unexpectedly able to push_back to a full BoundedList" );
    }
    catch( std::length_error ) {
        // okay.
    }
    catch( ... ) {
        UNIT_FAIL( "Unexpected exception while executing push_back to a full BoundedList" );
    }
}


static void push_back_test( )
{
    int i;
    BoundedList<int> my_list( 100 );

    make_list( my_list );

    BoundedList<int>::iterator p( my_list.begin( ) );
    i = 0;
    while( p != my_list.end( ) ) {
        UNIT_CHECK( *p == i );
        ++i;
        ++p;
    }
}

static void pop_back_test( )
{
    int i;
    BoundedList<int> my_list( 100 );

    make_list( my_list );

    i = 100;
    do {
        --i;
        UNIT_CHECK( my_list.back( ) == i );
        my_list.pop_back( );
    } while( i > 0 );
    UNIT_CHECK( my_list.size( ) == 0 );
}

static void iterator_test( )
{
    int i;
    BoundedList<int> my_list( 100 );
    BoundedList<int>::iterator p;

    make_list( my_list );

    p = my_list.begin( );
    for( i = 0; i < 100; ++i ) {
        UNIT_CHECK( *p == i );
        UNIT_CHECK( i == 0 || p != my_list.begin( ) );
        UNIT_CHECK( p != my_list.end( ) );
        ++p;
    }
    UNIT_CHECK( p == my_list.end( ) );

    do {
        --i;
        --p;
        UNIT_CHECK( p != my_list.end( ) );
        UNIT_CHECK( i == 0 || p != my_list.begin( ) );
        UNIT_CHECK( *p == i );
    } while( i > 0 );
    UNIT_CHECK( p == my_list.begin( ) );
}


static void insert_test( )
{
    BoundedList<int> my_list( 103 );
    BoundedList<int>::iterator p1, p2;

    make_list( my_list );

    // Try inserting at the beginning.
    p2 = my_list.insert( my_list.begin( ), -1 );
    UNIT_CHECK( *my_list.begin( ) == -1 );
    UNIT_CHECK( *p2 == -1 );
    UNIT_CHECK( my_list.size( ) == 101 );

    // Try inserting at the end.
    p2 = my_list.insert( my_list.end( ), -2 );
    p1 = my_list.end( );
    --p1;
    UNIT_CHECK( *p1 == -2 );
    UNIT_CHECK( *p2 == -2 );
    UNIT_CHECK( my_list.size( ) == 102 );

    // Try inserting in the middle.
    p1 = my_list.begin( );
    for( int i = 0; i < 50; ++i ) ++p1;
    int temp = *p1;
    p2 = my_list.insert( p1, -3 );
    UNIT_CHECK( *p1 == temp );
    UNIT_CHECK( *p2 == -3 );
    UNIT_CHECK( my_list.size( ) == 103 );

    // Try inserting into an empty list.
    BoundedList<int> empty_list( 100 );
    p2 = empty_list.insert( empty_list.begin( ), -1 );
    UNIT_CHECK( *empty_list.begin( ) == -1 );
    UNIT_CHECK( *p2 == -1 );
    UNIT_CHECK( empty_list.size( ) == 1 );
}


static void erase_test( )
{
    BoundedList<int> my_list( 100 );
    BoundedList<int>::iterator p1, p2;
    int i;

    make_list( my_list );

    // Try erasing at the beginning.
    for( i = 0; i < 100; ++i ) {
        UNIT_CHECK( *my_list.begin( ) == i );
        p2 = my_list.erase( my_list.begin( ) );
        UNIT_CHECK( p2 == my_list.end( ) || *p2 == i + 1 );
        UNIT_CHECK( static_cast<int>( my_list.size( ) ) == 100 - i - 1 );
    }

    make_list( my_list );

    // Try erasing at the end.
    p1 = my_list.end( );
    --p1;
    p2 = my_list.erase( p1 );
    UNIT_CHECK( p2 == my_list.end( ) );
    UNIT_CHECK( my_list.size( ) == 99 );

    // Try erasing in the middle.
    p1 = my_list.begin( );
    for( i = 0; i < 50; ++i ) ++p1;
    p2 = p1;
    ++p2;
    int temp = *p2;
    p2 = my_list.erase( p1 );
    UNIT_CHECK( *p2 == temp );
    UNIT_CHECK( my_list.size( ) == 98 );
}


bool BoundedList_tests( )
{
    constructor_test( );
    push_back_test( );
    pop_back_test( );
    iterator_test( );
    insert_test( );
    erase_test( );
    return true;
}
