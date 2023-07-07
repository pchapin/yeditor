/*! \file    LineEditFile.cpp
 *  \brief   Implementation of class LineEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstring>

#include "EditBuffer.hpp"
#include "LineEditFile.hpp"
#include "support.hpp"

//! This function returns a pointer to the text of the current line.
const EditBuffer *LineEditFile::get_line( )
{
    if( !extend_to_line( current_point.cursor_line( ) ) ) return NULL;
    file_data.jump_to( current_point.cursor_line( ) );
    return file_data.get( );
}


/*!
 * This function computes the length of the current line. If the current line does not exist, it
 * returns zero without trying to extend the file.
 */
unsigned LineEditFile::CP_line_length( )
{
    EditBuffer *current_line;

    file_data.jump_to( current_point.cursor_line( ) );
    current_line = file_data.get( );
    if( current_line == NULL ) return 0;
    return current_line->length( );
}


/*!
 * This function inserts the given line above the current point in the file. It returns false if
 * there is a loss of memory during the operation.
 */
bool LineEditFile::insert_line( const EditBuffer *new_line )
{
    // Extend file data if necessary.
    if( !extend_to_line( current_point.cursor_line( ) ) ) return false;
    file_data.jump_to( current_point.cursor_line( ) );

    // Make changes.
    is_changed = true;
    EditBuffer *new_stuff = new EditBuffer( *new_line );
    if( file_data.insert( new_stuff ) == NULL ) {
        memory_message( "Can't insert line into file" );
        return false;
    }
    return true;
}


/*!
 * This function replaces the current line with the new material. It returns false if there is a
 * memory error encountered during the operation.
 */
bool LineEditFile::replace_line( const EditBuffer *new_line )
{
    // Extend file data if necessary.
    if( !extend_to_line( current_point.cursor_line( ) ) ) return false;
    file_data.jump_to( current_point.cursor_line( ) );

    // Make changes.
    is_changed = true;
    delete file_data.get( );
    file_data.erase( );

    EditBuffer *new_stuff = new EditBuffer( *new_line );
    if( file_data.insert( new_stuff ) == NULL ) {
        memory_message( "Can't insert line into file" );
        return false;
    }
    return true;
}


//! This function deletes the current line forever.
void LineEditFile::delete_line( )
{
    // Syncronize the list and the cursor.
    file_data.jump_to( current_point.cursor_line( ) );

    // Do nothing if the cursor is off the end of the file.
    if( file_data.get( ) == NULL ) return;

    // Make changes.
    is_changed = true;
    delete file_data.get( );
    file_data.erase( );
}


/*!
 * This function deletes from the current point to the end of the line. Notice that if block
 * mode is on, every line in the block is affected.
 */
void LineEditFile::delete_to_EOL( )
{
    // Set list to top of block.
    long top, bottom;
    block_limits( top, bottom );
    file_data.jump_to( top );

    // Loop over all lines in the block, deleting as we go.
    while( top++ <= bottom ) {

        // If there's nothing left in the file, just break out.
        if( file_data.get( ) == NULL ) break;

        // Delete all the characters on this line to the end.
        while( file_data.get( )->length( ) > current_point.cursor_column( ) ) {
            is_changed = true;
            file_data.get( )->erase( current_point.cursor_column( ) );
        }

        file_data.next( );
    }
}
