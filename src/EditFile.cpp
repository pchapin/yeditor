/*! \file    EditFile.cpp
 *  \brief   Implementation of class EditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <climits>
#include <cstddef>

#include "EditBuffer.hpp"
#include "EditFile.hpp"
#include "support.hpp"

/*=========================================================*/
/*           Member Functions of class EditFile           */
/*=========================================================*/

//! Constructor
EditFile::EditFile( )
{
    block          = false;
    anchor         = 0L;
    is_changed     = false;
    constructed_ok = true;
}


//! Destructor
EditFile::~EditFile( )
{
    return;
}


//! Erases the data in this EditFile object.
void EditFile::erase( )
{
    if( file_data.size( ) > 0L ) is_changed = true;
    file_data.clear( );
}


//! Extends, if necessary, the file's data to include a particular line.
/*!
 * This function ensures that the EditFile contains at least line line_number. If the desired
 * line is off the end of the current data, blank lines are added to the data until there are
 * enough lines. If the extension does not succeed the object may be left with some additional
 * lines added to it anyway.
 *
 * \param line_number The line that is to become part of the file's data. Zero based numbering.
 * \return True if the extension is successful (or if no extension was necessary).
 * \throws std::bad_alloc if there is insufficient memory to do the extension.
 *
 * \todo If line_number is negative, bad things will happen.
 * \todo Currently this method always returns true because the extension will never fail if
 * there is sufficient memory. Perhaps the return type should be changed to void.
 *
 * \bug There is a memory leak if a blank like is allocated successfully but insertion into the
 * file data fails.
 */
bool EditFile::extend_to_line( long line_number )
{
    bool return_value = true;

    // If the file is already big enough, just return.
    if( file_data.size( ) > line_number ) return true;

    // Position the list to the end.
    file_data.set_end( );

    // Compute number of new lines required.
    long line_count = line_number - file_data.size( ) + 1;

    // Insert the new lines.
    while( return_value == true  &&  line_count-- ) {
        EditBuffer *blank = new EditBuffer( "" );
        file_data.insert( blank );
    }

    return return_value;
}


//! Return True if the EditFile was constructed successfully.
/*!
 * \todo What is the point of this method? The constructor assigns primitives so it can't fail.
 */
bool EditFile::built_ok( )
{
    return constructed_ok;
}


//! Return the range of the current block, if any.
/*!
 * A block exists if the 'block' flag is set. Under that condition, the block ranges between
 * anchor and the current line. If the 'block' flag is not set, this function returns the line
 * number of the current line for both top and bottom.
 *
 * This function does not extend the file if one of the block limits is off the end of the data.
 *
 * \param top [out] The variable to recieve the line number of the first line in the block.
 * \param bottom [out] The variable to receive the line number of the last line in the block.
 */
void EditFile::block_limits( long &top, long &bottom )
{
    if( !block ) {
        top    = current_point.cursor_line( );
        bottom = current_point.cursor_line( );
    }
    else if( anchor <= current_point.cursor_line( ) ) {
        top    = anchor;
        bottom = current_point.cursor_line( );
    }
    else {
        top    = current_point.cursor_line( );
        bottom = anchor;
    }
}


//! Turn block mode on or off.
/*!
 * When block mode is turned on the current line is used as the "anchor" or starting position of
 * the block.
 *
 * \param new_info Block mode is turned on if True and off otherwise.
 */
void EditFile::set_block_state( bool new_info )
{
    block = new_info;
    if( block ) {
        anchor = current_point.cursor_line( );
    }
}


//! Returns the current state of block mode.
/*!
 * \return True if block mode is on.
 *
 * \todo It would probably be better to use an enumeration type to represent block mode than the
 * plain bool type.
 */
bool EditFile::get_block_state( )
{
    return block;
}


//! Jump the current point to the top line of the active block.
/*!
 * This function positions the current point of file_data at the top of the current block (or at
 * the current line if there is no block). The data is extended as needed if the top of the
 * block is off the end of the data.
 *
 * \return False if a file extension was necessary but failed. In that case the current point is
 * not moved.
 *
 * \todo This seems like a strange function because moving the current point changes the block
 * extent. How is this function used?
 * \todo If extend_to_line( ) is changed to return void, this function can also be changed to
 * return void.
 */
bool EditFile::top_of_block( )
{
    // Set list to top of block (block may be one line).
    long top, bottom;
    block_limits( top, bottom );

    if( !extend_to_line( bottom ) ) return false;
    file_data.jump_to( top );
    return true;
}
