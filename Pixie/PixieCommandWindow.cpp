/*! \file    pixie_commandw.h
    \brief   Implementation of the PixieCommandWindow class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "debug.hpp"
#include "PixieCommandWindow.hpp"
#include "TaskWindow.hpp"

using namespace std;

static bool split_string(
    char *buffer,            // String to be split.
    const char *delimiters,  // String of delimiter characters.
    char *strings[],         // Array to be filled with pointers to substrings.
    int   max_strings,       // Maximum number of substrings allowed.
    int  *actual_strings     // Actual number of substrings found.
    )
{
    char *buffer_pointer;         // Temporary pointer into buffer.
    bool  in_string = false;      // =false when *buffer_pointer not pointing at string.
    bool  return_code = true;     // Returns false if too many substrings.

    // Initialize result and check for out of bounds input.
    *actual_strings = 0;
    if( max_strings < 0 ) return_code = false;
    else {

        // Sweep down string until null byte reached.
        buffer_pointer = buffer;
        while( *buffer_pointer != '\0' ) {

            // Try to process the current character only if no error so far.
            if( return_code == true ) {

                // If delimiter found while scanning an argument...
                if( ( strchr( delimiters, *buffer_pointer) != 0 ) && ( in_string == true ) ) {
                    *buffer_pointer = '\0';
                    in_string = false;
                }

                // If non delimiter found while scanning "white" space...
                else if( ( strchr( delimiters, *buffer_pointer) == 0 ) && ( in_string == false ) ) {

                    // Install only if space. If no space left set error flag.
                    if( *actual_strings < max_strings ) {
                        strings[*actual_strings] = buffer_pointer;
                        ( *actual_strings )++;
                        in_string = true;
                    }
                    else return_code = false;
                }
            }
            buffer_pointer++;
        } // End of while loop.
    }     // End of overall if... else...

    return return_code;
}


bool PixieCommandWindow::process_keystroke( int &key_code )
{
    scr::Tracer( "PixieCommandWindow::process_keystroke", 4 );
    char *command_parts[3];
    int   argument_count;

    if( key_code != scr::K_RETURN )
        return scr::CommandWindow::process_keystroke( key_code );

    auto_ptr< char > workspace( new char[command_text.size( ) + 1] );
    strcpy( workspace.get( ), command_text.c_str( ) );
    split_string( workspace.get( ), " ", command_parts, 3, &argument_count );

    if( argument_count == 0 ) return true;

    if( string( command_parts[0] ) == "quit" ) {
        key_code = scr::K_ESC;
        return false;
    }
    else if( string( command_parts[0] ) == "add" ) {
        if( argument_count == 3 ) {
            main_window->add_minutes( atoi( command_parts[1] ), atoi( command_parts[2] ) );
        }
    }
    else if( string( command_parts[0] ) == "create" ) {
        // This is kind of hacked.
        string::size_type index = command_text.find_first_of( ' ' );
        if( index != string::npos ) {
            main_window->create_task( command_text.substr( index + 1 ), 50 );
        }
    }
    else if( string( command_parts[0] ) == "daily" ) {
        if( argument_count == 3 ) {
            main_window->change_daily( atoi( command_parts[1] ), atoi( command_parts[2] ) );
        }
    }
    else if( string( command_parts[0] ) == "delete" ) {
        if( argument_count == 2 ) {
            main_window->delete_task( atoi( command_parts[1] ) );
        }
    }
    else if( string( command_parts[0] ) == "priority" ) {
        if( argument_count == 3 ) {
            main_window->change_priority( atoi( command_parts[1] ), atoi( command_parts[2] ) );
        }
    }
    else if( string( command_parts[0] ) == "rename" ) {
        // This is kind of hacked.
        string::size_type index = command_text.find_first_of( ' ' );
        index = command_text.find_first_of( ' ', index + 1);
        if( index != string::npos ) {
            main_window->rename( atoi( command_parts[1] ), command_text.substr( index + 1 ) );
        }
    }
    else if( string( command_parts[0] ) == "save" ) {
        main_window->save_tasks( );
    }
    else if( string( command_parts[0] ) == "start" ) {
        if( argument_count == 2 ) {
            main_window->start_task( atoi( command_parts[1] ) );
        }
    }
    else if( string( command_parts[0] ) == "stop" ) {
        main_window->stop_tasks( );
    }
    else if( string( command_parts[0] ) == "undo_daily") {
        main_window->undo_daily( );
    }
    else if( string( command_parts[0] ) == "zero" ) {
        main_window->zero_tasks( );
    }

    command_text.clear( );
    cursor_offset = 0;
    return true;
}
