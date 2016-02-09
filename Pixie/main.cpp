/*! \file    main.cpp
    \brief   Entry point for the Pixie program.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

LICENSE

This program is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if
not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA

Please send comments or bug reports pertaining to this file to

     Pixie
     c/o Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "debug.hpp"
#include "Manager.hpp"
#include "PixieCommandWindow.hpp"
#include "TaskWindow.hpp"

using namespace std;

//! Class to manage run time debug initialization.
/*!
 * This class allows one to activate run time debugging support in a selective region of the
 * program. Its constructor initializes debugging and its destructor automatically terminates
 * debugging. Thus this class provides RAI handling of the debugging system.
 */
class ScrDebugging_Helper {
private:
    bool debugging_active;

public:
    //! Initialize the debugging system if debug_flag is true.
    ScrDebugging_Helper( bool debug_flag ) : debugging_active( debug_flag )
        { if( debugging_active ) scr::initialize_debugging( ); }

    //! Terminate the debugging system if it was initialized by the constructor.
   ~ScrDebugging_Helper()
        { if( debugging_active ) scr::terminate_debugging( ); }
};


//! Pixie entry point.
int main( int argc, char **argv )
{
    int rc = EXIT_SUCCESS;
    bool debug_flag = false;  // Set to true to use the run time debugging system.

    // Analyze command line.
    if( argc > 1 ) {
        if( strcmp( argv[1], "-d") == 0 ) debug_flag = true;
    }

    try {
        ScrDebugging_Helper scr_helper(debug_flag);
        scr::Manager window_manager;
        int max_width = scr::number_of_columns( ) - 2;
        TaskWindow task_window( &window_manager, 2, 2, max_width, scr::number_of_rows( ) - 5 );
        PixieCommandWindow command_window(
            &window_manager, scr::number_of_rows( ) - 1, 2, max_width, 1, &task_window );
        command_window.set_prompt( "> " );

        window_manager.input_loop( );
    }
    catch( exception &e ) {
        cout << "Pixie: Unhandled exception reached main!\n"
             << "       (" << e.what( ) << ")\n";
        rc = EXIT_FAILURE;
    }
    catch( ... ) {
        cout << "Pixie: Unknown, unhandled exception reached main!\n";
        rc = EXIT_FAILURE;
    }
    return rc;
}
