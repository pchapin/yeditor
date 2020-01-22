/*! \file    DisplayWindow.hpp
    \brief   Interface to class DisplayWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef DISPLAYWINDOW_HPP
#define DISPLAYWINDOW_HPP

#include <list>
#include <string>
#include "Window.hpp"

namespace scr {

    /*!
     * Objects from this class are able to display the text in a list<string> to the user. The
     * user can scroll through the text easily. Derived classes can redefine the Display( )
     * function for other special purposes. show( ) has been implemented in a manner to simplify
     * the checking that needs to be done in display( ).
     *
     * Additionally, most of the data members are protected. This allows derived classes to
     * manipulate them freely (including the list<string> itself). Notice that a pointer to a
     * list<string> is maintained. Thus, the actual list<string> being displayed could be
     * modified by external forces. DisplayWindows could be used for showing extensive,
     * scrollable status information.
     *
     * The list<string> which is being display is not touched by this class. The DisplayWindow
     * shows a range of line numbers. If text is inserted above the window, the contents of the
     * window will scroll down (when show( ) or display( ) are called).
     */
    class DisplayWindow : public SimpleWindow {

    protected:
        const char  *title;        //!< Name of text.
        long         top_line;     //!< Line number at top of window.
        int          left_column;  //!< Column number at left of window.
        std::list< std::string > *text;  //!< Points to the text to display.

    public:
        void set(
            const char  *input_title,
            std::list< std::string > *input_text,
            long         start_line = 0L,
            int          start_column = 0
        );

        bool open(
            int row,
            int column,
            int width,
            int height,
            int color,
            int status_color,
            BoxType border,
            int border_color = WINDOW_COLOR
        );

        void show( );
        int  display( long forced = -1L );
    };

}

#endif
