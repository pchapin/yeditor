/*! \file    ListWindow.cpp
    \brief   Window class for displaying a list of strings.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "ListWindow.hpp"
#include "scr.hpp"
#include "scrtools.hpp"

using namespace std;

namespace scr {

    ImageBuffer *ListWindow::get_image( )
    {
        int width, height;

        width  = image.get_width( );
        height = image.get_height( );
        image.clear( );

        // Get ready to back up over the list of Strings. We want to display only the last part.
        list<string>::reverse_iterator current_text( managed_list.rbegin( ) );

        // Get ready to back up over the display area.
        int window_line = height;

        // Keep doing this until we run out of lines or display space.
        bool stop = false;

        while( ( current_text != managed_list.rend( ) ) && !stop ) {
            const char *text = (*current_text).c_str( );

            if( window_line < 1 ) break;

            // Display the tail.
            int tail_length = strlen( text ) % width;
            int text_count  = strlen( text ) - tail_length;

            if( tail_length != 0 ) {
                image.copy( &text[strlen( text ) - tail_length], window_line, 1, width, WHITE|REV_BLACK);
                window_line--;
                if( window_line < 1 ) break;
            }

            while( text_count > 0 && !stop ) {
                image.copy( &text[text_count - width], window_line, 1, width, WHITE|REV_BLACK);
                window_line--;
                if( window_line < 1 ) stop = true;
                text_count -= width;
            }
            ++current_text;
        }
        return &image;
    }


    //! Appends a new line to the end of the text in a list window.
    /*!
     * If the number of items currently in the list is at its maximum, the first item is removed
     * from the list.
     *
     * \param new_line The string to add to the end of the list.
     */
    void ListWindow::insert( const char *new_line )
    {
        std::string new_item( new_line );
        managed_list.push_back( new_item );
        if( managed_list.size( ) > max_items ) managed_list.pop_front( );
    }

}
