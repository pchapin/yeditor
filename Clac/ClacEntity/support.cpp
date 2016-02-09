/*! \file    support.cpp
 *  \brief   Definitions of various support functions.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <cctype>

#include "DisplayState.hpp"
#include "support.hpp"

using namespace std;

void underflow( )
{
    throw "Stack Underflow";
}


Entity *type_mismatch( Entity * )
{
    error_message( "Type Mismatch" );
    return 0;
}


Entity *type_mismatch( Entity *, Entity * )
{
    error_message( "Type Mismatch" );
    return 0;
}


double to_radians( double number )
{
    switch( DisplayState::get_angle_mode( ) ) {

    case DisplayState::DEGREES:
        return number * 3.141592653589793 / 180.0;

    case DisplayState::RADIANS:
        return number;

    case DisplayState::GRADIANS:
        return number * 3.141592653589793 / 200.0;
    }
    return number;
}


double from_radians( double number )
{
    switch( DisplayState::get_angle_mode( ) ) {

    case DisplayState::DEGREES:
        return number * 180.0 / 3.141592653589793;

    case DisplayState::RADIANS:
        return number;

    case DisplayState::GRADIANS:
        return number * 200.0 / 3.141592653589793;
    }
    return number;
}


int stricmp( char *A, char *B )
{
    while( *A && *B ) {
        if( toupper( *A ) != toupper( *B ) ) return 1;
        A++;
        B++;
    }
    if( *A == '\0' && *B == '\0' ) return 0;
    return 1;
}
