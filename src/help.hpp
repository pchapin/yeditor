/*! \file    help.hpp
 *  \brief   Declarations of the help text arrays.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the declarations of the help screens. These screens are located in
 * help.cpp, but are used in one of the command files (where the command associated with the
 * HELP macro is implemented).
*/

#ifndef HELP_HPP
#define HELP_HPP

struct HelpScreen {
  const char *const *current_screen;
  const HelpScreen *next_screen;
  const HelpScreen *previous_screen;
  };

extern const HelpScreen h_screens[];   // Help screens.
extern const HelpScreen e_screens[];   // Editor information screens.
extern const HelpScreen l_screens[];   // Legal information screens.

//! Displays a set of help screens.
const HelpScreen *display_screens( const HelpScreen *base, const HelpScreen *current, int size );

#endif

