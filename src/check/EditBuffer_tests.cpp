/*! \file    EditBuffer_tests.cpp
 *  \brief   EditBuffer unit tests.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * The tests below take into account the fact that edit buffers are initially allocated with a
 * capacity of 8. These tests explore the boundary between a size of 8 and a size of 9 to verify
 * that no errors occur just before or just after a re-allocation of internal memory. If
 * EditBuffer's initial capacity is changed, these tests should be adjusted to remain effective.
 */

#include <cstring>

// From Y.
#include "EditBuffer.hpp"

// From SpicaCpp.
#include "UnitTestManager.hpp"

#include "check.hpp"

namespace {

    void EditBuffer_compare( const EditBuffer &buffer, const char *expected )
    {
        UNIT_CHECK( buffer.length( ) == std::strlen( expected ) );
        for ( std::size_t i = 0; i < buffer.length( ); ++i ) {
            UNIT_CHECK( buffer[i] == expected[i] );
        }
    }

    void constructor_tests( )
    {
        UnitTestManager::UnitTest test( "constructor_tests" );
        
        EditBuffer test_buffer1;
        EditBuffer test_buffer2{ "Hello" };
        EditBuffer test_buffer3{ test_buffer2 };

        // Check to see if the constructors worked.
        UNIT_CHECK( test_buffer1.length( ) == 0 );
        EditBuffer_compare( test_buffer2, "Hello" );
        EditBuffer_compare( test_buffer3, "Hello" );
    }

    void assignment_tests( )
    {
        UnitTestManager::UnitTest test( "assignment_tests" );
        
        EditBuffer test_buffer1;
        EditBuffer test_buffer2{ "Hello" };

        // Check assignment operator.
        test_buffer1 = test_buffer2;
        EditBuffer_compare( test_buffer1, "Hello" );
        test_buffer1 = "";
        UNIT_CHECK( test_buffer1.length( ) == 0 );
    }

    void insert_tests( )
    {
        UnitTestManager::UnitTest test( "insert_tests" );
        
        EditBuffer test_buffer1;

        // Check insert.
        test_buffer1 = "";
        test_buffer1.insert( 'x', 0 );
        EditBuffer_compare( test_buffer1, "x" );
        test_buffer1.insert( 'y', 0 );
        EditBuffer_compare( test_buffer1, "yx" );
        test_buffer1.insert( 'z', 1 );
        EditBuffer_compare( test_buffer1, "yzx" );
        test_buffer1.insert( 'w', 3 );
        EditBuffer_compare( test_buffer1, "yzxw" );
        test_buffer1.insert( 'x', 7 );
        UNIT_CHECK( test_buffer1.length( ) == 8 );
        EditBuffer_compare( test_buffer1, "yzxw   x" );
        test_buffer1.insert( 'y', 8 );
        EditBuffer_compare( test_buffer1, "yzxw   xy" );
        test_buffer1.erase( );
        test_buffer1.insert( 'x', 16 );
        EditBuffer_compare( test_buffer1, "                x" );
    }

    void replace_tests( )
    {
        UnitTestManager::UnitTest test( "replace_tests" );
        
        EditBuffer test_buffer1;

        // Check replace. Note that replace is the same as insert if the offset is too large.
        test_buffer1 = "Hello";
        test_buffer1.replace( 'x', 0 );
        EditBuffer_compare( test_buffer1, "xello" );
        test_buffer1.replace( 'y', 5 );
        EditBuffer_compare( test_buffer1, "xelloy" );
        test_buffer1.replace( 'z', 7 );
        EditBuffer_compare( test_buffer1, "xelloy z" );
        UNIT_CHECK( test_buffer1.length( ) == 8 );
        test_buffer1.replace( 'w', 7 );
        EditBuffer_compare( test_buffer1, "xelloy w" );
        UNIT_CHECK( test_buffer1.length( ) == 8 );
    }

    void erase_tests( )
    {
        UnitTestManager::UnitTest test( "erase_tests" );
        
        EditBuffer test_buffer1;

        // Check erase.
        test_buffer1 = "Hello";
        UNIT_CHECK( test_buffer1.erase( 0 ) == 'H' );
        EditBuffer_compare( test_buffer1, "ello" );
        UNIT_CHECK( test_buffer1.erase( 3 ) == 'o' );
        EditBuffer_compare( test_buffer1, "ell" );
        UNIT_CHECK( test_buffer1.erase( 3 ) == '\0' );
        EditBuffer_compare( test_buffer1, "ell" );
        test_buffer1.erase( );
        UNIT_CHECK( test_buffer1.length( ) == 0 );
    }

    void append_tests( )
    {
        UnitTestManager::UnitTest test( "append_tests" );
        
        EditBuffer test_buffer1;

        // Check append.
        test_buffer1.erase( );
        test_buffer1.append( '1' );
        EditBuffer_compare( test_buffer1, "1" );
        test_buffer1.append( "2345678" );
        EditBuffer_compare( test_buffer1, "12345678" );
        test_buffer1.append( EditBuffer( "9" ) );
        EditBuffer_compare( test_buffer1, "123456789" );
    }

    void subbuffer_tests( )
    {
        UnitTestManager::UnitTest test( "subbuffer_tests" );
        
        EditBuffer test_buffer1;
        EditBuffer test_buffer2;

        // Check subbuffer.
        test_buffer1.erase( );
        test_buffer1.append( "0123456789ABCDEF" );
        test_buffer2 = test_buffer1.subbuffer( 1, 10 );
        EditBuffer_compare( test_buffer2, "123456789" );
        test_buffer2 = test_buffer1.subbuffer( 10, 20 );
        EditBuffer_compare( test_buffer2, "ABCDEF    " );
        test_buffer2 = test_buffer1.subbuffer( 15, 16 );
        EditBuffer_compare( test_buffer2, "F" );
        test_buffer2 = test_buffer1.subbuffer( 15, 15 );
        EditBuffer_compare( test_buffer2, "" );
        test_buffer2 = test_buffer1.subbuffer( 15, 14 );
        EditBuffer_compare( test_buffer2, "" );
        test_buffer2 = test_buffer1.subbuffer( 20, 30 );
        EditBuffer_compare( test_buffer2, "          " );
    }

    void trim_tests( )
    {
        UnitTestManager::UnitTest test( "trim_tests" );
        
        EditBuffer test_buffer1;

        // Check trim.
        test_buffer1 = "Hello, World!";
        test_buffer1.trim( 8 );
        EditBuffer_compare( test_buffer1, "Hello, W" );
        UNIT_CHECK( test_buffer1.length( ) == 8 );
        test_buffer1.trim( 7 );
        EditBuffer_compare( test_buffer1, "Hello, " );
        UNIT_CHECK( test_buffer1.length( ) == 7 );
        test_buffer1.trim( 0 );
        UNIT_CHECK( test_buffer1.length( ) == 0 );
    }

}


bool EditBuffer_tests( )
{
    constructor_tests( );
    assignment_tests( );
    insert_tests( );
    replace_tests( );
    erase_tests( );
    append_tests( );
    subbuffer_tests( );
    trim_tests( );
    return true;
}
