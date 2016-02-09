/*! \file    Window.cpp
    \brief   Implementation of class Simple_Window and Window.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstdlib>
#include <cstring>
#include <utility>

#include "Manager.hpp"
#include "scr.hpp"
#include "scrtools.hpp"
#include "Window.hpp"

using namespace std;

namespace scr {

    //! Computes upper left corner to center a window of given dimensions.
    /*!
     * This function is intended to be used before a call to open( ) or move( ) to compute an
     * appropriate position for a window that is to be centered. This function is not convenient
     * as a method because it has to be used before open( ) when the window dimensions are
     * undefined.
     *
     * \param width The total width of the window.
     * \param height The total height of the window.
     * \param row Pointer to location to receive the row coordinate.
     * \param column Pointer to location to receive the column coordinate.
     */
    void window_center_coordinates( int width, int height, int *row, int *column )
    {
        *row = 1 + ( number_of_rows( ) - height ) / 2;
        *column = 1 + ( number_of_columns( ) - width  ) / 2;
    }


    /*========================================*/
    /*           Class SimpleWindow           */
    /*========================================*/

    //! Default constructor
    /*!
     * The constructor sets the pointer to the window image and the pointer to the saved
     * background to nullptr. This ensures no problem when an undefined window is destroyed.
     */
    SimpleWindow::SimpleWindow( ) :
        is_hidden( false ),
        is_defined( false )
    { }


    //! Move constructor
    SimpleWindow::SimpleWindow( SimpleWindow &&existing )
    {
        // Move the storage areas.
        save_data = std::move( existing.save_data );
        hidden = std::move( existing.hidden );

        // Copy other members.
        total_row = existing.total_row;
        total_column = existing.total_column;
        total_width = existing.total_width;
        total_height = existing.total_height;
        window_row = existing.window_row;
        window_column = existing.window_column;
        window_width = existing.window_width;
        window_height = existing.window_height;
        window_color = existing.window_color;
        window_border_type = existing.window_border_type;
        window_border_color = existing.window_border_color;
        is_defined = existing.is_defined;
        is_hidden = existing.is_hidden;

        // Turn the lights out on the other object.
        existing.is_defined = false;
    }


    //! Destructor
    /*!
     * The destructor does nothing if the window has never been opened. It hides the window
     * first to ensure that the background material is properly restored.
     */
    SimpleWindow::~SimpleWindow( )
    {
        if( !is_defined ) return;
        hide( );
    }


    //! Open a window.
    /*!
     * This method defines all the window's parameters and draws the window on the screen. If a
     * window is already defined an attempt to redefine it using this method fails.
     *
     * \param row The coordinate of the top of the window image.
     * \param column The coordinate of the left edge of the window image.
     * \param width Total width of the window, including border.
     * \param height Total height of the window, including border.
     * \param color The color attribute to use for the interior of the window.
     * \param border_type Box type for the window border.
     * \param border_color The color attribute to use for the border.
     *
     * \return true if successful; false if the window is already defined.
     * \throws std::bad_alloc if resources for storing the window image can't be allocated.
     * \todo Sanity checking on the parameters is needed.
     */
    bool SimpleWindow::open(
        int row,
        int column,
        int width,
        int height,
        int color,
        BoxType border_type,
        int border_color
    )
    {
        if( is_defined ) return false;

        // Try to allocate memory for background and image.
        std::size_t size = 2 * width * height;

        // Allocate memory carefully for exception safety.
        std::unique_ptr<char[]> new_save( new char[size] );
        std::unique_ptr<char[]> new_hidden( new char[size] );
        save_data = std::move( new_save );
        hidden = std::move( new_hidden );

        // Set members to initial values.
        total_row = window_row = row;
        total_column = window_column = column;
        total_width = window_width = width;
        total_height = window_height = height;
        window_color = color;
        window_border_type = border_type;
        window_border_color = ( border_color == WINDOW_COLOR ) ? window_color : border_color;

        // Draw the window on the screen.
        read( total_row, total_column, total_width, total_height, save_data.get( ) );
        scr::clear( total_row, total_column, total_width, total_height, window_color );
        if( window_border_type != NO_BORDER ) {
            window_row++;
            window_column++;
            window_width  -= 2;
            window_height -= 2;
            draw_box( total_row,
                      total_column,
                      total_width,
                      total_height,
                      window_border_type,
                      window_border_color );
        }
        is_defined = true;
        is_hidden  = false;

        return true;
    }


    //! Draw a new border for an existing window.
    /*!
     * This method can be used to change the border type for a window. It can be used on a
     * window while it is hidden. It works by first showing the window, operating on its image,
     * and then hiding the window again. This does not cause any flicker because there is no
     * refresh( ) call between the show( ) and the hide( ) operations. If the window is
     * undefined this method has no effect.
     *
     * \param border_type The new border type for the window.
     * \param attribute The color attribute to use for the new border.
     */
    void SimpleWindow::redraw_border( BoxType border_type, int attribute )
    {
        if( !is_defined ) return;

        // Disregard attempts to turn border on or off.
        if( border_type == NO_BORDER || window_border_type == NO_BORDER ) return;

        bool was_hidden = is_hidden;
        show( );

        if( attribute == WINDOW_COLOR ) attribute = window_color;
        draw_box( total_row, total_column, total_width, total_height, border_type, attribute );
        window_border_type  = border_type;
        window_border_color = attribute;

        if( was_hidden ) hide( );
    }


    //! Hides a window.
    /*!
     * This method copies the screen image into memory and restores the background. If the
     * window is already hidden, there is no effect.
     */
    void SimpleWindow::hide( )
    {
        if( !is_defined ) return;

        if( !is_hidden ) {
            read( total_row, total_column, total_width, total_height, hidden.get( ) );
            write( total_row, total_column, total_width, total_height, save_data.get( ) );
            is_hidden = true;
        }
    }


    //! Shows a window.
    /*!
     * This method copies the background into memory and restores the window's image. If the
     * window is already showing, there is no effect.
     */
    void SimpleWindow::show( )
    {
        if( !is_defined ) return;

        if( is_hidden ) {
            read( total_row, total_column, total_width, total_height, save_data.get( ) );
            write( total_row, total_column, total_width, total_height, hidden.get( ) );
            is_hidden = false;
        }
    }


    //! Moves a window.
    /*!
     * This method moves a hidden or showing window to new coordinates.
     *
     * \param new_row The row coordinate of the window's new location.
     * \param new_column The column coordinate of the window's new location.
     */
    void SimpleWindow::move( int new_row, int new_column )
    {
        if( !is_defined ) return;

        // Do the actual move. For hidden windows, just adjust records.
        if( is_hidden ) {
            total_row = new_row;
            total_column = new_column;
        }
        else {
            hide( );
            total_row = new_row;
            total_column = new_column;
            show( );
        }

        // Adjust window_row and window_column.
        if( window_border_type != NO_BORDER ) {
            window_row = total_row + 1;
            window_column = total_column + 1;
        }
        else {
            window_row = total_row;
            window_column = total_column;
        }
    }

    //! Clears a window.
    /*!
     * This method resets a window image to its initial state. This includes the border as well
     * as the printable area.
     */
    void SimpleWindow::clear( )
    {
        if( !is_defined ) return;

        bool was_hidden = is_hidden;

        show( );
        scr::clear( window_row, window_column, window_width, window_height, window_color );
        redraw_border( window_border_type, window_border_color );

        if( was_hidden ) hide( );
    }


    //! Closes a window.
    /*!
     * This method cleans up a window and then re-initializes it.
     */
    void SimpleWindow::close( )
    {
        if( !is_defined ) return;
        hide( );
        save_data = nullptr;
        hidden = nullptr;
        is_hidden = false;
        is_defined = false;
    }

    /*================================*/
    /*          Class Window          */
    /*================================*/

    //! Constructor
    /*!
     * This constructor initializes a window's internal records and registers the window with
     * the given manager. The window's image is initially blank. The window position and size is
     * defined in terms of the printable area. The window manager adds the border (if any).
     *
     * \param my_manager Pointer to the window manager that controls this window.
     * \param row Initial row coordinate of window's upper left corner.
     * \param column Initial column coordinate of window's upper left corner.
     * \param width Initial window width.
     * \param height Initial window height.
     */
    Window::Window( Manager *my_manager, int row, int column, int width, int height ) :
        my_manager( my_manager ), image( width, height )
    {
        is_registered = false;

        if( my_manager->register_window( this, row, column, width, height ) )
            is_registered = true;
    }


    //! Destructor
    Window::~Window( )
    {
        // Don't do anything if registration failed.
        if( !is_registered ) return;

        // Remove this window from the display list and fix the display.
        my_manager->deregister_window( this );
        my_manager->update_display( );
    }


    //! Process a keystroke.
    /*!
     * This function performs window specific processing on the given keystroke. Derived classes
     * override this function to provide their functionality.
     *
     * \param key_code the code as returned by Key( ).
     *
     * \return true if the keystroke was processed; false otherwise. If false is returned The
     * window manager will then process the key (if possible) in that case. Note that this
     * function is free to modify the key code it is given and then return false to "trick" the
     * window manager into also processing the key.
     */
    bool Window::process_keystroke( int & )
    {
        return false;
    }


    //! Returns the window's image.
    /*!
     * This function computes (if necessary) the image of the window. The image format is
     * alternating character ASCII codes and color attributes. The color attribute for a
     * particular character immediately follows that character. The data returned by this
     * function is not null terminated. The size of the data returned is exactly 2 * width *
     * height. The memory block used may or may not be dynamically allocated; the caller should
     * not attempt to free this memory. The data is invalidated after a call to any other Window
     * method.
     */
    ImageBuffer *Window::get_image( )
    {
        return &image;
    }


    //! Return the cursor row coordinate relative to the printable area.
    int Window::cursor_row( )
    {
        return 1;
    }


    //! Return the cursor column coordinate relative to the printable area.
    int Window::cursor_column( )
    {
        return 1;
    }


    //! Reposition the window.
    /*!
     * This method is called by the manager when the window is moved. It is also called when the
     * window is first registered to set its initial location. The window can decline the
     * position specified here. In that case the window is not moved (or registration fails).
     *
     * \param new_row The new row position of the upper left corner of the border.
     * \param new_column The new column position of the upper left corner of the border.
     * \return true if the new position is accepted; false otherwise.
     */
    bool Window::reposition( int, int )
    {
        return true;
    }


    //! Resize the window.
    /*!
     * This method is called by the manager when the window is being resized. It is also called
     * when the window is registered to set its initial size. The window can decline the size
     * specification used here. In that case the window's size is not modified (or registration
     * fails).
     *
     * \param new_width The new width of the printable area.
     * \param new_height The new height of the printable area.
     * \return true if the new size accepted; false otherwise.
     */
    bool Window::resize( int new_width, int new_height)
    {
        image.resize( new_width, new_height );
        return true ;
    }

}
