/*! \file    TextWindow.hpp
    \brief   Interface to class TextWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef TEXTWINDOW_HPP
#define TEXTWINDOW_HPP

#include "Window.hpp"

namespace scr {

    class TextWindow : public SimpleWindow {
    private:
        int current_row;

    public:
        TextWindow( ) : current_row( 0 ) { }

        void print( const char *format_string, ... );

        //! Sets the current row to new_row.
        void set_row( int new_row )
            { current_row = ( new_row <= height( ) - 1 ) ? new_row : height( ) - 1; }
        void print_at( int row, int column, const char *format_string, ... );
        void clear( int row_offset );
        void home( );
        void get( int start_row, int start_column, char *buffer, int size );
    };
}

#endif

