/*! \file    check.cpp
    \brief   This file contains a test program for "simple" windows.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstdio>
#include <cstdlib>

#include "scr.hpp"
#include "check.hpp"

using namespace std;

/*==================================*/
/*           Main Program           */
/*==================================*/

int main( )
{
    // Testing initialization function.
    printf( "Ready to initialize! Hit ENTER..." );
    while( getchar( ) != '\n' ) /* Null */ ;
    if( scr::initialize( ) == false ) {
        printf( "scr::initialize( ) returned an error status!\n" );
        return( 1 );
    }

    bool done = false;
    while( !done ) {
        CLEAR_SCREEN;
        scr::print_text( 1, 1, 80, "0: Quit" );
        scr::print_text( 2, 1, 80, "1: Basic Test" );
        scr::print_text( 3, 1, 80, "2: Key Test" );
        scr::refresh( );

        int Command = scr::key( );
        CLEAR_SCREEN;
        switch( Command ) {
            case '0': done = true; break;
            case '1': basic_test( ); break;
            case '2': key_test( ); break;
        }
    }

    // Testing termination function.
    scr::terminate( );
    printf( "That's all folks... (This should be in upper left corner)\n" );
    scr::terminate( );
    printf( "Screen should not have just cleared.\n" );
    return( 0 );
}
