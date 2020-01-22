/*! \file    ImageBuffer.cpp
    \brief   Interface to class ImageBuffer.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef IMAGEBUFFER_HPP
#define IMAGEBUFFER_HPP

#include <cstddef>
#include <string>
#include "scr.hpp"

namespace scr {

    //! Manages a rectangular region of the screen in memory.
    /*!
     * This class encapsulates a memory image of a rectangular region of the screen. It hides
     * the precise way in which characters and colors are stored and lets its clients treat
     * screen "images" in an abstract manner. This class provides methods for memory management
     * and basic text handling in images.
     */
    class ImageBuffer {
    public:
        ImageBuffer( int width, int height, int color = WHITE|REV_BLACK, char letter = ' ' );
        ImageBuffer( const ImageBuffer &other );
       ~ImageBuffer( );
        ImageBuffer &operator=( const ImageBuffer &other );

        void clear( int color = WHITE|REV_BLACK, char letter = ' ' );
        void copy( const char *source, int row, int column, std::size_t extent, int color );
        void copy( const std::string &source, int row, int column, std::size_t extent, int color)
            { copy( source.c_str( ), row, column, extent, color ); }
        void read( int row, int column );
        void write( int row, int column );
        void resize( int new_width, int new_height, int color = WHITE|REV_BLACK, char letter = ' ' );

        //! Returns the width of the image.
        int get_width( )
            { return width; }

        //! Returns the hight of the image.
        int get_height( )
            { return height; }

    private:
        int   width;
        int   height;
        char *buffer;
    };

}

#endif
