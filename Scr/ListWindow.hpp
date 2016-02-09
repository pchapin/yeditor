/*! \file    ListWindow.hpp
    \brief   Interface to window class for displaying a list of strings.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef LISTWINDOW_HPP
#define LISTWINDOW_HPP

#include <list>
#include <string>

#include "ImageBuffer.hpp"
#include "Manager.hpp"

namespace scr {

    //! Manages a read-only list of strings.
    /*!
     * Class ListWindow displays a list of strings and allows the user to scroll through this
     * list. It provides the user with no ability to edit the list (directly, at least).
     * ListWindow objects have a maximum number of strings they can handle. If another string is
     * added beyond that maximum number, the first string is deleted. This allows ListWindows to
     * be used to scroll information arbitrarily.
     */
    class ListWindow : public Window {
    private:
        std::list< std::string > managed_list;  //!< Strings to display.
        std::size_t max_items;                  //!< Maximum number of strings in the list.

    public:
        ListWindow( Manager *my_manager, std::size_t max, int row, int column, int width, int height ) :
            Window( my_manager, row, column, width, height ), max_items( max )
            { }

        virtual ImageBuffer *get_image( );
        void insert( const char *new_line );
    };
}

#endif

