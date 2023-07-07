/*! \file    y.cpp
 *  \brief   Main program of the Y editor.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "command.hpp"
#include "command_table.hpp"
#include "FileList.hpp"
#include "FileNameMatcher.hpp"
#include "global.hpp"
#include "MessageWindow.hpp"
#include "macro_stack.hpp"
#include "parameter_stack.hpp"
#include "support.hpp"
#include "YEditFile.hpp"
#include "yfile.hpp"

/*=================================*/
/*           Global Data           */
/*=================================*/

// The following defines the color of all the message windows.
const int W_attr = scr::REV_WHITE;

// Descriptors for message windows. See documentation for MessageWindow for details.
static scr::MessageWindowDescriptor my_mdescriptors[] = {
    { W_attr, scr::SINGLE_LINE, W_attr, NULL,         W_attr,  '\0' },
    { W_attr, scr::SINGLE_LINE, W_attr, NULL,         W_attr,  scr::MESSAGE_WINDOW_ANY },
    { W_attr, scr::SINGLE_LINE, W_attr, "Warning",    W_attr,  scr::K_ESC },
    { W_attr, scr::SINGLE_LINE, W_attr, "Sorry",      W_attr,  scr::K_ESC },
    { W_attr, scr::SINGLE_LINE, W_attr, "Bug Found!", W_attr | scr::BLINK, scr::K_ESC }
};

/*========================================*/
/*           Internal Functions           */
/*========================================*/

/*!
 * Search for and execute the startup macro. The command line has been placed onto the parameter
 * stack before this function is called. When this function returns, the material on the
 * parameter stack is processed as the command line. Thus a command line directive to put the
 * editor into restricted mode will not be in force during the starup macro.
 *
 * The parameter EXE_Directory is the name of the directory containing the executable. That
 * string is such that a name can be appended to it to form a valid pathname.
 */
static void execute_startup_macro( EditBuffer &EXE_directory )
{
    char            *file_name;
    FileNameMatcher  startup_file;

    startup_file.set_name( "ystart.ymy" );
  
    // First let's see if it's in the current directory (project specific).
    if( startup_file.next( ) != NULL ) {
        parameter_stack.push( "ystart.ymy" );
        execute_file_command( );
    }

    // Next, let's see if it's in the environment.
    else if( ( file_name = std::getenv( "YSTART" ) ) != NULL ) {
        parameter_stack.push( file_name );
        execute_file_command( );
    }

    // Finally, let's see if it's in the directory containing the executable.
    else {
        FileNameMatcher startup_file;

        EXE_directory.append( "ystart.ymy" );
        startup_file.set_name( EXE_directory.to_string( ).c_str( ) );

        if( startup_file.next( ) != NULL ) {
            parameter_stack.push( EXE_directory );
            execute_file_command( );
        }
    }
}


/*!
 * Force Y to load the named file (no wildcards), and position the cursor at the desired
 * location. This function is used by process_command_line().
 */
static bool load_file( const char *name, long line_number, int column_number )
{
    bool return_value;

    if( ( return_value = FileList::lookup( name ) ) == false ) {
        return_value = FileList::new_file( name );
    }

    if( return_value == true ) {
        if( line_number != -1L )
            FileList::active_file( ).CP( ).jump_to_line( line_number );
        if( column_number != -1L )
            FileList::active_file( ).CP( ).jump_to_column( column_number );
    }
  
    return return_value;
}


//! Read and process filelist.yfy.
static bool process_yfile( )
{
    EditBuffer active_name;

    // First, read the file into the descriptors array.
    read_yfile( );

    // Save filelist.yfy when doing external commands.
    yfile_flag = true;

    // Scan the descriptor list and load up the non deleted files.
    List<FileDescriptor>::Iterator stepper( descriptor_list );
    FileDescriptor *next;
    while( ( next = stepper( ) ) != NULL ) {

        // Only process files that are not "deleted."
        if( !next->is_deleted( ) ) {

            // Save the name of this file for later if this file is "active." This assumes that
            // there is one non-deleted active file.
            // 
            if( next->is_active( ) ) active_name = next->get_name( );

            // Get the file.
            parameter_stack.push( next->get_name( ) );
            find_file_command( );

            // The operation above removed a descriptor from the list. This contracted the list
            // by one position. I now have to back up my iterator by one to catch all the files.
            // This assumes that the list marking done by the List template is based on
            // indicies.
            // 
            descriptor_list.previous( );
        }
    }

    // Switch to the active file.
    FileList::lookup( active_name.to_string( ).c_str( ) );

    return true;
}


/*!
 * Process strings on the parameter_stack as if they were command line arguments. It is called
 * after the start up macro has executed and if there is a non-zero number of items left on the
 * parameter_stack.
 */
