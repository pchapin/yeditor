/*! \file    check.cpp
    \brief   This file contains the test program for the window manager.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "CommandWindow.hpp"
#include "debug.hpp"
#include "ListWindow.hpp"
#include "Manager.hpp"
#include "Window.hpp"

using namespace std;
using namespace scr;

CommandWindow *create_command_window( Manager &test_manager )
{
    Tracer object( "create_command_window", 1 );

    CommandWindow *w = new CommandWindow( &test_manager, 10, 10, 40, 10 );
    w->set_prompt( "Command: ");
    return( w );
}

ListWindow *create_list_window( Manager &test_manager )
{
    Tracer object( "create_list_window", 1 );

    static const char *message[] = {
        "Hello, World!",
        "This is a List_Window",
        "Use it to display text messages.",
        "It acts like a FIFO queue of strings.",
        "Goodbye, World!",
        NULL
    };

    ListWindow *w = new ListWindow( &test_manager, 5, 15, 15, 40, 10 );
    for( const char **p = message; *p != NULL; ++p ) {
        w->insert( *p );
    }
    return( w );
}

/*==================================*/
/*           Main Program           */
/*==================================*/

int main( )
{
    initialize_debugging( );

    Manager test_manager;
    Window w1( &test_manager,  1,  1, 40, 10 );
    create_command_window( test_manager );
    create_list_window( test_manager );
    test_manager.input_loop( );

    terminate_debugging( );
    return( 0 );
}

