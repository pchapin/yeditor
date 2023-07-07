/*! \file    command_y.cpp
 *  \brief   Implementation of the 'y' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "command.hpp"
#include "FileList.hpp"
#include "yfile.hpp"

bool yexit_command( )
{
    write_yfile( );
    if( FileList::save_changes( ) == true ) std::exit( 0 );
    return false;
}
