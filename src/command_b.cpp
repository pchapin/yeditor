/*! \file    command_b.cpp
 *  \brief   Implementation of the 'b' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <cstring>

#include "FileList.hpp"
#include "global.hpp"
#include "parameter_stack.hpp"
#include "support.hpp"
#include "YEditFile.hpp"

bool background_color_command( )
{
    YEditFile &the_file = FileList::active_file( );

    static Parameter parameter( "BACKGROUND COLOR:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    // Search the Colors array looking for a match.
    ColorInfo *current = colors;
    while( current->color_name != NULL ) {
        if( my_stricmp( current->color_name, parameter_value.c_str( ) ) == 0 ) break;
        current++;
    }

    // Let's see if this color is legal.
    if( current->color_name == NULL ) {
        error_message( "Unknown Color: %s", parameter_value.c_str( ) );
        return false;
    }

    // Get the current color and adjust the foreground. THIS NEEDS TO BE IMPROVED. IT DEPENDS
    // TOO MUCH ON THE LAYOUT OF THE int USED TO REPRESENT COLOR VALUES!!
    //
    int current_color = the_file.color_attribute( );
    the_file.set_color( ( current_color & 0x0F ) | ( current->as_background ) );
    return true;
}


bool backspace_command( )
{
    YEditFile &the_file = FileList::active_file();
    bool        return_value = true;
    unsigned    offset;

    // Is this a line join operation?
    if( the_file.CP( ).cursor_column( ) == 0   &&
        the_file.CP( ).cursor_line( )   != 0L  &&
       !the_file.get_block_state( ) ) {

        // If so, be sure to position the cursor at the join point.
        the_file.CP( ).cursor_up( );
        offset = the_file.CP_line_length( );
        if( the_file.insert_mode( ) == YEditFile::INSERT ) {
            the_file.CP( ).cursor_down( );
            return_value = the_file.backspace( );
            the_file.CP( ).cursor_up( );
        }
        the_file.CP( ).jump_to_column( offset );
    }

    // It's not a join operation.
    else {
        if( the_file.insert_mode( ) == YEditFile::INSERT )
            return_value = the_file.backspace( );

        if( return_value == true ) the_file.CP( ).cursor_left( );
    }

    return return_value;
}


bool block_off_command( )
{
    YEditFile &the_file = FileList::active_file( );

    the_file.set_block_state( false );
    return true;
}
