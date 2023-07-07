/*! \file    command_p.cpp
 *  \brief   Implementation of the 'p' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "clipboard.hpp"
#include "command.hpp"
#include "FileList.hpp"
#include "YEditFile.hpp"

bool page_down_command( )
{
    FileList::active_file( ).CP( ).page_down( );
    return true;
}


bool page_up_command( )
{
    FileList::active_file( ).CP( ).page_up( );
    return true;
}


bool pan_left_command( )
{
    FileList::active_file( ).CP( ).pan_left( 8 );
    return true;
}


bool pan_right_command( )
{
    FileList::active_file( ).CP( ).pan_right( 8 );
    return true;
}


bool paste_block_command( )
{
    YEditFile &The_File = FileList::active_file( );

    if( The_File.get_block_state( ) ) {
        The_File.delete_block( );
        The_File.toggle_block( );
    }
    return The_File.insert_block( clipboard );
}


bool previous_file_command( )
{
    FileList::previous( );
    return true;
}


bool previous_procedure_command( )
{
    return FileList::active_file( ).previous_procedure( );
}
