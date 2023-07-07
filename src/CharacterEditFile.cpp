/*! \file    CharacterEditFile.cpp
 *  \brief   Implementation of class CharacterEditfile
 *  \author  Peter C. Chapin <spicacalitkelseymountain.org>
 */

#include <cstring>

#include "EditBuffer.hpp"
#include "CharacterEditFile.hpp"
#include "support.hpp"

//! Toggles the insert/replace mode.
void CharacterEditFile::toggle_insert()
{
    insert_state = ( insert_state == INSERT ) ? REPLACE : INSERT;
}


//! Changes the insert mode.
/*!
 *  \param new_mode The insert/replace mode to use.
 */
void CharacterEditFile::set_insert(InsertMode new_mode)
{
    insert_state = new_mode;
}


//! Insert a new line.
/*!
 * This function does the primitive new-line operation. This involves inserting a line after the
 * current point, and bringing any characters after the cursor down. This function does not
 * actually move the current point.
 *
 * \bug is_changed is set to true before the object is sure it won't have memory problems.
 *
 * \return false if creating the new line fails (out of memory?); true otherwise.
 */
bool CharacterEditFile::new_line()
{
    bool return_value;

    // Syncronize the list and the cursor.
    file_data.jump_to(current_point.cursor_line());

    // Do nothing if the cursor is off the end of the file.
    if( file_data.get( ) == NULL ) return true;

    is_changed = true;

    // See if cursor is off the end of the line.
    if( file_data.get( )->length( ) < current_point.cursor_column( ) ) {

        // If so, just insert a blank line after the current line.
        EditBuffer *blank = new EditBuffer( "" );
        file_data.next( );
        if( blank == NULL ) return_value = false;
            else return_value = static_cast< bool >( file_data.insert( blank ) != NULL );
    }
    else {
        // Otherwise, transfer text to next line.
        EditBuffer *current_buffer = file_data.get( );
        EditBuffer *new_stuff = new EditBuffer(
            current_buffer->subbuffer( current_point.cursor_column( ),
                                       current_buffer->length( ) ) );
        file_data.next( );
        if( new_stuff == NULL ) return_value = false;
        else {
            return_value = static_cast<bool>( file_data.insert( new_stuff ) != NULL );
            file_data.previous( );
            file_data.previous( );

            // Now, delete the text on the old line only if the above worked.
            if( return_value != false ) {
                file_data.get( )->trim( current_point.cursor_column( ) );
            }
        }
    }

    if( !return_value ) {
        memory_message( "Can't insert a new line into the file" );
    }
    return return_value;
}


//! Insert a single character.
/*!
 * This function inserts a character into the object at the current point. If block mode is on,
 * this function will do the insertion on every line in the block. The current point is not
 * moved. The function returns false if it runs out of memory.
 *
 * \bug is_changed is adjusted before the object knows that it won't have memory problems.
 *
 * \param letter The character to insert. [Are there restrictions?]
 * \return false if the insert operation fails (out of memory?); true otherwise.
 */
bool CharacterEditFile::insert_char( char letter )
{
    bool return_value = true;

    // Make changes.
    is_changed = true;

    // Set list to top of block (block may be one line).
    long top, Bottom;
    block_limits( top, Bottom );

    if( !top_of_block( ) ) return false;

    // Loop over all lines in the block, inserting as we go.
    while( top++ <= Bottom && return_value == true ) {
        file_data.get( )->insert( letter, current_point.cursor_column( ) );
        file_data.next( );
    }

    if( return_value == false ) {
        memory_message( "Can't insert character into file" );
    }
    return return_value;
  }


//! Replace a single character.
/*!
 * This function replaces the current character in the object. If block mode is on, this
 * function will do the replacement on every line in the block. The current point is not moved.
 *
 * \bug is_changed is adjusted before the object knows that it won't have memory problems.
 *
 * \param letter The letter that will replace the current letter.
 * \return false if the replacement fails (out of memory?); true otherwise.
 */
