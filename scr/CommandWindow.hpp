/*! \file    CommandWindow.hpp
    \brief   Interface to class CommandWindow
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef COMMANDWINDOW_HPP
#define COMMANDWINDOW_HPP

#include <string>

#include "ImageBuffer.hpp"
#include "Manager.hpp"
#include "scr.hpp"

namespace scr {

    //! Window for accepting command lines.
    /*!
     * This class allows a command line to be entered. It supports basic editing features like
     * text insertion and arrow key navigation.
     */
    class CommandWindow : public Window {

    private:
        std::string prompt;

    protected:
        std::string command_text;
        std::string::size_type cursor_offset;

    public:
        CommandWindow( Manager *my_manager, int row, int column, int width, int height ) :
            Window( my_manager, row, column, width, height ), cursor_offset( 0 )
            { }

        //! Set the prompt.
        void set_prompt( const char * );

        bool process_keystroke( int &key_code );

        //! Returns the CommandWindow's on-screen image.
        ImageBuffer *get_image( );

        //! Returns the command cursor's row position.
        int cursor_row( );

        //! Returns the command cursor's column position.
        int cursor_column( );
    };

}

#endif
