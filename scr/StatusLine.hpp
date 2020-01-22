/*! \file    StatusLine.hpp
    \brief   Interface to class StatusLine
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef STATUSLINE_HPP
#define STATUSLINE_HPP

#include "Window.hpp"

namespace scr {

    //! An encapsulation of a status line.
    /*!
     * This class allows for easy handling of one line status flags in a program. The function
     * which creates the string that is printed in the status line is provided by the consumer
     * and registered with the StatusLine object using set( ). This function can be changed on
     * the fly without the use of close( ). This allows easy implementation of several apparant
     * status lines with one StatusLine object.
     */
    class StatusLine : public SimpleWindow {
    private:
        const char *( *make_line )( );

    public:
        StatusLine( );

        void set( const char *( *function )( ) )
            { make_line = function; }

        bool open( int row, int column, int width, int attribute );
        void show( );
    };
}

#endif
