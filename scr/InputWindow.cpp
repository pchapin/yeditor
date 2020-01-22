/*! \file    InputWindow.cpp
    \brief   Implementation of class InputWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstring>

#include "InputWindow.hpp"
#include "scr.hpp"
#include "scrtools.hpp"

static scr::InputWindowDescriptor *descriptor_list = NULL;

namespace scr {

    //! Set the descriptor list.
    /*!
     * This method allows the caller to set up an array of InputWindowDescriptor objects for use
     * by the application. Each InputWindow has an associated 'level' that refers to a
     * particular entry in this array. The appearance of the window is then controlled by the
     * corresponding descriptor. If this list is changed any exiting InputWindows need to be
     * closed and then reopened for the change to take effect.
     *
     *  \bug It is the user's responsibility to ensure that there are enough entries on the
     *  given list to support the highest level used in the program.
     *
     *  \param use A pointer to the first element of an array of InputWindowDescriptors.
     */
    void InputWindow::set_descriptors( InputWindowDescriptor *use )
    {
        descriptor_list = use;
    }


    //! Constructor.
    /*!
     * The default constructor builds an uninitailized InputWindow. The set( ) method can be
     * later used to configure the window.
     */
    InputWindow::InputWindow( )
    {
        static char dummy_buffer[1 + 1];

        prompt_text   = "Error: Opened a scr::InputWindow without Set( ): ";
        buffer        = dummy_buffer;
        buffer_size   = 1;
        current_level = 0;
    }


    //! Constructor.
    /*!
     * This constructor initializes the InputWindow using the given parameters. The window is
     * automatically opened.
     *
     * \param prompt_text The text to be used for prompting the user.
     * \param buffer A pointer to the buffer when the user's input will be stored.
     * \param buffer_size The size of the provided buffer (not including space for the null
     * character).
     * \param level The 'level' of the InputWindow. An index into the previously provided
     * descriptor array.
     */
    InputWindow::InputWindow( char *prompt_text, char *buffer, int buffer_size, int level )
    {
        this->prompt_text   = prompt_text;
        this->buffer        = buffer;
        this->buffer_size   = buffer_size;
        this->current_level = level;
        open( );
    }


    //! Configure an InputWindow.
    /*!
     * This method configures an existing InputWindow in a manner similar to the constructor.
     * Use this method after default constructing an uninitialized InputWindow. This method can
     * also be used to reconfigure an existing InputWindow object. Note that this method does
     * not automatically open the window.
     *
     * \param prompt_text The text to be used for prompting the user.
     * \param buffer A pointer to the buffer when the user's input will be stored.
     * \param buffer_size The size of the provided buffer (not including space for the null
     * character).
     * \param level The 'level' of the InputWindow. An index into the previously provided
     * descriptor array.
     */
    void InputWindow::set( char *prompt_text, char *buffer, int buffer_size, int level )
    {
        this->prompt_text   = prompt_text;
        this->buffer        = buffer;
        this->buffer_size   = buffer_size;
        this->current_level = level;
    }


    #define ACTIVE_DESC descriptor_list[current_level]

    //! Opens an InputWindow.
    /*!
     * This method displays the InputWindow on the screen and lets the user input data. When the
     * user has finished inputing text the window is automatically closed.
     *
     * \param row The row coordinate for the top of the window. A value of zero means to
     * center the window vertically.
     * \param column The column coordinate for the left edge of the window. A value of zero
     * means to center the window horizontally.
     *
     * \return True if successful; false otherwise.
     */
    bool InputWindow::open( int row, int column )
    {
        bool return_value;
        int  current_row, current_column;

        // Make sure the window has a border.
        if( ACTIVE_DESC.border_type == NO_BORDER )
            ACTIVE_DESC.border_type = BLANK_BOX;

        // Compute size of window.
        int width = static_cast<int>( std::strlen( prompt_text ) ) + buffer_size;
        width += 4;
        if( width > 80 ) width = 80;

        // Compute starting row and column for the window.
        int center_row, center_column;
        window_center_coordinates( width, 3, &center_row, &center_column );
        if( row == 0 ) row = center_row;
        if( column == 0 ) column = center_column;

        // Make the shadow.
        the_shadow.open( row + 1, column + 2, width, 3 );

        // Try to open the window itself.
        if ( ( return_value = SimpleWindow::open(
                row, column, width, 3,
                ACTIVE_DESC.primary_attribute,
                ACTIVE_DESC.border_type,
                ACTIVE_DESC.primary_attribute ) ) == true ) {

            // Display the prompt and learn about the current cursor location.
            print(
                SimpleWindow::row( ),
                SimpleWindow::column( ) + 1,
                SimpleWindow::width( ) - 2,
                SimpleWindow::color( ),
                prompt_text,
                SimpleWindow::color( )
            );
            get_cursor_position( &current_row, &current_column );

            // Get the data from the user.
            gets(
                SimpleWindow::row( ),
                SimpleWindow::column( ) + static_cast<int>( std::strlen( prompt_text ) ) + 1,
                buffer_size,
                ACTIVE_DESC.secondary_attribute,
                buffer
            );

            // Restore the cursor and hide the window.
            set_cursor_position( current_row, current_column );
            hide( );
        }
        return return_value;
    }


    //! Display an already existing InputWindow.
    /*!
     * This method allows the user to enter new data into an already existing InputWindow. If
     * the caller has not changed the contents of the buffer since the last use of this
     * InputWindow, the data previously entered by the user will be displayed. This method hides
     * the window the user is done.
     */
    int InputWindow::show( )
    {
        int return_value;
        int current_row, current_column;

        // Draw the shadow.
        if( is_hidden ) {
            the_shadow.open( row( ) - 1 + 1, column( ) - 1 + 2, width( ) + 2, 3 );
        }

        // Show the main window (prompt still there).
        SimpleWindow::show( );

        // Learn about the current cursor position.
        get_cursor_position( &current_row, &current_column );

        // Get data from the user.
        return_value = gets(
            row( ),
            column( ) + static_cast<int>( std::strlen( prompt_text ) ) + 1,
            buffer_size,
            ACTIVE_DESC.secondary_attribute,
            buffer
        );

        // Reposition the cursor and hide the window.
        set_cursor_position( current_row, current_column );
        hide( );
        return return_value;
    }


    //! Hide the InputWindow.
    void InputWindow::hide( )
    {
        SimpleWindow::hide( );
        the_shadow.close( );
    }


    //! Close the InputWindow.
    void InputWindow::close( )
    {
        SimpleWindow::close( );
        the_shadow.close( );
    }

}
