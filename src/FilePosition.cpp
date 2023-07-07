/*! \file    FilePosition.cpp
 *  \brief   Implementation of class FilePosition
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "FilePosition.hpp"
#include "scr.hpp"

// When moving the cursor off the edge of the window these values hold the number of lines (or
// columns) that the window moves in response. Setting these values to 1 results in "smooth"
// scrolling. Do not set either of these values to zero!
//
const int      window_vertical_jump_distance = 1;
const unsigned window_horizontal_jump_distance = 1U;


//! Initializes a FilePosition object so the cursor is at the top of the file.
FilePosition::FilePosition( )
    : c_line  ( 0L ),
      c_column( 0U ),
      w_line  ( 0L ),
      w_column( 0U ),
      w_heigth( scr::number_of_rows( ) - 2 ),
      w_width ( static_cast<unsigned>( scr::number_of_columns( ) ) - 2 )
{
    // TODO: Assert that w_heigth and w_width are adequate.
}


//! Initializes a FilePosition object according to the parameters.
/*!
 * The parameters are sanity checked before the constructor returns. If necessary the window is
 * moved so that the cursor is inside the window.
 *
 * \bug The sanity checking done here behaves in a potentially unexpected way in the case where
 * the cursor line and the window line have their maximum values and yet the window height is
 * not zero. There is a corresponding issue in the column computations.
 */
FilePosition::FilePosition( const long     initial_cursor_line,
                            const unsigned initial_cursor_column,
                            const long     initial_window_line,
                            const unsigned initial_window_column )
    : c_line  ( initial_cursor_line   ),
      c_column( initial_cursor_column ),
      w_line  ( initial_window_line   ),
      w_column( initial_window_column ),
      w_heigth( scr::number_of_rows( ) - 2 ),
      w_width ( static_cast<unsigned>( scr::number_of_columns( ) ) - 2 )
{
    // Check sanity on cursor position.
    if( c_line   < 0L ) c_line   = 0L;
    if( w_line   < 0L ) w_line   = 0L;
    // TODO: Assert that w_heigth and w_width are adequate.

    // Check that the cursor is in the window. Move the window if necessary.
    if( c_line <  w_line            ) w_line = c_line;
    if( c_line >= w_line + w_heigth ) w_line = ( c_line - w_heigth ) + 1;
    if( c_column <  w_column           ) w_column = c_column;
    if( c_column >= w_column + w_width ) w_column = ( c_column - w_width ) + 1;
}


//! Move the view down one page.
/*!
 * When the jump is done, the cursor keeps the same position relatiave to the window.
 *
 * \param jump_distance The number of lines to move the window. -1L indicates an exact window
 * size. No other negative values are allowed.
 *
 * \bug It is possible to jump beyond line number LONG_MAX.
 */
void FilePosition::page_down( long jump_distance )
  {
    if( jump_distance <= -1L ) jump_distance = w_heigth;
    w_line += jump_distance;
    c_line += jump_distance;
  }


//! Move the view up one page.
/*!
 * When the jump is done, the cursor keeps the same position relative to the window. If an
 * attempt is made to jump before the beginning of the text, the window (and if necessary the
 * cursor) saturate at offset zero from the start of the text.
 *
 * \param jump_distance The number of lines to move the window. -1L indicates an exact window
 * size. No other negative values are allowed.
 *
 * \bug If one tries to page before the beginning of the text a situation can arise where, due
 * to window position adjustment, the relative position of the cursor in the window changes.
 */
void FilePosition::page_up( long jump_distance )
  {
    if( jump_distance <= -1L ) jump_distance = w_heigth;
    w_line -= jump_distance;
    c_line -= jump_distance;
    if( w_line < 0L ) w_line = 0L;
    if( c_line < 0L ) c_line = 0L;
  }


//! Move the view to the right.
/*!
 * When the jump is done, the cursor only moves when necessary. In that case it is dragged
 * along by the edge of the window.
 *
 * \param jump_distance The number of columns to move the window. -1 indicates an exact window
 * size. No other negative values are allowed.
 *
 * \bug It is possible to jump beyond column number UINT_MAX.
 */
void FilePosition::pan_right( const int jump_distance )
{
    unsigned actual_jump_distance;

    // Compute how far we are really jumping.
    if( jump_distance <= -1 )
        actual_jump_distance = w_width;
    else
        actual_jump_distance = static_cast<unsigned>( jump_distance );

    // Do the jump. Correct cursor position if necessary.
    w_column += actual_jump_distance;
    if( c_column < w_column ) c_column = w_column;
}


//! Move the view to the left.
/*!
 * When the jump is done, the cursor only moves when necessary. In that case it is dragged
 * along by the edge of the window.
 *
 * \param jump_distance The numbr of columns to move the window. -1 indicates an exact window
 * size. No other negative values are allowed.
 */
