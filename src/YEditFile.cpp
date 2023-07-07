/*! \file    YEditFile.cpp
 *  \brief   Implementation of class YEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the implementation of class YEditFile. Objects of this class are EditFiles
 * that have been elaborated using inheritance to have all the features required by Y. As most
 * of what YEditFiles do is implemented in its various base classes, this file is concerned
 * mostly with the display function.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "EditBuffer.hpp"
#include "FileList.hpp"
#include "scr.hpp"
#include "scrtools.hpp"
#include "support.hpp"
#include "YEditFile.hpp"
#include "yfile.hpp"

/*=============================================*/
/*           Public Member Functions           */
/*=============================================*/

/*!
 * This function constructs the generic YEditFile. It sets some attributes and sets up the
 * screen image so that the first call to Display() will show a border and name. Finally, it
 * loads the file for the first time.
 */
YEditFile::YEditFile( const char *name_of_file, int tab_distance, int file_color ) :
    CharacterEditFile( tab_distance ),
    file_name        ( name_of_file ),
    color            ( file_color )
{
    // Adjust the screen color if a monochrome screen is in use.
    if( scr::is_monochrome( ) ) color = scr::BRIGHT|scr::WHITE|scr::REV_BLACK;

    // Load the file if it file exists. If does not exist, just stay blank.
    std::FILE *file_to_edit;
    if( ( file_to_edit = std::fopen( file_name.c_str( ), "r" ) ) != NULL ) {
        std::fclose( file_to_edit );
        load( file_name.c_str( ) );           // Read the file.
        set_timestamp( file_name.c_str( ) );  // Read the time stamp for the first load.
    }

    // File has not yet changed. (Note: new files will not save unless something is entered into
    // them).
    //
    is_changed = false;
}


/*!
 * A virtual destructor is required so that classes derived from YEditFile are properly
 * destroyed when a YEditFile* is deleted.
 */
YEditFile::~YEditFile( )
{
    return;
}


//! This function allows clients to set the screen color anyway they want.
void YEditFile::set_color( int new_column )
{
    // Adjust the screen color if a monochrome screen is in use.
    if( scr::is_monochrome( ) ) color = scr::BRIGHT|scr::WHITE|scr::REV_BLACK;
    else color = new_column;
}


/*!
 * This function allows clients to set "all" the attributes of a file from an entry in the file
 * descriptor list. If 'this' file is not in the descriptor list, this function does nothing.
 * After the attributes are set, this function will remove the descriptor from the list. Files
 * that are actually loaded in the editor (as opposed to just being deleted) are are mentioned
 * in the descriptor list.
 */
void YEditFile::set_attributes( )
{
    // First, search the file descriptor list for 'this' file.
    List< FileDescriptor >::Iterator stepper( descriptor_list );
    FileDescriptor *next;
    while( ( next = stepper( ) ) != NULL ) {

        // If we've found it...
        if( my_stricmp( file_name.c_str( ), next->name.to_string( ).c_str( ) ) == 0 ) {

            // Set all the attributes.
            block  = static_cast< bool >( next->block_flag );
            anchor = next->block_line;
            set_color( next->color_attribute );
            CP( ).jump_to_column( next->cursor_column );
            CP( ).jump_to_line( next->cursor_line );
            set_insert( ( next->insert_flag == true ) ? INSERT : REPLACE );
            set_tab( next->tab_setting );
            CP( ).adjust_window_line( int(next->cursor_line - next->window_line) );
            CP( ).adjust_window_column( int(next->cursor_column - next->window_column) );

            // Now delete the descriptor from the list. During normal operation, the descriptor
            // list only stores information on files that are deleted. Don't loop back!
            //
            descriptor_list.previous( );
            descriptor_list.erase( );
            break;
        }
    }
}


/*!
 * This function loads up a File_Descriptor object with information from 'this' YEditFile. It's
 * used to find out what needs to be saved in the yfile. Since a YEditFile cannot tell if it's
 * active (only the file list knows that), this function assumes 'true.' If this is not the
 * case, the calling program will have to use Make_Inactive in class File_Descriptor to change
 * it later.
 */
void YEditFile::set_descriptor(FileDescriptor &new_descriptor)
{
    new_descriptor.active_flag     = true;
    new_descriptor.block_flag      = ( block == true ) ? true : false;
    new_descriptor.block_line      = anchor;
    new_descriptor.color_attribute = color;
    new_descriptor.cursor_column   = CP( ).cursor_column( );
    new_descriptor.cursor_line     = CP( ).cursor_line( );
    new_descriptor.deleted_flag    = false;
    new_descriptor.insert_flag     = ( insert_mode( ) == INSERT ) ? true : false;
    new_descriptor.tab_setting     = tab_distance( );
    new_descriptor.window_column   = CP( ).window_column( );
    new_descriptor.window_line     = CP( ).window_line( );
}


/*!
 * These functions have to be different for each type of YEditFile. In future versions the
 * behavior of certain commands will vary depending on the extension of the file. This will be
 * done by deriving from YEditFile for each type of file and reimplementing the relevant
 * commands. The reimplementations will be accessed as virtual functions to reduce the impact on
 * the rest of the program. Here are the default behaviors for those virtual functions.
 */
bool YEditFile::next_procedure( )
{
    error_message( "Can't find procedures in this file type" );
    return false;
}

bool YEditFile::previous_procedure( )
{
    error_message( "Can't find procedures in this file type" );
    return false;
}

bool YEditFile::extra_indent( )
{
    return false;
}

bool YEditFile::insert_char( char letter )
{
    return CharacterEditFile::insert_char( letter );
}


