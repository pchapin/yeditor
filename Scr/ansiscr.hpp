/*! \file    ansiscr.hpp
    \brief   Functions for I/O using ANSI escape sequences.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

The functions in this file do I/O using ANSI standard escape sequences. For this to work an
appropriate console driver must be loaded. Typically this is only an option on Unix systems, but
MS-DOS and some other systems also support ANSI console drivers.

Although this method of screen handling is quite slow, it has the advantage of using only
standard I/O facilities. In principle this makes it very portable. This also means that, for
example, one can redirect the output of a program using this approach to a file and then later
rebuild that output by just printing the saved file.
*/

#ifndef ANSISCR_HPP
#define ANSISCR_HPP

namespace scr {

    namespace ansi {

        //! Clears the screen and positions the cursor in the upper left corner.
        void clear_screen( );

        //! Erases the line from the cursor position to the right.
        void clear_to_eol( );

        // Nice names for the colors.
        const int F_BLACK    = 30;
        const int F_BLUE     = 34;
        const int F_CYAN     = 36;
        const int F_GREEN    = 32;
        const int F_MAGENTA  = 35;
        const int F_RED      = 31;
        const int F_WHITE    = 37;
        const int F_YELLOW   = 33;
        const int B_BLACK    = 40;
        const int B_BLUE     = 44;
        const int B_CYAN     = 46;
        const int B_GREEN    = 42;
        const int B_MAGENTA  = 45;
        const int B_RED      = 41;
        const int B_WHITE    = 47;
        const int B_YELLOW   = 43;

        //! Sets the current color to 'color'.
        void set_color( int color );

        //! Turns on bold.
        void bold_on( );

        //! Turns on blink
        void blink_on( );

        //! Turns on "reverse video"
        void reverse_on( );

        //! Turns off all attributes.
        void reset_screen( );

        //! Moves the cursor to coordinates (row, column).
        void position_cursor( int row, int column );

        //! Moves the cursor up 'count' lines.
        void cursor_up( int count );

        //! Moves the cursor down 'count' lines.
        void cursor_down( int count );

        //! Moves the cursor to the right 'count' columns.
        void cursor_forward( int count );

        //! Moves the cursor to the left 'count' columns.
        void cursor_backward( int count );

        //! Saves the cursor position in an internal state variable.
        void save_cursor_position( );

        //! Restores a saved cursor position.
        void restore_cursor_position( );

        void draw_border      ( int row, int column, int width, int height );
        void fill_box         ( int row, int column, int width, int height );
        void fill_shadowed_box( int row, int column, int width, int height );
    }
}

#endif
