/*! \file    Shadow.hpp
    \brief   Interface to class Shadow
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef SHADOW_HPP
#define SHADOW_HPP

namespace scr {

    //! A representation of a "shadow" on the screen.
    /*!
     * Objects of this class are rectangular shadows on the text screen. It is assumed that they
     * will be used with windows and menus, etc, to provide a snazzy look. They work by changing
     * the attribute of the region to be shaded to bright black on black. This class assumes
     * that the region being shaded does not change while the shadow is displayed.
     */
    class Shadow {
    private:
        char *background;    //!< Points at true background. (NULL if not open)
        int   top_row;       //!< Row coordinate of shadow's upper left corner.
        int   left_column;   //!< Column coordinate of shadow's upper left corner.
        int   shadow_width;  //!< Width of shadow.
        int   shadow_height; //!< Height of shadow.

    public:
        Shadow( ) { background = 0; }
       ~Shadow( ) { close( ); }

        void open( int row, int column, int width, int height );
        void close( );
    };
}

#endif

