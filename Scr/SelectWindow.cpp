/*! \file    SelectWindow.cpp
    \brief   Implementation of class SelectWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <climits>
#include "scr.hpp"
#include "scrtools.hpp"
#include "SelectWindow.hpp"

using namespace std;

namespace scr {

    bool SelectWindow::open(
        int row,    int column,
        int width,  int height,
        int color,  int Status_Color,
        BoxType border, int border_color )
    {
        highlight_color = Status_Color;
        show_bar        = false;
        return
            DisplayWindow::open(
                row,    column,
                width,  height,
                color,  Status_Color,
                border, border_color);
    }


    void SelectWindow::show( )
    {
        char  buffer[160];
        // I will probably need a private member to hold the current position.
        long  current = 0;

        DisplayWindow::show( );
        if( show_bar ) {
            if( current >= top_line && current < top_line + height( ) ) {
                read( row( ) + ( current - top_line ), column( ), width( ), 1, buffer );
                for( int i = 0; i < width( ); i++ ) buffer[1 + 2 * i] = highlight_color;
                write( row( ) + ( current - top_line ), column( ), width( ), 1, buffer);
            }
        }
    }

    //! Allow the user to make a selection.
    int SelectWindow::select( long forced )
    {
        int  return_value;
        bool stop = false;
        list< string >::iterator p = text->begin( );
        long current_index = 0;

        // If caller wants to force the display to a certain line, try to honor.
        if( forced != -1L ) top_line = forced;

        // Turn on the selection bar.
        show_bar = true;

        // Loop until user presses an invalid key.
        do {
            show( );
            switch( return_value = key( ) ) {
            case K_UP:
                if( p != text->begin( ) ) { --p; --current_index; }
                if( current_index < top_line ) top_line--;
                break;

            case K_DOWN:
                if( p != text->end( ) ) { ++p; ++current_index; }
                if( current_index >= top_line + height( ) ) top_line++;
                break;

            case K_PGUP:
                // Not implemented.
                break;

            case K_PGDN:
                // Not implemented.
                break;

            case K_CPGUP:
                p = text->begin( );
                current_index = 0;
                break;

            case K_CPGDN:
                p = text->end( );
                current_index = text->size( );
                break;

            default:
                stop = true;
                break;
            }
        } while( !stop );

        // Turn off the selection bar.
        show_bar = false;

        return( return_value );
    }

}
