/*! \file    command_s.cpp
 *  \brief   Implementation of the 's' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "command.hpp"
#include "FileList.hpp"
#include "global.hpp"
#include "MessageWindow.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "YEditFile.hpp"


static void do_replacement(
    YEditFile &the_file, Parameter &search_parameter, Parameter &replace_parameter)
{
    std::string search_value = search_parameter.value( );
    std::string replace_value = replace_parameter.value( );
    unsigned i;

    for( i = 0; i < search_value.length( ); i++ ) {
        the_file.delete_char( );
    }
    for( i = 0; i < replace_value.length( ); i++ ) {
        the_file.insert_char( replace_value[i] );
        the_file.CP( ).cursor_right( );
    }
    return;
}


bool save_file_command( )
{
    bool return_value;
    YEditFile &the_file = FileList::active_file( );

    // If block mode is not on, just save the entire file under its normal name.
    if( !the_file.get_block_state( ) ) {
        return_value = the_file.save( the_file.name( ) );
        if( return_value == true ) {
            the_file.set_timestamp( the_file.name( ) );
            the_file.mark_as_unchanged( );
        }
    }

    // Otherwise prompt for a name and save the block.
    else {
        if( restricted_mode ) {
            error_message( "Can't save blocks in restricted mode" );
            return false;
        }

        static Parameter parameter( "SAVE BLOCK TO:" );

        if( parameter.get( ) == false ) return_value = false;
        else {
            std::string parameter_value = parameter.value( );
            return_value = the_file.save( parameter_value.c_str( ), DiskEditFile::BLOCK_ONLY );
            the_file.toggle_block( );
        }
    }
    return return_value;
}


bool search_and_replace_command( )
{
    bool return_value = true;
    bool block_was_on = false;
    YEditFile &the_file = FileList::active_file( );
    YEditFile::BlockInfo block_extent;

    // Get the search and replace strings.
    if( search_parameter.get( ) == false ) return false;
    std::string search_value = search_parameter.value( );
    search_set = true;

    if( replace_parameter.get( ) == false ) return false;
    std::string replace_value = replace_parameter.value( );
    replace_set = true;

    // Learn about the block limits selected, if any.
    long top_line = 0L;
    long bottom_line = LONG_MAX;

    if( the_file.get_block_state( ) ) {
        the_file.get_blockinfo(block_extent);
        block_was_on = true;

        the_file.block_limits( top_line, bottom_line );
        the_file.CP( ).jump_to_line( top_line );
        the_file.CP( ).jump_to_column( 0 );
        the_file.toggle_block( );
    }

    // Remember the current point so that it can be restored after the work.
    FilePosition old_CP = the_file.CP( );

    char buffer[80+1];
    bool stop          = false;   // =true when user asks to stop.
    bool dont_question = false;   // =true when user says to do all.
    bool done;                    // =true when no more instances found.
    bool wiggle;                  // =true when CP must be adjusted to skip.

    // See if there's a match in the range of lines of interest.
    done = static_cast< bool >( !the_file.simple_search( search_value.c_str( ) ) );
    if( the_file.CP( ).cursor_line( ) > bottom_line ) done = true;

    wiggle = true;
    while( !stop && !done ) {

        if( dont_question ) {
            do_replacement( the_file, search_parameter, replace_parameter );
            wiggle = false;
        }

        else {
            FilePosition point = the_file.CP( );

            // Show the user what we've got.
            the_file.display( );

            // Print the string into a holding buffer.
            std::sprintf( buffer,
                          "Replace with '%s'?  [y]/n/a", replace_value.c_str( ) );

            // Compute the desired line number of window's upper left corner.
            int box_line = static_cast< int >( ( point.cursor_line( ) - point.window_line( ) )  + 2 );
            box_line = ( box_line > scr::number_of_rows( ) - 5 ) ? box_line - 4 : box_line + 1;

            // Compute the desired column number of window's upper left corner.
            int box_column = point.cursor_column( ) - point.window_column( ) + 2;
            box_column =
                ( box_column + std::strlen( buffer ) + 6 > static_cast< std::size_t >( scr::number_of_columns( ) - 2 ) ) ?
                    scr::number_of_columns() - 2 - std::strlen(buffer) - 6 : box_column;

            scr::MessageWindow prompt;
            prompt.set( buffer, scr::MESSAGE_WINDOW_PROMPT );
            switch( prompt.open( box_line, box_column ) ) {
            case 'n':
            case 'N':
                break;

            case scr::K_ESC:
                stop = true;
                break;

            case 'a':
            case 'A':
                dont_question = true;
                // Fall through to do this replacement.

            default:
                do_replacement( the_file, search_parameter, replace_parameter );
                wiggle = false;

                // Show the user the effect while s/he waits for next instance.
                if( !dont_question ) the_file.display( );
                break;
            }
        }

        // Try to get to next instance.
        if( !stop ) {

            // Bump the CP if we didn't do a replacement to bypass the current instance.
            if( wiggle ) the_file.CP( ).cursor_right( );

            // Find the next instance.
            done = static_cast< bool >( !the_file.simple_search( search_value.c_str( ) ) );
            if( the_file.CP( ).cursor_line( ) > bottom_line ) done = true;

            // Fix the CP adjustment if we are done so it looks nice for the user.
            if( done && wiggle ) the_file.CP( ).cursor_left( );

            // Assume user won't want to replace.
            wiggle = true;
        }

    }
    if( !stop && !dont_question ) info_message( "Not found" );

    // Restore the old current point.
    the_file.CP( ) = old_CP;

    // Restore block information if necessary.
    if( block_was_on ) the_file.set_blockinfo( block_extent );

    return return_value;
}


bool search_first_command( )
{
    bool return_value = true;
    YEditFile &the_file = FileList::active_file( );

    if( search_parameter.get( ) == false ) return false;
    std::string search_value = search_parameter.value( );
    search_set = true;

    // Do the actual search.
    if( the_file.simple_search( search_value.c_str( ) ) == false ) {
        info_message( "Not found" );
        return_value = false;
    }
    return return_value;
}


bool search_next_command( )
{
    bool return_value = true;
    YEditFile &the_file = FileList::active_file( );

    if( search_set == false ) {
        error_message( "No search string set" );
        return_value = false;
    }
    else {
        std::string search_value = search_parameter.value( );
        if( the_file.CP( ).cursor_right( ), the_file.simple_search( search_value.c_str( ) ) == false ) {
            the_file.CP( ).cursor_left( );
            info_message( "Not found" );
            return_value = false;
        }
    }
    return return_value;
}


bool set_bookmark_command( )
{
    FileList::set_bookmark( );
    info_message( "Bookmark set here" );
    return true;
}


bool set_tab_command( )
{
    static Parameter parameter( "NEW TAB DISTANCE:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    YEditFile &the_file = FileList::active_file( );
    the_file.set_tab( std::atoi( parameter_value.c_str( ) ) );
    return true;
}


bool skip_left_command( )
{
    YEditFile &the_file = FileList::active_file( );  // Assume active file does not change.
    int old_column       = the_file.CP( ).cursor_column( );  // Original cursor column.
    int column           = old_column;                       // New cursor column position.
    std::size_t length   = the_file.CP_line_length( );

    // If cursor is at or off the end of the line, just advance to end.
    if( static_cast< std::size_t >( column ) > length ) {
        the_file.end( );
    }
    else {
        column = word_left( *the_file.get_line( ), column );

        // Position cursor.
        the_file.CP( ).cursor_left( old_column - column );
    }
    return true;
}


bool skip_right_command( )
{
    YEditFile &the_file = FileList::active_file( );     // Assume active file does not change.
    int old_column       = the_file.CP( ).cursor_column( );  // Original cursor column position.
    int column           = old_column;                  // New column position.
    std::size_t length   = the_file.CP_line_length( );  // Length of line.

    // Do nothing if cursor off end of line.
    if( static_cast< std::size_t >( column ) < length ) {
        column = word_right( *the_file.get_line( ), column );

        // Position the cursor on the next word.
        the_file.CP( ).cursor_right( column - old_column );
    }
    return true;
}
