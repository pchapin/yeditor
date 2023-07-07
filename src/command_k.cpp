/*! \file    command_k.cpp
 *  \brief   Implementation of the 'k' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "command.hpp"
#include "FileList.hpp"
#include "support.hpp"
#include "YEditFile.hpp"

bool kill_file_command( )
{
    YEditFile &current = FileList::active_file( );

    // If block mode is on, trash the block.
    if( current.get_block_state( ) ) {
        current.delete_block( );
        current.toggle_block( );
        return true;
    }

    // Otherwise see if this is the only file or not.
    if( FileList::count( ) == 1 ) {
        error_message( "Can't remove. This is the only file loaded" );
        return false;
    }

    // Make sure user knows if they are going to throw something out.
    if( current.changed( ) ) {
        if( confirm_message( "Changes will be lost. Continue? [y]/n", 'N', false ) == false )
            return false;
    }

    // Do the actual destruction.
    FileList::kill( );
    return true;
}
