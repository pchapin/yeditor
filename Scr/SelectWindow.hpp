/*! \file    SelectWindow.hpp
    \brief   Interface to class SelectWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef SELECTWINDOW_HPP
#define SELECTWINDOW_HPP

#include "DisplayWindow.hpp"

namespace scr {

    //! Allow the user to select an item from a list.
    /*!
     * This class behaves like DisplayWindow except that it also highlights an entry in the list
     * of strings and allows the user to move that highlighted entry around to select a
     * particular item.
     */
    class SelectWindow : public DisplayWindow {
    private:
        int  highlight_color;
        bool show_bar;

    public:
        bool open(
            int row,
            int column,
            int width,
            int height,
            int color,
            int status_color,
            BoxType border,
            int border_color = WINDOW_COLOR);

        void show( );
        int  select( long forced = -1L );
    };
}

#endif