/*!
 * This function displays the contents of an YEditFile on the screen. Everything is updated.
 * including the border, etc.
 */
void YEditFile::display( )
{
    int   i;
    char  buffer[40+1];
    // Used to hold the row, column position. The arbitrary static limit will only be a problem
    // if the number of digits involved grows to this quantity.

    int screen_width  = scr::number_of_columns( );
    int screen_height = scr::number_of_rows( );
    scr::BoxChars *box_type = scr::get_box_characters( scr::DOUBLE_LINE );

    // The following have to do with where the file name is displayed. This function assumes the
    // space is not "small." It displays very long names in a reasonable way and it will use the
    // full width of a big screen. However, it will have problems with very small screens!


    // Column for left side of file name.
    const int left_anchor = 5;

    // Max column for right side of file name.
    const int right_max = screen_width - 5;

    // Number of characters availble for name.
    const int name_width = right_max - left_anchor - 3;

    // First erase the old image.
    scr::clear( 1, 1, screen_width, screen_height, color );

    // Now draw the border.
    scr::draw_box( 1, 1, screen_width, screen_height, scr::DOUBLE_LINE, color );

    // Set visual is_changed flag.
    if( is_changed ) scr::print_text( 1, 3, 1, "*" );
    else scr::print_text( 1, 3, 1, "%c", box_type->horizontal );

    // The following several sections display the name of the file.
    int screen_offset    = left_anchor;
    int file_name_length = file_name.length( );

    // First, display the left hand border character.
    scr::print_text( 1, screen_offset, 2, "%c ", box_type->left_stop );
    screen_offset += 2;

    // If the name fits, just print it.
    if( file_name_length <= name_width ) {
        scr::print_text( 1, screen_offset, file_name_length, "%s", file_name.c_str( ) );
        screen_offset += file_name_length;
    }
    // Otherwise print the right hand part and show some dots to indicate that not all the path
    // is being displayed.
    //
    else {
        scr::print_text( 1, screen_offset, 3, "..." );
        screen_offset += 3;
        int p = file_name_length - ( name_width - 3 );
        scr::print_text( 1, screen_offset, name_width - 3, "%s", file_name.c_str( ) + p );
        screen_offset += name_width - 3;
    }

    // Display the right hand border character.
    scr::print_text( 1, screen_offset, 2, " %c", box_type->right_stop );

    // Display an 'I' in the upper left corner if we are in insert mode.
    if (insert_mode() == INSERT) scr::print_text(1, screen_width - 3, 1, "I");
    else scr::print_text(1, screen_width - 3, 1, "%c", box_type->horizontal);

    // Write the position onto the lower right corner of the screen.
    std::sprintf( buffer, "(%ld, %u)",
                  current_point.cursor_line( ) + 1, current_point.cursor_column( ) + 1 );

    scr::print_text(
        screen_height,
        screen_width - std::strlen( buffer ) - 3,
        std::strlen( buffer ),
        "%s",
        buffer
    );

    // Prepare list for sequential access.
    file_data.jump_to( current_point.window_line( ) );

    // Loop until screen is full or list is empty.
    const char *line;
    EditBuffer *edit_line;
    for( i = 2; i < screen_height; i++ ) {

        // Get a pointer to this line.
        if( ( edit_line = file_data.next( ) ) != NULL ) {
            std::string temp = edit_line->to_string( );
            line = temp.c_str( );

            // Advance line to point at appropriate column (or at null byte).
            const char *base = line;
            while( ( line - base ) < static_cast<std::ptrdiff_t>( current_point.window_column( ) )  &&  *line ) line++ ;

            // Copy text from list into screen until null is found (or right edge). print_text()
            // can only handle 1024 byte strings (after formatting). I want to use print_text()
            // in this way for performance reasons. I don't want to call print_text() for each
            // and every character.
            //
            // Profiling shows that the call to draw_box() above requires 38% of the execution
            // time of this function. Inside draw_box() calls to print() that print single
            // characters account for over 70% of the execution time.
            //
            // BTW... 30% of the execution time for this function is in the call to clear(). The
            // code below only accounts for 11% of the time.

            static char line_buffer[1024+1];
            // Used to hold a line of text before going to the screen. This is effectively the
            // maximum width display Y20 can handle.

            std::size_t length = std::strlen( line ) + 1;
            length = ( length > 1024 ) ? 1024 : length;

            std::memcpy( line_buffer, line, length );
            scr::print_text( i, 2, screen_width-2, "%s", line_buffer );
        }
    }

    // If block mode is active, indicate block.
    if( get_block_state( ) ) {

        long top, bottom;
        block_limits( top, bottom );

        int  top_row, bottom_row;

        // Compute screen row number for top of block.
        if( top < current_point.window_line( ) )
            top_row = 1 + 1;
        else
            top_row = ( int ) ( top - current_point.window_line( ) ) + 2;

        // Compute screen row number for bottom of block.
        if( bottom >= current_point.window_line( ) + screen_height - 2 )
            bottom_row = screen_height - 1;
        else
            bottom_row = ( int ) ( bottom - current_point.window_line( ) ) + 2;

        // Insert revised attribute into screen image.
        for( int j = top_row; j <= bottom_row; j++ ) {
            scr::set_color( j, 2, screen_width - 2, 1, scr::BLACK|scr::REV_WHITE );
        }
    }

    // Position cursor.
    scr::set_cursor_position(
              ( int ) ( 2 + current_point.cursor_line( ) - current_point.window_line( ) ),
                        2 + current_point.cursor_column( ) - current_point.window_column( ) );
}
