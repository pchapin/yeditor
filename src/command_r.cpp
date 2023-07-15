/*! \file    command_r.cpp
 *  \brief   Implementation of the 'r' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "command.hpp"
#include "FileList.hpp"
#include "global.hpp"
#include "help.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "yfile.hpp"

bool redirect_from_command( )
{
    if( restricted_mode ) {
        error_message( "Can't run external commands in restricted mode" );
        return false;
    }

    bool return_value = true;
    YEditFile &the_file = FileList::active_file( );

    static Parameter parameter( "REDIRECT FROM:" );
    if( parameter.get( ) == false ) return false;
    std::string command = parameter.value( );

    // Prepare command.
    insert_AWK( command );
    command += " >STDOUT$.TMP";

    // Save all information to disk.
    if( yfile_flag ) write_yfile( );
    FileList::save_changes( );

    // Prepare display.
    scr::clear_screen( );

    // Scr...() functions OFF. All I/O done with standard functions.
    scr::off( );

    // Execute command and ask for user confirmation.
    int ch;

    std::printf( "%s\n", command.c_str( ) );
    int exit_status = std::system( command.c_str( ) );
    std::printf( "\n******************************* " );
    std::printf( "\n Command exited with status: %d", exit_status );
    std::printf( "\n***** Do Insertion? [y]/n ***** " );

    // Get the user's response and eat the rest of the line.
    ch = std::getchar( );
    if( ch != '\n' ) {
        while( std::getchar( ) != '\n' ) /* Null */ ;
    }

    // Scr...( ) functions ON. All I/O done with Scr package.
    scr::on( );

    scr::clear_screen( );
    FileList::reload_files( );

    // Perform the replacement.
    if( ch != 'n'  &&  ch != 'N' ) {
        if( the_file.get_block_state( ) ) {
            the_file.delete_block( );
            the_file.toggle_block( );
        }
        return_value = the_file.load( "STDOUT$.TMP" );
        if( return_value == true ) the_file.mark_as_changed( );
    }

    // Trash the temporary files.
    std::remove( "STDOUT$.TMP" );

    // Update display.
    the_file.display( );
    return return_value;
}


bool redirect_to_command( )
{
    if( restricted_mode ) {
        error_message( "Can't run external commands in restricted mode" );
        return false;
    }

    YEditFile &the_file = FileList::active_file( );

    // Return with error indicator if user ESCapes out.
    static Parameter parameter( "REDIRECT TO:" );
    if( parameter.get( ) == false ) return false;
    std::string command = parameter.value( );

    // Prepare command.
    insert_AWK( command );
    command += " <STDIN$.TMP";

    // Save all information to disk. Abort if STDIN$.TMP cannot be created.
    if( yfile_flag ) write_yfile( );
    FileList::save_changes( );
    if( the_file.get_block_state( ) ) {
        if( the_file.save( "STDIN$.TMP", YEditFile::BLOCK_ONLY ) == false )
            return false;
    }
    else {
        if( the_file.save( "STDIN$.TMP" ) == false )
            return false;
    }

    // Prepare display.
    scr::clear_screen( );

    // Scr...( ) functions OFF. All I/O done with standard functions.
    scr::off( );

    // Execute command.
    std::printf( "%s\n", command.c_str( ) );
    int exit_status = std::system( command.c_str( ) );
    std::printf( "\n*************************************** " );
    std::printf( "\n Command exited with status: %d", exit_status );
    std::printf( "\n***** Strike ENTER to return to Y ***** " );
    while( std::getchar( ) != '\n' ) /* Null */ ;

    // Scr...( ) functions ON. All I/O done with the Scr package.
    scr::on( );

    scr::clear_screen( );
    FileList::reload_files( );

    // Trash the temporary file.
    std::remove( "STDIN$.TMP" );

    // Update display.
    FileList::active_file( ).display( );
    return true;
}


bool reformat_command( )
{
    YEditFile &the_file = FileList::active_file( );
    return the_file.reformat_paragraph( );
}


bool refresh_file_command( )
{
    bool return_value;
    YEditFile &the_file = FileList::active_file( );
    FilePosition old_CP = the_file.CP( );

    if( the_file.changed( ) ) {
        if( confirm_message( "Changes will be lost. Continue? [y]/n", 'N', false ) == false )
            return false;
    }

    // Make sure block mode is off.
    the_file.set_block_state( false );

    // Erase entire file.
    the_file.top_of_file( );
    the_file.toggle_block( );
    the_file.bottom_of_file( );
    the_file.delete_block( );
    the_file.toggle_block( );

    // Position cursor at the top of the file.
    the_file.top_of_file( );

    // Load the file.
    return_value = the_file.load( the_file.name( ) );
    the_file.set_timestamp( the_file.name( ) );
    the_file.mark_as_unchanged( );

    // Be sure cursor is positioned correctly.
    the_file.CP( ) = old_CP;

    return return_value;
}


bool remove_file_command( )
{
    YEditFile &current = FileList::active_file( );

    if( FileList::count( ) == 1 ) {
        error_message( "Can't remove. This is the only file loaded" );
        return false;
    }

    // Do the actual destruction if there is no problem with saving.
    if( ( current.changed( ) ) ? current.save( current.name( ) ) : true ) {
        FileList::kill( );
    }

    return true;
}


bool rename_file_command( )
{
    if( restricted_mode ) {
        error_message( "Can't rename files in restricted mode" );
        return false;
    }

    bool return_value;
    YEditFile &the_file = FileList::active_file( );
    FilePosition old_CP = the_file.CP( );

    // Point at the name field of the current file.
    const char *old_name = the_file.name( );

    // Get a new name.
    static Parameter parameter( "NEW NAME:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    // Insert the new name as the destination file for a File_Insert.
    parameter_stack.push( parameter_value.c_str( ) );
    return_value = file_insert_command( );

    // If the insertion worked, go back to the orignal file and trash it.
    if( return_value == true ) {
        FileList::lookup( old_name );
        FileList::kill( );
        FileList::lookup( parameter_value.c_str( ) );
    }

    // Restore the original current point.
    FileList::active_file( ).CP( ) = old_CP;

    return return_value;
}


bool restricted_mode_command( )
{
    bool return_value = true;

    // Return with error indicator if user ESCapes out.
    static Parameter parameter( "RESTRICTED MODE:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    if( my_stricmp( "ON", parameter_value.c_str( ) ) == 0 ) {
        restricted_mode = true;
        info_message( "Restricted Mode is ON" );
    }
    else if( my_stricmp( "OFF", parameter_value.c_str( ) ) == 0 ) {
        restricted_mode = false;
        info_message( "Restricted Mode is OFF" );
    }
    else {
        error_message( "Use ON/OFF to adjust restricted mode" );
        return_value = false;
    }

    return return_value;
}
