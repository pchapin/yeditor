/*! \file    TextWindow.cpp
    \brief   Implementation of class TextWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "scr.hpp"
#include "scrtools.hpp"
#include "TextWindow.hpp"

using namespace std;

namespace scr {

    /*!
     * This helper class makes it easier to manage hidden windows. When an object of this class
     * is constructed the state of a specified window is remembered and that window is forced to
     * be visable. When the object is then destroyed, the window is rehidden if it was hidden in
     * the first place. Using an object of this class in the TextWindow methods below allows
     * those methods to operate correctly on hidden windows (by temporarly showing them for the
     * duration of the operation).
     */
    class TextWindowHideManager {
    private:
        bool        hidden_state;
        TextWindow *managed_window;

    public:
        TextWindowHideManager( TextWindow *managed, bool was_hidden ) :
            hidden_state( was_hidden ), managed_window( managed ) { managed_window->show( ); }

       ~TextWindowHideManager( )
        { if( hidden_state == true ) managed_window->hide( ); }
    };


    //! Print formatted output into window on current line.
    /*!
     * Scroll vertically if necessary and erase rest of line.
     */
    void TextWindow::print( const char *format_string, ... )
    {
        if( !is_defined ) return;
        TextWindowHideManager object( this, is_hidden );

        va_list arg_pointer;
        int     actual_row;
        int     actual_column;
        char    buffer[128+1];

        if( current_row == height( ) ) {
            scroll( UP, row( ), column( ), width( ), height( ), 1, color( ) );
            current_row--;
        }

        va_start( arg_pointer, format_string );

        // Compute actual screen coordinates.
        actual_row = row( ) + current_row;
        actual_column = column( );

        // Print information into a local buffer. Note: problems arise if buffer too short.
        vsprintf( buffer, format_string, arg_pointer );

        // Print material on the screen, making sure the rest of the line is cleared.
        print_text( actual_row, actual_column, width( ), "%s", buffer );
        if( strlen( buffer ) < static_cast< size_t >( width( ) ) )
            scr::clear( actual_row, actual_column + strlen( buffer ), width( ) - strlen( buffer ), 1, color( ) );

        // Next call to this function will print on the next row.
        current_row++;

        va_end( arg_pointer );
    }


    //! Print formatted output into window at specified coordinates.
    /*!
     * No scroll and no unnecessary erasing. Note that the coordinates are zero based.
     */
    void TextWindow::print_at( int print_row, int print_column, const char *format_string, ... )
    {
        if( !is_defined ) return;
        TextWindowHideManager object( this, is_hidden );

        va_list arg_pointer;
        char    buffer[128+1];

        // If coordinates outside of the window, return at once.
        if( print_row >= height( ) ) return;
        if( print_column >= width( ) ) return;

        va_start( arg_pointer, format_string );

        // Compute the actual screen coordinates.
        int actual_row = row( ) + print_row;
        int actual_column = column( ) + print_column;

        // Print information into a local buffer. Note: problems arise if buffer too short.
        vsprintf( buffer, format_string, arg_pointer );

        // Print material on the screen.
        print_text( actual_row, actual_column, width( ) - print_column - 1, "%s", buffer );

        va_end( arg_pointer );
    }


    //! Erase the specifed row.
    void TextWindow::clear( int row_offset )
    {
        TextWindowHideManager object( this, is_hidden );

        if( is_defined == true )
            ::scr::clear( row( ) + row_offset, column( ), width( ), 1, color( ) );
    }


    //! Make the current line the top of the window. Nothing is output.
    void TextWindow::home( )
    {
        current_row = 0;
    }


    //! Read a string out of the window.
    /*!
     * The buffer must be size + 1 bytes long to hold the null byte.
     */
    void TextWindow::get( int start_row, int start_column, char *buffer, int size )
    {
        if( !is_defined ) return;
        TextWindowHideManager object( this, is_hidden );

        gets(
             row( ) + start_row,
             column( ) + start_column,
             size <= width( ) - start_column ? size : width( ) - start_column,
             color( ),
             buffer
         );
    }
    
}