bool CharacterEditFile::replace_char( char letter )
  {
    bool    return_value = true;
    char    new_letter;

    // Make changes.
    is_changed = true;

    // Set list to top of block (block may be one line).
    long top, bottom;
    block_limits( top, bottom );

    if( !top_of_block( ) ) return false;

    // Loop over all lines in the block, inserting as we go.
    while( top++ <= bottom && return_value == true ) {
        new_letter   = letter;
        file_data.get( )->replace( new_letter, current_point.cursor_column( ) );
        file_data.next( );
    }

    if( return_value == false ) {
        memory_message( "Can't extend current line" );
    }
    return return_value;
  }


//! Deletes the character to the left of the current point.
/*!
 * This function deletes the character to the left of the current point. If block mode is on, a
 * character is deleted on every line. If the current point is at the beginning of a line, that
 * line is joined with the previous line (if there is one).
 *
 * \return false if the deletion fails (out of memory?); true otherwise.
 */
bool CharacterEditFile::backspace( )
{
    // If we're at the start of a line and in block mode, do nothing.
    if( current_point.cursor_column( ) == 0  &&  get_block_state( ) ) return true;
    is_changed = true;

    // If we're at the start of a line and not in block mode, join lines.
    if( current_point.cursor_column( ) == 0  && !get_block_state( ) ) {

        // Make sure there is a line above us.
        if( current_point.cursor_line( ) > 0 ) {

            // Synchronize list.
            file_data.jump_to( current_point.cursor_line( ) );

            // Do the dirty deed.
            EditBuffer *current = file_data.get( );
            if( current != NULL ) {
                file_data.previous( );
                file_data.get( )->append( *current );
                file_data.next( );
                delete current;
                file_data.erase( );
            }
        }
    }

    // Otherwise we must be in the interior of a line.
    else {

        // If the block is entirely off the end of the file, do nothing.
        long top, bottom;
        block_limits( top, bottom );

        if( top > file_data.size( ) ) return true;

        // Synchronize list.
        file_data.jump_to( top );

        // Loop over all lines in the block, backspacing as we go.
        while( top++ <= bottom && file_data.get( ) != NULL ) {
            file_data.get( )->erase( current_point.cursor_column( ) - 1 );
            file_data.next( );
        }
    }

    return true;
  }


//! Delete the character under the cursor.
/*!
 * This function deletes the character under the cursor. If the current point is at or past the
 * end of the line, the next line (if there is one) is joined to the current line at the cursor
 * position. The current point is not moved.
 *
 * \return false if the deletion fails (out of memory?); true otherwise.
 */
bool CharacterEditFile::delete_char()
{
    bool return_value = true;

    is_changed = true;

    // Get the goods about the current line.
    file_data.jump_to( current_point.cursor_line( ) );
    EditBuffer *Current = file_data.get( );

    // If off the end of the line, try to join lines (only if not in block mode).
    if(Current != NULL                               &&
       current_point.cursor_column( ) >= Current->length( )  &&
      !get_block_state( )                               ) {

        // Extend the current line and append the next line.
        char Space_Character = ' ';
        Current->replace( Space_Character, current_point.cursor_column( ) );
        file_data.next( );
        if( file_data.get( ) != NULL && return_value != false ) {
            Current->append( *file_data.get( ) );
            if( return_value != false ) file_data.erase( );
        }

        // Delete extra character introduced in the replace action.
        Current->erase( current_point.cursor_column( ) );
    }

    // Otherwise try to do the delete for the whole block (or line).
    else {

        // If the block is entirely off the end of the file, do nothing.
        long top, bottom;
        block_limits( top, bottom );

        if( top > file_data.size( ) ) return true;

        // Synchronize list.
        file_data.jump_to( top );

        // Loop over all lines in the block, deleting as we go.
        while( return_value == true && top++ <= bottom && file_data.get( ) != NULL ) {
            return_value =
                static_cast< bool >( file_data.get( )->erase( current_point.cursor_column( ) ) != '\0' );
            file_data.next( );
        }
    }

    if( return_value == false ) {
        memory_message( "Can't manipulate text" );
    }

    return return_value;
  }
