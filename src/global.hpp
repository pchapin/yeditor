/*! \file    global.hpp
 *  \brief   Declarations of global objects.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "parameter_stack.hpp"
#include "mystack.hpp"

extern bool yfile_flag;
  // =true if filelist.yfy is to be saved before external commands.

extern Parameter search_parameter;
extern Parameter replace_parameter;
  // The search and search & replace commands share parameters. They must be initialized in
  // global.cpp to insure correct ordering of global object construction. In particular, the
  // constructors of these objects require that scr::Initialize() has already been called.

extern bool  search_set;   // =true when search string is set.
extern bool  replace_set;  // =true when replace string is set.

extern int   box_size;     // The number of columns used for the input box.
extern int   start_row;    // The row number of the top row of the box.
extern int   start_column; // The col number of the left col of the box.

extern Stack< EditBuffer > parameter_stack;

extern bool restricted_mode;
  // =true when restricted mode is active. In restricted mode, the editor protects against the
  // destruction of files; only the file named on the command line can be overwritten.

struct ColorInfo {
    const char *color_name; // The name of the color as typed by the user.
    int   as_foreground;    // The value for a foreground color by that name.
    int   as_background;    // The value for a background color by that name.
};

// The following table is used by the commands that set colors to interprete the color words
// typed by the user.
//
extern ColorInfo colors[];

// The following functions deal with global initialization. They are called in main() to do
// cross-file initialization work.
//
extern void global_setup( );
extern void global_cleanup( );

#endif
