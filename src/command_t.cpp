/*! \file    command_t.cpp
 *  \brief   Implementation of the 't' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "command.hpp"
#include "FileList.hpp"
#include "YEditFile.hpp"

bool tab_command( )
{
    bool        return_value = true;
    YEditFile &the_file     = FileList::active_file( );
    int         column       = the_file.CP( ).cursor_column( );
    int         insertions   = the_file.tab_distance( ) - ( column % the_file.tab_distance( ) );

    for( int i = 0; i < insertions && return_value == true; i++ ) {
        if( the_file.insert_mode( ) == YEditFile::INSERT )
            return_value = the_file.insert_char( ' ' );
        if( return_value == true ) the_file.CP( ).cursor_right( );
    }

    return return_value;
}


bool toggle_block_command( )
{
    FileList::active_file( ).toggle_block( );
    return true;
}


bool toggle_bookmark_command( )
{
    FileList::toggle_bookmark( );
    return true;
}
