/*! \file    InputWindow.cpp
    \brief   Interface to class InputWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef INPUTWINDOW_HPP
#define INPUTWINDOW_HPP

#include "Shadow.hpp"
#include "Window.hpp"

namespace scr {

    //! Describes the characteristics of an Input_Window.
    struct InputWindowDescriptor {
        BoxType border_type;           //!< Border to use for the input window.
        int     primary_attribute;     //!< Color of border, prompt, etc.
        int     secondary_attribute;   //!< Color of text entered by user.
    };

    //! A window where the user can input a line of text.
    /*!
     * This class is intended to be used for accepting a command line or similar input from the
     * user. It provides a number of convenience services to simplify the entry of such input.
     */
    class InputWindow : public SimpleWindow {
    private:
        Shadow the_shadow;         //!< The shadow for the window.
        const char *prompt_text;   //!< Text of prompt.
        char  *buffer;             //!< Buffer used for user's input.
        int    buffer_size;        //!< Number of characters user can type.
        int    current_level;      //!< Index into descriptor array specifying window attributes.

    public:
        static void set_descriptors( InputWindowDescriptor *use );

        InputWindow( );
        InputWindow( char *prompt_text, char *buffer, int buffer_size, int level );

        void set( char *prompt_text, char *buffer, int buffer_size, int level );
        bool open( int row = 0, int column = 0 );
        void hide( );
        int  show( );
        void close( );
    };
}

#endif
