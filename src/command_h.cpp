/*! \file    command_h.cpp
 *  \brief   Implementation of the 'h' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "command.hpp"
#include "help.hpp"

bool help_command( )
{
    // Declare as static so Y remembers the last screen watched.
    static const HelpScreen *current = h_screens;

    current = display_screens( h_screens, current, 10 );
    return true;
}

