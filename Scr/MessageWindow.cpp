/*! \file    MessageWindow.cpp
    \brief   Implementation of class MessageWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstddef>
#include <cstring>

#include "MessageWindow.hpp"
#include "scr.hpp"
#include "scrtools.hpp"

using namespace std;

static const struct scr::MessageWindowDescriptor *descriptor_list = NULL;

namespace scr {

    //! Set a new message descriptor table.
    /*!
     * The size of the table is not restricted to five entries (ie one for each symbolic name),
     * although it should have at least one entry. The client is responsible for passing correct
     * parameters to MessageWindow objects: the objects will assume that the table is long
     * enough.
     */
    void MessageWindow::set_descriptors( const struct MessageWindowDescriptor *new_stuff )
    {
        descriptor_list = new_stuff;
    }


    //! Default constructor.
    /*!
     * This method simply ensures that the object is initialized in a reasonable way. To use the
     * object properly, however, requires that set( ) be called after this constructor.
     */
    MessageWindow::MessageWindow( )
    {
        current_text  = "MessageWindow opened without set( )";
        current_level = 0;
    }


    //! Set the text and open the window at construction time.
    /*!
     * This constructor combines the abilities of set( ) and open( ) in one step. It is intended
     * for use when a lot of preliminaries are not needed.
     */
    MessageWindow::MessageWindow( const char *text, int level )
    {
        current_text  = text;
        current_level = level;
        open( );
    }


    //! Destructor.
    /*!
     * The destructor makes sure the window (and shadow) are hidden before letting the
     * destruction of the base object (Simple_Window) or the members (Shadow) occurs. This must
     * be done because the shadow is under the window.
     */
    MessageWindow::~MessageWindow( )
    {
        hide( );
    }


    //! Configure the window after construction.
    /*!
     * This method should be called after the default constructor and before open( ). It allows
     * the caller to define the message text and the type of MessageWindow to be displayed. The
     * level parameter is the index into the current descriptor array.
     */
    void MessageWindow::set( const char *text, int level )
    {
        current_text  = text;
        current_level = level;
    }


    #define ACTIVE_DESC descriptor_list[current_level]

    //! Display the message.
    /*!
     * This method calculates the size of the needed window based on the current parameters in
     * the descriptor table and on the message currently registered with the object. It then
     * displays the message and waits for the appropriate key to be pressed. It returns 0 if it
     * couldn't display the message, otherwise it returns the key code (from get_key( )). This
     * method will truncate long messages so that they will fit on one line. This method will
     * not accept NO_BORDER for a border type; if such a type is specified, it returns with a
     * failure code.
     */
    int MessageWindow::open( int row, int column )
    {
        int return_value;

        // Check to be sure a border exists.
        if( ACTIVE_DESC.border_type == NO_BORDER ) return 0;

        // Calculate width. Header text considered only if it exists.
        int width = strlen( current_text );
        if( ACTIVE_DESC.header_text != NULL )
            if( strlen( ACTIVE_DESC.header_text ) > static_cast< size_t >( width ) )
                width = strlen( ACTIVE_DESC.header_text );
        width += 4;
        if( width > number_of_columns( ) ) width = number_of_columns( );

        // Calculate height.
        int height = 4;
        if( ACTIVE_DESC.header_text == NULL ) height--;

        // Calculate position of window.
        int center_row, center_column;
        window_center_coordinates( width, height, &center_row, &center_column );
        if( row == 0 ) row = center_row;
        if( column == 0 ) column = center_column;

        // Create the shadow.
        the_shadow.open( row + 1, column + 2, width, height );

        // Open the window proper.
        if( ( return_value = SimpleWindow::open( row, column, width, height,
                                                 ACTIVE_DESC.interior_attribute,
                                                 ACTIVE_DESC.border_type,
                                                 ACTIVE_DESC.border_attribute ) ) != false ) {

            // Get first printable row.
            row = SimpleWindow::row( );
      
            // Print text into window.
            if( ACTIVE_DESC.header_text != NULL ) {
                center( row, SimpleWindow::column( ), SimpleWindow::width( ),
                        ACTIVE_DESC.header_attribute,
                        ACTIVE_DESC.header_text );
                row++;
            }
            center( row, SimpleWindow::column( ), SimpleWindow::width( ),
                    ACTIVE_DESC.interior_attribute,
                    current_text );

            // Wait for appropriate keystroke.
            if( ACTIVE_DESC.exit_char == MESSAGE_WINDOW_ANY ) {
                return_value = key( );
                hide( );
            }
            else if( ACTIVE_DESC.exit_char != '\0' ) {
                while( ( return_value = key( ) ) != ACTIVE_DESC.exit_char ) /* Null */ ;
                hide( );
            }
        }

        return return_value;
    }


    void MessageWindow::hide( )
    {
        SimpleWindow::hide( );
        the_shadow.close( );
    }

    
    void MessageWindow::close( )
    {
        SimpleWindow::close( );
        the_shadow.close( );
    }


    /*!
     * The show( ) method is overridden in this class to wait for the appropriate key press and
     * hide the message when it is received. This function depends on the fact that
     * Shadow::Open( ) does nothing if the shadow is already open. If this were not the case,
     * using show( ) on an already visable MessageWindow would cause problems.
     *
     * \return The key code pressed to exit the window (zero is returned if this method doesn't
     * wait for a keypress.
     */
    int MessageWindow::show( )
    {
        int return_value = 0;  // Use this value if there is no waiting for a key press.
        
        // Make window object visable.
        the_shadow.open( row( )-1 + 1, column( )-1 + 2, width( ) + 2, height( ) + 2 );
        SimpleWindow::show( );

        // Get a keypress.
        if( ACTIVE_DESC.exit_char == MESSAGE_WINDOW_ANY ) {
            return_value = key( );
            hide( );
        }
        else if( ACTIVE_DESC.exit_char != MESSAGE_WINDOW_NOWAIT ) {
            while( ( return_value = key( ) ) != ACTIVE_DESC.exit_char ) /* null */ ;
            hide( );
        }
        return return_value;
    }

}
