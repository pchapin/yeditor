/*! \file    FilePosition.hpp
 *  \brief   Interface to class FilePosition
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the interface to class FilePosition. Objects of this class record and
 * manipulate the description of where in the file "we are." They are interested not only in the
 * location of the cursor (current point), but also the location of the window (ie which portion
 * of the file is currently to be displayed. Objects of this class insure that the cursor never
 * strays outside the window.
 *
 * Notice that the first line in the file is line zero. Further notice that the first column in
 * the file is column zero. Line numbers are of type long (negative numbers treated like zero),
 * and column numbers are of type unsigned.
 *
 * This implementation is simple in that the dimensions of the window are hard coded to satisfy
 * Ys needs. Obviously one can do better than that. In a future version, FilePosition objects
 * may be able to deal with various sized windows. Certainly, this situation will need
 * correcting before Y could become multiwindowed (should that ever become desirable).
*/

#ifndef FILEPOSITION_HPP
#define FILEPOSITION_HPP

class FilePosition {

private:
    long     c_line;   //!< Line number of current point (0..whatever).
    unsigned c_column; //!< Column number of current point (0..whatever).
    long     w_line;   //!< Line number of top line in display.
    unsigned w_column; //!< Column number of left column in display.

    int      w_heigth; //!< Dimensions of the window area were printing is allowed.
    unsigned w_width;

public:
    FilePosition( );
    FilePosition( long     initial_cursor_line,
                  unsigned initial_cursor_column,
                  long     initial_window_line,
                  unsigned initial_window_column );

    // Access functions.
    long     cursor_line( )   const { return c_line;   }
    unsigned cursor_column( ) const { return c_column; }
    long     window_line( )   const { return w_line;   }
    unsigned window_column( ) const { return w_column; }

    // Cursor relative jumping (-1 implies a window sized jump).
    void page_down( long jump_distance = -1L );
    void page_up  ( long jump_distance = -1L );
    void pan_right( int  jump_distance = -1  );
    void pan_left ( int  jump_distance = -1  );

    // Window alignment. Sets the window_line or the window_column so that cursor is at offset.
    void adjust_window_line  ( int      cursor_offset = 0 );
    void adjust_window_column( unsigned cursor_offset = 0U );

    // "Smooth" scrolling.
    void cursor_down ( long count = 1L );
    void cursor_up   ( long count = 1L );
    void cursor_right( unsigned count = 1 );
    void cursor_left ( unsigned count = 1 );

    // Cursor absolute jumping.
    void jump_to_line  ( long new_line );
    void jump_to_column( unsigned new_column );
};

#endif
