/*! \file    Manager.hpp
    \brief   Window manager definition file.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <list>
#include "Window.hpp"

namespace scr {
  
    //! Manages windows derived from class Window.
    /*!
     * A Manager object manages a list of windows that are shown on the display device. It
     * allows windows of various types to be overlapped and moved around. A Manager object
     * handles "system" keystrokes and passes all other keystrokes to the process_keystroke( )
     * function of the foreground window for handling by that window.
     */
    class Manager {
    public:

        //! Associates key handling functions with particular key codes.
        /*!
         * Applications can register special key handling functions with the Manager, and those
         * functions get executed when specific keys are pressed. Such special keys are thus
         * handled separately from any window. Even the system-mode keys normally handled by the
         * manager can be redefined in this way.
         */
        struct SpecialKey {
            int key_code;              //!< Key code for special key.
            void ( *key_function )( ); //!< Function to execute for that key.
        };

    private:

        //! Holds everything about a window that the Manager needs to know.
        struct WindowInformation {
            Window *the_window;       //<! Points to the actual window.
            int     row_position;     //<! Coords of window's printable area UL corner.
            int     column_position;  //<! Coords of window's printable area UL corner.
            int     width;            //<! Width of the Window's printable area.
            int     height;           //<! Height of the Window's printable area.

            // This is only necessary because OW doesn't do lazy instantiation.
            bool operator==( const WindowInformation &other );
            bool operator< ( const WindowInformation &other );
        };

        //! List of all the windows we are managing.
        std::list< WindowInformation > the_windows;

        //! True when keystrokes are to be interpreted as system control keys.
        bool system_mode;

        //! Application specific special keys that are handled globally.
        SpecialKey *key_array;  //!< Pointer to array of special key definitions.
        int key_count;          //!< Number of special keys in the Key_Array.

    public:

        Manager( );
       ~Manager( );

        //! Register an array of Special_Key handlers.
        /*!
         * Use a count of zero to turn off previously registered special key handling.
         */
        void set_special_keys( SpecialKey *key_definitions, int count )
            { key_array = key_definitions; key_count = count; }

        // The following methods are for use by Window classes.
        bool register_window( Window *new_window, int row, int column, int width, int height );
        void deregister_window( const Window *old_window );
        void get_size( Window *w, int &width, int &height );

        // The following methods are for use by the application as a whole.
        void update_display( );
        void input_loop( );
        void swap_top( );
    };

}

#endif