static bool process_command_line( )
{
    std::string base_name;          // Contains the base part of a saved name.
    bool        base_valid = false; // =true if base_name valid.
    bool        first_file = true;  // =false if at least one file loaded.
    YEditFile  *leading_file;       // Reference to the first file loaded.

    // Default value for initial position = no change.
    long line_number   = -1;
    int  column_number = -1;

    // For each parameter.
    while( parameter_stack.size( ) > 0 ) {

        // TODO: It seems like some checking of parameter syntax is needed here!
        EditBuffer workspace;
        parameter_stack.pop( workspace );
        std::string parameter = workspace.to_string( );

        // If we're looking at a switch (accept either '-' or '/')...
        if( parameter[0U] == '-'  ||  parameter[0U] == '/' ) {
            switch( parameter[1U] ) {

            // Set initial line number. Note that user's line number 1 based.
            case 'l':
            case 'L':
                line_number = std::atol( parameter.c_str( ) + 2 ) - 1L;
                break;

           // Set initial column number. Note that user's column number 1 based.
            case 'c':
            case 'C':
                column_number = std::atoi( parameter.c_str( ) + 2 ) - 1;
                break;
        
            // If the user sets restricted mode anywhere on the command line, set it globally.
            // The editor is either in restricted or it is not!
            // 
            case 'r':
            case 'R':
                restricted_mode = true;
                break;

            // Print message and wait for acknowledgment.
            default:
                warning_message( "Unrecognized switch (%c) ignored", workspace[1U] );
                break;

            } // End of switch statement which handles each switch type.
        }   // End of switch handling if...

        // Otherwise it was no switch, it must be a file name (or wildcard spec).
        else {
            FileNameMatcher wild_match;
            char *file_name;

            // If the current argument is an abbreviation, try expanding it. BUG: The filename
            // ..\foo.c will confuse this code. It will incorrectly think it is an abreviation.
            // 
            if( workspace[0U] == '.' && base_valid ) {
                EditBuffer temp( base_name.c_str( ) );
                temp.append( workspace );
                workspace = temp;
            }

            // Otherwise, it's a normal filename. Save the base part of the name. BUG: The
            // filename c:\sub.dir\foo will confuse this code. It will incorrectly think c:\sub.
            // is the basename.
            // 
            else {
                base_valid = true;
                base_name = workspace.to_string( );
                std::string::size_type dot_index = base_name.find( '.' );
                if( dot_index != std::string::npos ) {
                    base_name = base_name.substr( 0, dot_index );
                }
            }

            // strupr( workspace );
            std::string workspace_string = workspace.to_string( );
            wild_match.set_name( workspace_string.c_str( ) );

            // If no files match the spec, assume the spec is the name of a new file.
            if( ( file_name = wild_match.next( ) ) == NULL ) {

                // Tell the user if they tried a wildcard and there were no matches. DON'T take
                // the failed wildcard string as a filename!
                // 
                if( std::strchr( workspace_string.c_str( ), '*' ) != NULL ||
                    std::strchr( workspace_string.c_str( ), '?' ) != NULL ) {
                    warning_message( "No files match %s", workspace_string.c_str( ) );
                }
                else {

                    // Load the file.
                    load_file( workspace_string.c_str( ), line_number, column_number );

                    // Remember the first file so we can show that to the user when we're done.
                    if( first_file ) {
                        first_file = false;
                        leading_file = &FileList::active_file( );
                    }
                }
            }

            // It is a valid wildcard spec (there are matching files).
            else {

                // Loop over all names which match the wildcard spec and load them.
                while( file_name != NULL ) {

                    load_file( file_name, line_number, column_number );
                    
                    // Remember the first file so we can show that to the user when we're done.
                    if( first_file ) {
                        first_file = false;
                        leading_file = &FileList::active_file( );
                    }
                    file_name = wild_match.next( );
                }
            }
        } // End of if...else... that determines if we are looking at a switch.
    }   // End of loop that passes over all command line arguments.

    FileList::lookup( leading_file->name( ) );
    return true;
}


/*!
 * Initialize the editor by searching for and executing a startup macro, processing the command
 * line, etc. It is called before any normal, interactive editing else is done. Be aware that
 * global variables are initialized elsewhere before this function is called.
 */
static bool initialize( )
{
    scr::MessageWindow::set_descriptors( my_mdescriptors );

    // Move the cursor to the side so that screen is uncluttered.
    scr::set_cursor_position( scr::number_of_rows( ), 1 );

    // Let's see if there's a ystart.ymy file to process. We need to figure out the name of the
    // directory containing the executable. The name of the executable has been passed to us by
    // main() via the parameter stack.

    EditBuffer executable_name, executable_path;
    parameter_stack.pop( executable_name );

    // For now, just send execute_startup_macro( ) a null path.
    execute_startup_macro( executable_path );
  
    // If there is nothing on the stack, try to process a .yfy file; otherwise the command line.
    if( parameter_stack.size( ) == 0 ) {
        process_yfile( );
    }
    else {
        process_command_line( );
    }
    
    // Make sure the editor has at least one file loaded.
    if( FileList::count( ) == 0 ) {
        error_message( "No files loaded. Y requires at least one file" );
        return false;
    }

    return true;
}

/*==================================*/
/*           Main Program           */
/*==================================*/

/*!
 * The main loop of the program. After initializing the editor this function simply gets command
 * words and executes them repeatedly.
 */
int main( int argc, char *argv[] )
{
    // Perform program-wide (cross file) initializations.
    global_setup( );
    std::atexit( global_cleanup );

    EditBuffer word;

    // Push the command line onto the parameter stack.
    for( int i = argc; i > 0; i-- ) {
        parameter_stack.push( argv[i - 1] );
    }

    if( initialize( ) ) {
        while( 1 ) {
            get_word( word );
            if( word.length( ) != 0 ) handle_word( word );
        }
    }
    return 1;
}
