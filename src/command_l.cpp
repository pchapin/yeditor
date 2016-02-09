/*! \file    command_l.cpp
    \brief   Implementation of the 'l' command functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "command.hpp"
#include "help.hpp"

bool legal_info_command( )
{
    // Declare as static so Y remembers the last screen watched.
    static const HelpScreen *current = l_screens;

    current = display_screens( l_screens, current, 2 );
    return true;
}
