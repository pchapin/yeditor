/*! \file    DisplayWindow.cpp
    \brief   Implementation of class DisplayWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <climits>
#include <cstring>
#include "DisplayWindow.hpp"
#include "scr.hpp"

using namespace std;

namespace scr {

    //! Associate a list<string> with a DisplayWindow.
    /*!
     * This method must be called before open( ). The initial position in the list<string> and
     * a title string can be registered with the DisplayWindow object.
     *
     * Notice that this class provides no way to changed the displayed column number. The show(
     * ) method handles the column number correctly, however. Thus derived classes (or the
     * caller to set( )) can use this ability to good effect.
     */
    void DisplayWindow::set(
        const char  *input_title,
        std::list< std::string > *input_text,
        long         start_line,
        int          start_column)
    {
        top_line    = start_line;
        left_column = start_column;
        title       = input_title;
        text        = input_text;
    }


    //! Display the window for the first time.
    /*!
     * This method also is responsible for displaying the title of the window inside the top
     * border. It is expected that DisplayWindows will be built using border types of DBL_LINE
     * or SNGL_LINE. The other types will not cause a problem, however. The parameters are self
     * explanatory.
     */
    bool DisplayWindow::open(
        int row,    int column,
        int width,  int height,
        int color,  int status_color,
        BoxType border, int border_color)
    {
        struct BorderPlug {
            BoxType     border_type;
            const char *left_plug;
            const char *right_plug;
            int         size;
        };
        static const BorderPlug generic = { BLANK_BOX, " ", " ", 1 };
        static const BorderPlug plugs[] = {
            { DOUBLE_LINE, "\xB5 ", " \xC6", 2 },
            { SINGLE_LINE, "\xB4 ", " \xC3", 2 },
            { ASCII,       "| ",    " |",    2 }
        };
        const int number_of_plugs   = 3;

        bool return_value = false;
        int  left_column;
        int  right_column;

        // Make sure the border is acceptable.
        if( border == NO_BORDER ) border = BLANK_BOX;

        // Attempt to open the primary window.
        if( SimpleWindow::open(row, column, width, height, color, border, border_color ) ) {

            // Locate appropriate border plug information.
            const BorderPlug *active_plug;
            for( active_plug = plugs;
                 active_plug - plugs < number_of_plugs && active_plug->border_type != border;
                 active_plug++ ) /* null */ ;
            if( active_plug - plugs >= number_of_plugs ) active_plug = &generic;

            // Display the title on the top border.
            left_column  =
                SimpleWindow::column( ) +
               (SimpleWindow::width( ) - static_cast<int>( strlen( title ) ) )/2 -
                active_plug->size;

            right_column = left_column + static_cast<int>( strlen( title ) ) + active_plug->size;

            print_text( SimpleWindow::row( ) - 1,
                        left_column,
                        active_plug->size,
                        active_plug->left_plug );

            print_text( SimpleWindow::row( ) - 1,
                        right_column,
                        active_plug->size,
                        active_plug->right_plug );

            print_text( SimpleWindow::row( ) - 1,
                        left_column + active_plug->size,
                        strlen(title),
                        (char *)title );

            // Fill primary window with text.
            show( );
            return_value = true;
        }
        return return_value;
    }


    //! Fill the window with text from the previously provided list of strings.
    /*!
     * This method takes top_line as a request for the desired first line to display. It
     * verifies that this request will cause material from the list< string > to be seen. If
     * top_line is out of range, it is adjusted. Thus functions that use show( ), such as
     * display( ) below, do not need to do these checks.
     *
     * This method attempts to fill the window quickly and without flicker. The window is NOT
     * cleared and then refilled (which causes flicker); rather, each line is redrawn with
     * trailing spaces if required.
     *
     * Note that this method correctly handles left_column.
     *
     * /todo The flicker control is outdated. Current versions of scr do not require this.
     *
     */
    void DisplayWindow::show( )
    {
        list< string >::iterator line_pointer; // Points at a line from the list.

        char   buffer[80+1];   // Holds text to be displayed in the window.
        char  *buffer_pointer; // Points into buffer.
        int    i;              // Loops over all rows in the window.

        // Force primary window on screen. If it already is, this has no effect.
        SimpleWindow::show( );

        // Be sure that we are going to show some of the string list.
        long max_line = text->size( ) - height( );
        if( max_line < 0 )        max_line = 0L;
        if( top_line > max_line ) top_line = max_line;
        if( top_line < 0 )        top_line = 0L;

        // Move current point of list to line number top_line.
        line_pointer = text->begin( );
        for( i = 0; i < top_line; ++i ) ++line_pointer;

        // Loop until all lines of the window are filled or until there's no text left.
        for( i = row( );
             i < row( ) + height( ) && line_pointer != text->end( );
             ++i, ++line_pointer ) {

            // Set text_pointer to the first character to be displayed in window.
            const char *text_pointer = line_pointer->c_str( );
            while( text_pointer - line_pointer->c_str( ) < left_column  && *text_pointer != '\0' ) {
                text_pointer++;
            }

            // Fill buffer with spaces and null terminate.
            memset( buffer, ' ', 80 );
            buffer[80] = '\0';

            // Fill buffer with material from the current line.
            buffer_pointer = buffer;
            while( *text_pointer  &&  buffer_pointer - buffer < 80 )
                *buffer_pointer++ = *text_pointer++;

            // Print the line into the window.
            print( i, column( ), width( ), color( ), buffer );
        }

        // If the entire screen was not filled, clear the lower section.
        if( i < row( ) + height( ) )
            scr::clear( i, column( ), width( ), height( ) - ( i-row( ) ), color( ) );
    }


    //! Browse the text in the previously provided list of strings.
    /*!
     * This method responds to a certain "normal" command set. When the user presses a key not
     * in the set, the function returns with that key code. The object is not hidden or closed
     * by this function. Thus the calling code could handle the extra keystrokes and call this
     * method again with no apparent break in the display.
     *
     * Additionally, a derived class could override this method to obtain more exotic effects.
     * The derived class has access to several important protected variables.
     *
     * The caller can try to force the display to start from a position in the list which is
     * different the current top line. This is NOT done by default.
     */
    int DisplayWindow::display( long forced )
    {
        int  return_value;
        bool stop = false;

        // If caller wants to force the display to a certail line, try to honor.
        if( forced != -1L ) top_line = forced;

        // Loop until user presses an invalid key.
        do {
            show( );
            switch( return_value = key( ) ) {
                case K_UP:    top_line--;            break;
                case K_DOWN:  top_line++;            break;
                case K_PGUP:  top_line -= height( ); break;
                case K_PGDN:  top_line += height( ); break;
                case K_CPGUP: top_line = 0L;         break;
                case K_CPGDN: top_line = LONG_MAX;   break;   // Force to end.
                default:      stop = true;           break;
            }
        } while( !stop );

        return return_value;
    }

}