void FilePosition::pan_left( const int jump_distance )
{
    unsigned actual_jump_distance;

    // Compute how far we are really jumping.
    if( jump_distance <= -1 )
        actual_jump_distance = w_width;
    else
        actual_jump_distance = static_cast<unsigned>( jump_distance );

    // Do the jump.
    if( actual_jump_distance > w_column )
        w_column = 0;
    else
        w_column -= actual_jump_distance;

    // Correct cursor position if necessary.
    if( c_column >= w_column + w_width )
        c_column = w_column + w_width - 1;
}


//! Adjust window vertical position.
/*!
 * Set the window so that the cursor line is at the indicated offset from the top of the window.
 * If the final window line is before the beginning of the text it is forced to zero. This
 * reduces the cursor offset below what was desired. The cursor is not moved in any case.
 *
 * \param cursor_offset The desired final position of the cursor relative to the top of the
 * window. Negative values result in undefined behavior.
 */
void FilePosition::adjust_window_line( int cursor_offset )
{
    if( cursor_offset < 0 ) cursor_offset = 0;
    if( cursor_offset >= w_heigth ) cursor_offset = w_heigth - 1;
    w_line = c_line - cursor_offset;
    if( w_line < 0L ) w_line = 0L;
}


//! Adjust the window horizontal position.
/*!
 * Set the window so that the cursor column is at the indicated offset from the left side of the
 * window. If the final window column is before the beginning of the text it is forced to zero.
 * This reduces the cursor offset below what was desired. The cursor is not moved in any case.
 *
 * \param cursor_offset The desired final position of the cursor relative to the left edge of
 * the window. Negative values result in undefined behavior.
 */
void FilePosition::adjust_window_column( unsigned cursor_offset )
{
    if( cursor_offset >= w_width ) cursor_offset = w_width - 1;
    if( cursor_offset > c_column )
        w_column = 0U;
    else
        w_column = c_column - cursor_offset;
}


//! Move the cursor down.
/*!
 * If the cursor is moved outside the window, the window line is adjusted so that the cursor is
 * inside the window on a line that is window_vertical_jump_distance above the bottom frame.
 *
 * \param count The number of lines the cursor is to move. Negative counts are allowed. In that
 * case, the cursor is moved up instead.
 *
 * \bug It is possible to jump beyond line LONG_MAX.
 */
void FilePosition::cursor_down( const long count )
{
    if( count < 0 ) cursor_up( -count );
    else {
        c_line += count;
        if( c_line >= w_line + w_heigth )
            w_line = ( c_line - w_heigth ) + window_vertical_jump_distance;
    }
}


//! Move the cursor up.
/*!
 * If the cursor is moved outside the window, the window line is adjusted so that the cursor is
 * inside the window on a line that is window_vertical_jump_distance below the top frame.
 *
 * \param count The number of lines the cursor is to move. Negative counts are allowed. In that
 * case, the cursor is moved down instead.
 */
void FilePosition::cursor_up( const long count )
{
    if( count < 0 ) cursor_down( -count );
    else {
        c_line -= count;
        if( c_line < 0L ) c_line = 0L;
        if( c_line < w_line ) w_line = c_line - ( window_vertical_jump_distance - 1 );
        if( w_line < 0L ) w_line = 0L;
    }
}


//! Move the cursor right.
/*!
 * If the cursor is moved outside the window, the window column is adjusted so that the cursor
 * is inside the window on a column that is window_horizontal_jump_distance to the left of the
 * right frame.
 *
 * \param count The number of columns the cursor is to move.
 *
 * \bug It is possible to jump beyond column UINT_MAX.
 */
void FilePosition::cursor_right( const unsigned count )
{
    c_column += count;
    if( c_column >= w_column + w_width )
        w_column = ( c_column - w_width ) + window_horizontal_jump_distance;
}


//! Move the cursor left.
/*!
 * If the cursor is moved outside the window, the window column is adjusted so that the cursor
 * is inside the window on a column that is window_horizontal_jump_distance to the right of the
 * left frame.
 *
 * \param count The number of columns the cursor is to move.
 */
void FilePosition::cursor_left( const unsigned count )
{
    if( count >= c_column ) {
        c_column = 0U;
        w_column = 0U;
    }
    else {
        c_column -= count;
        if( c_column < w_column ) {
            if( c_column >= window_horizontal_jump_distance - 1 )
                w_column = c_column - ( window_horizontal_jump_distance - 1 );
            else
                w_column = 0U;
        }
    }
}


//! Jump the cursor to a specific line.
/*!
 * \param new_line The target line.
 */
void FilePosition::jump_to_line( const long new_line )
{
    if( new_line >= w_line && new_line < w_line + w_heigth ) {
        c_line = new_line;
    }
    else {
        w_line = new_line - w_heigth/2;
        c_line = new_line;
        if( w_line < 0L ) w_line = 0L;
        if( c_line < 0L ) c_line = 0L;
    }
}


//! Jump the cursor to a specific column.
/*!
 * \param new_column The target column.
 */
void FilePosition::jump_to_column( const unsigned new_column )
{
    if( new_column >= w_column && new_column < w_column + w_width ) {
        c_column = new_column;
    }
    else {
        c_column = new_column;
        if( new_column < w_width/2 )
            w_column = 0U;
        else
            w_column = new_column - w_width/2;
    }
}
