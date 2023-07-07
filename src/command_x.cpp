/*! \file    command_x.cpp
 *  \brief   Implementation of the experimental command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cctype>
#include <cstdlib>

#include "command.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"

// Parameter stack commands for the macro language.

bool drop_command( )
{
    if( parameter_stack.size() == 0 ) {
        error_message( "Cannot drop from an empty stack" );
        return false;
    }
    parameter_stack.delete_top( );
    return true;
}

bool dup_command( )
{
    if( parameter_stack.size( ) == 0 ) {
        error_message( "Cannot dup an empty stack" );
        return false;
    }
    EditBuffer top_object;

    parameter_stack.pop ( top_object );
    parameter_stack.push( top_object );
    parameter_stack.push( top_object );
    return true;
}

bool xchg_command( )
{
    if( parameter_stack.size( ) < 2 ) {
        error_message( "Cannot exchange top stack levels; not enough data" );
        return false;
    }
    EditBuffer top_object_1;
    EditBuffer top_object_2;

    parameter_stack.pop ( top_object_1 );
    parameter_stack.pop ( top_object_2 );
    parameter_stack.push( top_object_1 );
    parameter_stack.push( top_object_2 );
    return true;
}


bool getch_command( )
{
    // Get a keystroke directly from the keyboard. Bypass the keyboard manager.
    int        key_code = scr::key( );
    EditBuffer temp;

    // If this is a printable letter, just push it onto the parameter stack.
    if( key_code < 128 && std::isprint( key_code ) ) {
        temp.append(static_cast< char>( key_code ) );
    }

    // Otherwise, I want to push the *name* of the key.
    else {
        temp.append( "*** UNKNOWN KEY ****" );
    }
    parameter_stack.push( temp );

    return true;
}
