/*! \file    command_f.cpp
 *  \brief   Implementation of the 'f' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <cstring>
#include <string>

#include "command.hpp"
#include "EditFile.hpp"
#include "FileList.hpp"
#include "global.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "YEditFile.hpp"
#include "yfile.hpp"

bool filelist_info_command( )
{
    error_message( "Not implemented" );
    return false;
}


bool file_info_command( )
{
    error_message( "Not implemented" );
    return false;
}


bool file_insert_command( )
{
    if( restricted_mode ) {
        error_message( "Can't create new files in restricted mode" );
        return false;
    }

    static Parameter parameter( "INSERT INTO:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    return FileList::insert_active( parameter_value.c_str( ) );
  }


bool filter_command( )
{
    if( restricted_mode ) {
        error_message( "Can't run external commands in restricted mode" );
        return false;
    }

    bool return_value = true;
    YEditFile &the_file = FileList::active_file( );

    static Parameter parameter( "FILTER COMMAND:" );
    if( parameter.get( ) == false ) return false;
    std::string command = parameter.value( );

    // Prepare command.
    insert_AWK( command );
    command += " <STDIN$.TMP >STDOUT$.TMP";

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

    // Scr...() functions OFF. All I/O done with standard functions.
    scr::off( );

    // Execute command and ask for user confirmation. Ch holds the user's response.
    int ch;

    std::printf( "%s\n", command.c_str( ) );
    int exit_status = std::system( command.c_str( ) );
    std::printf( "\n********************************* " );
    std::printf( "\n Command exited with status: %d", exit_status );
    std::printf( "\n***** Do Replacement? [y]/n ***** " );

    // Get the user's response and eat the rest of the line.
    ch = std::getchar( );
    if( ch != '\n' ) {
        while( std::getchar( ) != '\n' ) /* Null */ ;
    }

    // Scr...() functions ON. All I/O done with the Scr package.
    scr::on( );

    scr::clear_screen( );
    FileList::reload_files( );

    // Perform the replacement.
    if( ch != 'n'  &&  ch != 'N' ) {
        if( the_file.get_block_state( ) ) {
            the_file.delete_block( );
            the_file.toggle_block( );
        }
        else {
            the_file.top_of_file( );
            the_file.toggle_block( );
            the_file.bottom_of_file( );
            the_file.delete_block( );
            the_file.toggle_block( );
            the_file.top_of_file( );
        }
        return_value = the_file.load( "STDOUT$.TMP" );
    }

    // Trash the temporary files.
    std::remove( "STDIN$.TMP" );
    std::remove( "STDOUT$.TMP" );

    // Update display.
    the_file.display( );
    return return_value;
}


bool find_file_command( )
{
    if( restricted_mode ) {
        error_message( "Can't load additional files in restricted mode" );
        return false;
    }

    // TODO: Rewrite this code to use a std::vector<std::string>.

    const int WORDS = 10;
    const char *argv[WORDS + 2];

    // Get the name of the file.
    static Parameter parameter( "FILE TO EDIT:" );
    if( parameter.get( ) == false ) return false;
    std::string workspace( parameter.value( ) );

    // Break down the command string into individual tokens.
    // Casting away const here is gross. When the TODO above is handled this will be fixed.
    argv[1] = std::strtok( const_cast< char * >(workspace.c_str( )), " " );
    int i;
    for( i = 2; i < WORDS + 1 && argv[i - 1] != NULL; i++ ) {
        argv[i] = std::strtok( NULL, " " );
    }
    argv[i] = NULL;

    return load_files( argv );
}


bool foreground_color_command( )
{
    YEditFile &the_file = FileList::active_file( );

    static Parameter parameter( "FOREGROUND COLOR:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    // For foreground colors, we need to check for the special word "bright."
    bool bright_flag = false;
    const char *color_word  = parameter_value.c_str( );

    if( my_strnicmp( color_word, "bright", 6 ) == 0 ) {

        // We found it. Skip after and look for a space. If there is a space, skip it and go on.
        // Otherwise they the user just typed "bright."
        //
        bright_flag = true;
        color_word += 6;
        if( *color_word == ' ' ) color_word++;
        else {
            error_message( "Can't specify BRIGHT alone" );
            return false;
        }
    }

    // Search the colors array looking for a match.
    ColorInfo *current = colors;
    while( current->color_name != NULL ) {
        if( my_stricmp( current->color_name, color_word ) == 0 ) break;
        current++;
    }

    // Let's see if this color is legal.
    if( current->color_name == NULL ) {
        error_message( "Unknown Color: %s", color_word );
        return false;
    }

    // Get the current color and adjust the foreground. THIS NEEDS TO BE IMPROVED. IT DEPENDS
    // TOO MUCH ON THE LAYOUT OF THE int USED TO REPRESENT COLOR VALUES!!
    //
    int current_color = the_file.color_attribute( );
    the_file.set_color( ( current_color & 0xF0 ) | ( current->as_foreground ) );

    // Now handle the bright flag properly.
    current_color  = the_file.color_attribute( );
    current_color &= ~scr::BRIGHT;
    if( bright_flag ) current_color |= scr::BRIGHT;
    the_file.set_color( current_color );
    return true;
  }
