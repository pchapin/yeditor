/*! \file    BlockEditFile.cpp
 *  \brief   Implementation of class BlockEditFile
 *  \author  Peter C. Chapin <spicacalitkelseymountain.org>
 *
 * IMPLEMENTATION NOTES
 *
 * Is there any value in implementing a replace_block() function? Currently the client must
 * delete the block and then insert the new block. A smart replace_block() could avoid all the
 * deallocation and reallocation of the EditList nodes.
 *
 * The current version of insert_block() leaves its parameter alone. If the function was able to
 * remove lines from the parameter the copying of the EditBuffers would not be necessary.
 */

#include <cstddef>

#include "BlockEditFile.hpp"
#include "EditBuffer.hpp"
#include "support.hpp"

/*=====================================================================*/
/*           Public Member Functions of Class Block_EditFile          */
/*=====================================================================*/

void BlockEditFile::get_blockinfo( BlockInfo &result )
{
    result.is_on = block;
    if( block ) {
        result.anchor = anchor;
        result.limit  = current_point;
    }
}


void BlockEditFile::set_blockinfo( const BlockInfo &desired )
{
    if( desired.is_on ) {
        block         = true;
        anchor        = desired.anchor;
        current_point = desired.limit;
    }
}


//! Change the block mode status.
/*!
 * This function changes the block mode status. If the new status is ON, then this function
 * saves the current line number.
 */
void BlockEditFile::toggle_block( )
{
    block = !block;
    if( block ) anchor = current_point.cursor_line( );
}


//! Copy the current block to a temporary storage area.
/*!
 * This function copies the current block out of this object and inserts it into the specified
 * EditList. This function does not turn block mode off; the caller must do so. However, it will
 * properly deal with the case where the defined block goes off the end of the file or is
 * entirely defined off the end of the file. In such a case, result is given the appropriate
 * number of blank lines. The file data is not extended.
 *
 * \param result [in-out] The edit list that will receive the contents of the current block.
 * Note that the initial contents of the given EditList are not erased by this operation. The
 * block is inserted at the list's current point.
 */
bool BlockEditFile::get_block( EditList &result )
{
    bool        abort = false;           // Assume we will have memory.
    long        top, bottom;             // Top is higher on screen.
    EditBuffer *line;                    // Points at a specific line.

    // Get the current block extent.
    block_limits( top, bottom );

    // Position the main EditList to the top line (lower line number).
    file_data.jump_to( top );

    // While there is something in the list and there's still more to do...
    while( !abort  &&  ( line = file_data.next( ) ) != NULL  &&  top <= bottom ) {

        // Make a copy of the line and insert it into the parameter.
        EditBuffer *new_copy = new EditBuffer( *line );
        if( result.insert(new_copy) == NULL ) abort = true;
        top++;
    }

    // If we didn't get all the lines (ran out of list), insert blanks.
    while( !abort  &&  top <= bottom ) {
        EditBuffer *blank = new EditBuffer( "" );
        if( result.insert(blank) == NULL ) abort = true;
        top++;
    }

    // Tell user if we had problems.
    if( abort ) {
        memory_message( "Can't extract entire block" );
    }
    return !abort;
}


//! Delete the current block.
/*!
 * This function deletes the currently active block (or the current line if there is no active
 * block).
 */
void BlockEditFile::delete_block( )
{
    long top, current, bottom;    // Block limits and current line number.

    // Get the current block extent.
    block_limits( top, bottom );

    // Deleting a real lines will mark the object as changed.
    if( top < file_data.size( ) ) is_changed = true;

    // Position the data to the top line.
    file_data.jump_to( top );

    // For each line in the block trash that line. Stop if we go off the end.
    for( current = top; current <= bottom; current++ ) {
        if( file_data.get( ) == NULL ) break;
        delete file_data.get( );
        file_data.erase( );
    }

    // Make sure the current point is at the line number of the block's top.
    current_point.jump_to_line( top );
}


//! Insert block above current point.
/*!
 * This function inserts the parameter into the object just above the current point.
 *
 * \bug When a block is inserted off the end of the file, an extra blank line will be added.
 * This is because extend_to_line() ensures that the current line exists yet the block is added
 * above the current line. We really only need to be sure that the line before the current line
 * exists (if the current line is not line 0).
 *
 * \bug There is a memory leak if allocating a new line succeeds but inserting that line into
 * the file data fails.
 *
 * \param new_stuff The EditList to insert.
 * \return false if the insertion fails for some reason (out of memory?); true otherwise.
 */
bool BlockEditFile::insert_block( EditList &new_stuff )
{
    bool        abort = false;  // Assume it will work.
    EditBuffer *line;           // Points at a line in the block.

    // Insertions always change this object if there's something coming in.
    if( new_stuff.size( ) > 0L ) is_changed = true;

    // Rewind the incoming list so that we get it all.
    new_stuff.jump_to( 0 );

    // Adjust the internal list. Make sure the current line exists.
    if( !extend_to_line( current_point.cursor_line( ) ) ) return false;
    file_data.jump_to( current_point.cursor_line( ) );

    // While there are still lines in the parameter.
    while( !abort && ( line = new_stuff.next( ) ) != NULL ) {

        // Build a new copy of the line and stuff it into this object.
        EditBuffer *new_copy = new EditBuffer( *line );
        if( file_data.insert( new_copy ) == NULL) abort = true;
    }

    if( abort ) {
        memory_message( "Can't insert entire block into file" );
    }
    else {
        // Jump down to just past the end of new stuff.
        current_point.jump_to_line( current_point.cursor_line( ) + new_stuff.size( ) );
    }

    return !abort;
}
