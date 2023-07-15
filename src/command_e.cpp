/*! \file    command_e.cpp
 *  \brief   Implementation of the 'e' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdio>
#include <cstdlib>
#include <string>

#include "command.hpp"
#include "FileList.hpp"
#include "global.hpp"
#include "help.hpp"
#include "macro_stack.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "yfile.hpp"

bool editor_info_command( )
{
    display_screens( e_screens, e_screens, 1 );
    return true;
}


bool error_message_command( )
{
    static Parameter parameter( "MESSAGE TEXT:" );

    if( parameter.get( ) == false ) return false;
    error_message( "%s", parameter.value( ).c_str( ) );
    return true;
}


bool execute_file_command( )
{
    if( restricted_mode ) {
        error_message( "Can't execute macro files in restricted mode" );
        return false;
    }
  
    static Parameter parameter( "MACRO FILE:" );

    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );
    FileList::save_changes( );
    // Do the macro file only if the above works? What about filelist.yfy?

    start_macro_file( parameter_value.c_str( ) );
    return true;
}


bool execute_macro_command( )
{
    if( restricted_mode ) {
        error_message( "Can't execute explicit macro text in restricted mode" );
        return false;
    }

    static Parameter parameter( "MACRO TEXT:" );

    if ( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );
    start_macro_string( parameter_value.c_str( ) );
    return true;
}


bool exit_command( )
{
    if( FileList::save_changes( ) == true ) std::exit( 0 );
    return false;
}


bool external_command_command( )
{
    if( restricted_mode ) {
        error_message( "Can't run external commands in restricted mode" );
        return false;
    }

    static Parameter parameter( "COMMAND:" );
    if( parameter.get( ) == false ) return false;
    std::string command = parameter.value( );

    // Prepare command.
    insert_AWK( command );

    // Do the extern command.
    if( yfile_flag ) write_yfile( );
    FileList::save_changes( );
    scr::clear_screen( );

    // Scr...() functions OFF. All I/O with standard functions!
    scr::off( );

    std::printf( "%s\n", command.c_str( ) );
    int exit_status = std::system( command.c_str( ) );
    std::printf( "\n*************************************** " );
    std::printf( "\n Command exited with status: %d", exit_status );
    std::printf( "\n***** Strike ENTER to return to Y ***** " );
    while( std::getchar( ) != '\n' ) /* Null */ ;

    // Scr...() functions ON. All I/O with Scr... library.
    scr::on( );

    scr::clear_screen( );
    FileList::reload_files( );
    FileList::active_file( ).display( );

    return true;
}
