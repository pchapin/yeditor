/*! \file    MessageWindow.hpp
    \brief   Public interface to class MessageWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef M_WINDOW_HPP
#define M_WINDOW_HPP

#include "Shadow.hpp"
#include "Window.hpp"

namespace scr {

    //! An array of these descriptors define the different types of windows used.
    struct MessageWindowDescriptor {
        int          border_attribute;   //!< Color of border.
        enum BoxType border_type;        //!< Type of border.
        int          interior_attribute; //!< Color of message text.
        const char  *header_text;        //!< Header. NULL means no header.
        int          header_attribute;   //!< Color of header text.
        int          exit_char;          //!< Character to escape window (see below).
    };

    //! A window for displaying simple messages.
    /*!
     * Objects of this class give the consumer a simple way to display one line messages to the
     * program's user. The messages are displayed in a window in the center (by default) of the
     * screen. They are intended to be used for error messages or other simple messages that do
     * not require any extended user interaction.
     */
    class MessageWindow : public SimpleWindow {
    private:
        Shadow      the_shadow;      //!< Shadow for the message window.
        const char *current_text;    //!< Points the current message text.
        int         current_level;   //!< Index into descriptor array.

    public:
        static void set_descriptors( const struct MessageWindowDescriptor *use );

        MessageWindow( );                              // Use set( ) and open( ).
        MessageWindow( const char *text, int level );  // Window appears at once.
       ~MessageWindow( );                              // Makes sure shadow is gone.

        void set( const char *text, int level );       // Set text and style.
        int  open( int row = 0, int column = 0 );      // Specify position.

        int  show( );      // Display window and return keystroke.
        void hide( );
        void close( );
    };

    // This symbol is used as an exit_char to mean any key is to exit the window.
    const int MESSAGE_WINDOW_ANY = -1;

    // This symbol is used as an exit_char to mean that there is no waiting for a keypress.
    const int MESSAGE_WINDOW_NOWAIT = '\0';

    // Suggested names for descriptor levels.
    const int MESSAGE_WINDOW_MESSAGE    = 0;
    const int MESSAGE_WINDOW_PROMPT     = 1;
    const int MESSAGE_WINDOW_WARNING    = 2;
    const int MESSAGE_WINDOW_ERROR      = 3;
    const int MESSAGE_WINDOW_INT_ERROR  = 4;
}

#endif
