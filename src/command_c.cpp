/*! \file    command_c.cpp
 *  \brief   Implementation of the 'c' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "clipboard.hpp"
#include "FileList.hpp"
#include "yfile.hpp"

bool copy_block_command( )
{
    bool return_value;
    YEditFile &the_file = FileList::active_file( );

    clipboard.clear( );
    return_value = the_file.get_block( clipboard );

    // Turn off block mode if it's on.
    if( the_file.get_block_state( ) ) the_file.toggle_block( );

    return return_value;
}


bool CP_down_command( )
{
    FileList::active_file( ).CP( ).cursor_down( );
    return true;
}


bool CP_left_command( )
{
    FileList::active_file( ).CP( ).cursor_left( );
    return true;
}


bool CP_right_command()
{
    FileList::active_file( ).CP( ).cursor_right( );
    return true;
}


bool CP_up_command()
{
    FileList::active_file( ).CP( ).cursor_up( );
    return true;
}
