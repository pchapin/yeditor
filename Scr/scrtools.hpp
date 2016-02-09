/*! \file    scrtools.hpp
    \brief   Public interface to screen tools package.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef SCRTOOLS_HPP
#define SCRTOOLS_HPP

namespace scr {

    void center  ( int row, int column, int width, int attribute, const char *text );
    void draw_box( int row, int column, int width, int height, BoxType the_type, int attribute );
    int  gets    ( int row, int column, int length, int attribute, char *buffer );

}

#endif
