/*! \file    Window.hpp
    \brief   Interface to class Simple_Window and Window.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include "ImageBuffer.hpp"
#include "scr.hpp"

namespace scr {

    const int WINDOW_COLOR = 0x7FFF;

    //! Base class for unmanaged windows.
    /*!
     * A window of this type is a simple rectangular region on the screen with an optional
     * border. SimpleWindows save their background and can be drawn with a shadow but they do
     * not participate in a collection of windows under the control of a window manager.
     * SimpleWindows are good for applications with simple screen handling needs or for showing
     * material quickly as with, for example, message boxes.
     */
    class SimpleWindow {
    private:
        int total_row;      //!< Top row of window (including border).
        int total_column;   //!< Left column of window (including border).
        int total_width;    //!< Total width (including border).
        int total_height;   //!< Total height (including border).

        int window_row;     //!< Top row of printable area.
        int window_column;  //!< Left column of printable area.
        int window_width;   //!< Width (without border).
        int window_height;  //!< Height (without border).
        int window_color;   //!< Color attribute of window interior.
        BoxType window_border_type;  //!< Border type.
        int window_border_color;     //!< Color attribute of border.

        std::unique_ptr<char[]> hidden;     //!< Window image when a window is hidden.
        std::unique_ptr<char[]> save_data;  //!< Saved background material.

    protected:
        bool is_defined;    //!< True when open( ) has been used without error.
        bool is_hidden;     //!< True when window is in a hidden state.

    public:
        SimpleWindow( );
        SimpleWindow( const SimpleWindow &existing ) = delete;
        SimpleWindow &operator=( const SimpleWindow &existing ) = delete;
        SimpleWindow( SimpleWindow &&existing );
       ~SimpleWindow( );

        bool open(
            int row,
            int column,
            int width,
            int height,
            int color,
            BoxType border_type,
            int border_color = WINDOW_COLOR
        );

        void redraw_border( BoxType border_type, int attribute = WINDOW_COLOR );
        void hide( );
        void show( );
        void move( int new_row, int new_column );
        void clear( );
        void close( );

        // Accessor methods.
        int row( ) const { return window_row;     }
        int column( ) const { return window_column;  }
        int width( ) const { return window_width;   }
        int height( ) const { return window_height;  }
        int color( ) const { return window_color;   }
        BoxType border_type( ) const { return window_border_type;  }
        int border_color( ) const { return window_border_color; }
    };

    void window_center_coordinates( int width, int height, int *row, int *column );



    class Manager;

    //! Base class for managed windows.
    /*!
     * Every window in the program is derived from this class. Each specific window type
     * overrides the virtual functions in such a way as to specialize the behavior of that
     * window. Each window type *must* provide a function that returns the window's image.
     */
    class Window {
    private:
        bool is_registered;

    protected:
        Manager    *my_manager;  //!< Pointer to the manager that is managing this window.
        ImageBuffer image;       //!< The image of what is currently in the window.

    public:
        Window( Manager *my_manager, int row, int column, int width, int height );
        virtual ~Window( );

        virtual ImageBuffer *get_image( );
        virtual bool process_keystroke( int &key_code );
        virtual int  cursor_row( );
        virtual int  cursor_column( );
        virtual bool reposition( int new_row, int new_column );
        virtual bool resize( int new_width, int new_height );
    };

}

#endif
