/*! \file    command_q.cpp
    \brief   Implementation of the 'q' command functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstdlib>

#include "command.hpp"
#include "FileList.hpp"
#include "support.hpp"

bool quit_command( )
{
    if( FileList::no_changes( ) ) std::exit( 0 );
    else {
        // Be sure the user knows what he/she is doing.
        if( confirm_message( "At least one file changed. Quit? y/[n]", 'Y', true ) == false )
            std::exit( 0 );
    }

    return true;
}
