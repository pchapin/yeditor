/*! \file    Manager.cpp
    \brief   Implementation of the window Manager.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <list>
#include <stdexcept>

#include "ImageBuffer.hpp"
#include "Manager.hpp"
#include "scrtools.hpp"

using namespace std;

namespace scr {

    //! Constructs a Manager object.
    /*!
     * The default constructor initializes an empty Manager object. As a side effect the
     * underlying screen Initialize() function is executed causing the screen the clear.
     * Initially Managers are not in "system mode" and so all key strokes are sent to the top
     * window for processing. Use ALT+S to toggle system mode and thus cause the Manager to
     * intercept certain window control keys.
     *
     *  \throws std::runtime_error if the screen initialization fails.
     */
    Manager::Manager( ) :
        the_windows( ), system_mode( false ), key_array( 0 ), key_count( 0 )
    {
        if( !initialize( ) ) {
            throw runtime_error( "scr::Manager::Manager failed" );
        }
    }

    //! Destructor.
    /*!
     * The destructor deletes any leftover windows assuming they have been dynamically
     * allocated. Statically allocated and automatically allocated windows should already have
     * been destroyed (and deregistered themselves) by the time the Manager is destroyed. The
     * Manager destructor also calls the underlying screen Terminate() function which causes the
     * screen to clear as a side effect.
     */
    Manager::~Manager( )
    {
        // Copy the window list so that the iteration is not confused by window deregistrations.
        std::list< WindowInformation > temp_list( the_windows );

        std::list< WindowInformation >::iterator stepper = temp_list.begin( );
        while( stepper != temp_list.end( ) ) {
            delete stepper->the_window;
            stepper++;
        }
        key_array = 0;

        // Clean up the display.
        terminate( );
    }

    //! Register a window with the Manager.
    /*!
     * The Window constructor uses this function to register its window with the Manager.
     * Immediately after registration the new window is the foreground window. The location and
     * size specifications specify the Window's initial configuration. However, the Manager is
     * not obligated to honor this request. For example, the Manager will force the window on
     * screen (in case it extends off the side) and will prevent zero sized windows. The manager
     * may apply other restrictions as well. The window must cope with the possibility that the
     * manager ends up giving it a different size than requested. The window is given the
     * opportunity to sign off on the adjusted location and size. If the window is unhappy with
     * either, the registration fails.
     *
     * \return true if registration successful; false otherwise.
     */
    bool Manager::register_window( Window *new_window, int row, int column, int width, int height )
    {
        int total_rows = number_of_rows( );
        int total_columns = number_of_columns( );
        
        // General sanity checks. Make empty windows illegal.
        if( width  < 1 ) width = 1;
        if( height < 1 ) height = 1;

        // Sanity checks. Keep the window on screen.
        if( row    < 2 ) row = 2;
        if( column < 2 ) column = 2;
        if( row    > total_rows - 1         ) row    = total_rows - 1;
        if( column > total_columns - 1      ) column = total_columns - 1;
        if( width  > total_columns - column ) width  = total_columns - column;
        if( height > total_rows - row       ) height = total_rows - row;

        // If the window doesn't like its initial position and size, we give up.
        if( !new_window->reposition( row, column ) ) return false;
        if( !new_window->resize( width, height ) ) return false;

        // Build a new WindowInformation object with info about the new window.
        const WindowInformation new_information =
            { new_window, row, column, width, height };

        // Put it at the end. The new window is in the foreground.
        the_windows.push_back( new_information );
        return true;
    }


    //! Deregister a window from the window manager.
    /*!
     * The Window destructor uses this function to deregister its window from the Manager.
     *
     * \param old_window A pointer to the window object being deregistered.
     */
    void Manager::deregister_window( const Window *old_window )
    {
        list< WindowInformation >::iterator stepper = the_windows.begin( );

        while( stepper != the_windows.end( ) ) {
            if( stepper->the_window == old_window ) {
                the_windows.erase( stepper );
                break;
            }
            ++stepper;
        }
    }

    //! Returns the dimensions of the window pointed at by 'w.'
    /*!
     * Windows sometimes need to know how big they are on the managed display. This method looks
     * up the given window's dimensions in the manager's database. Note that this method
     * performs a linear search of the windows known to the manager. While this is inefficient,
     * it should be acceptable in cases where the manager is only handling a small number of
     * windows (for example, less than 100).
     *
     * \param w The window for which dimensions are to be retrieved.
     * \param width Location to receive the window's width.
     * \param height Location to recieve the window's height.
     */
    void Manager::get_size( Window *w, int &width, int &height )
    {
        list< WindowInformation >::iterator stepper = the_windows.begin( );
        while( stepper != the_windows.end( ) ) {
            if( stepper->the_window == w ) {
                width = stepper->width;
                height = stepper->height;
                return;
            }
            ++stepper;
        }

        // If we get here we should probably signal an error in some way.
        return;
    }


    //! Refresh the entire display.
    /*!
     * This function builds the display with all windows shown in the correct order, with the
     * correct positioning, and using the correct attributes. Every character on the display is
     * recomputed.
     */
    void Manager::update_display( )
    {
        // This sets the background. For now let's just use a plain background.
        clear( 1, 1, number_of_columns( ), number_of_rows( ), WHITE|REV_BLACK );

        // Step down the list of WindowInformation structures.
        list< WindowInformation >::iterator stepper = the_windows.begin( );
        while( stepper != the_windows.end( ) ) {

            // Get a pointer to the current image.
            ImageBuffer *ptr = stepper->the_window->get_image( );

            // Write the image into the virtual screen. This assumes a border is present.
            ptr->write( stepper->row_position, stepper->column_position );

            // Draw the border around the window.
            draw_box( stepper->row_position - 1, stepper->column_position - 1, stepper->width + 2, stepper->height + 2, SINGLE_LINE, WHITE );

            ++stepper;
        }

        // If the list wasn't empty...
        if( the_windows.size( ) != 0 ) {
            --stepper;

            // Draw the foreground window with a different border.
            draw_box( stepper->row_position - 1, stepper->column_position - 1, stepper->width + 2, stepper->height + 2, DOUBLE_LINE, BRIGHT|WHITE );

            // Position the cursor on the screen correctly.
            set_cursor_position(
                stepper->row_position + stepper->the_window->cursor_row( ) - 1,
                stepper->column_position + stepper->the_window->cursor_column( ) - 1
            );
        }

        // Copy stuff to the physical screen.
        refresh( );
    }


    //! Read and process input key strokes infinitely.
    /*!
     * This function loops infinitely, updating the display and then accepting key stroke input.
     * If the input key is a registered special key, the corresponding special function is
     * executed. Otherwise if the key is ALT+S, system mode is toggled. Otherwise if the Manager
     * is in system mode, the key is interpreted as a command to the manager itself. Otherwise
     * the key is sent to the foreground window for processing. If that window indicates that
     * the manager should handle the key anyway, it will then be processed as a manager command.
     *
     * Manager command keys are: ESC (return from this function), Arrow keys (move the
     * foreground window), Ctrl+PageUp and Ctrl+PageDn (resize the current window vertically,
     * Ctrl+LeftArrow and Ctrl+RightArrow (resize the current window horizontally, and TAB
     * (switch to another window).
     */
    void Manager::input_loop( )
    {
        int ch;
        int desired_row, desired_column;
        int desired_width, desired_height;

        for( ; ; ) {

            update_display( );
            ch = key( );

            // First, scan the special key list (if non-empty) and deal with any matches found there.
            bool found_special = false;
            int  special_index;
            for( special_index = 0; special_index < key_count; special_index++ ) {
                if( ch == key_array[special_index].key_code ) {
                    key_array[special_index].key_function( );
                    found_special = true;
                    break;
                }
            }
            if( found_special ) continue;

            // The keystroke Alt+S is always a system key. It toggles system mode.
            if( ch == K_ALTS ) {
                system_mode = !system_mode;
                continue;
            }

            // Do things only if the window list isn't empty.
            if( the_windows.size( ) != 0 ) {
                std::list< WindowInformation >::iterator top = the_windows.end( );
                --top;

                bool handle_key = false;

                if( system_mode )
                    handle_key = true;
                else
                    handle_key = !top->the_window->process_keystroke( ch );
        
                if( handle_key ) {
                    switch( ch ) {

                    // An ESC character while in system mode exits the loop.
                    case K_ESC:
                        return;

                    // Move the window up.
                    case K_UP:
                        desired_row = top->row_position - 1;
                        if( desired_row <= 2 ) desired_row = 2;
                        if( top->the_window->reposition( desired_row, top->column_position ) ) {
                            top->row_position = desired_row;
                        }
                        break;

                    // Move the window down.
                    case K_DOWN:
                        desired_row = top->row_position + 1;
                        if( ( desired_row + top->height ) > number_of_rows( ) ) {
                            desired_row--;
                        }
                        if( top->the_window->reposition( desired_row, top->column_position ) ) {
                            top->row_position = desired_row;
                        }
                        break;

                    // Move the window to the left.
                    case K_LEFT:
                        desired_column = top->column_position - 1;
                        if( desired_column <= 2 ) desired_column = 2;
                        if( top->the_window->reposition( top->row_position, desired_column ) ) {
                            top->column_position = desired_column;
                        }
                        break;

                    // Move the window to the right.
                    case K_RIGHT:
                        desired_column = top->column_position + 1;
                        if( ( desired_column + top->width ) > number_of_columns( ) ) {
                            desired_column--;
                        }
                        if( top->the_window->reposition( top->row_position, desired_column ) ) {
                            top->column_position = desired_column;
                        }
                        break;

                    // Select the next window on the list.
                    case K_TAB: {
                            const WindowInformation temp = the_windows.back( );
                            the_windows.pop_back( );
                            the_windows.push_front( temp );
                        }
                        break;

                    // Try to resize the window vertically. (Making it smaller).
                    case K_CUP:
                        desired_height = top->height - 1;
                        if( desired_height < 1 ) desired_height = 1;
                        if( top->the_window->resize( top->width, desired_height ) ) {
                            top->height = desired_height;
                        }
                        break;
          
                    // Try to resize the window vertically. (Making it larger).
                    case K_CDOWN:
                        desired_height = top->height + 1;
                        if( ( top->row_position + desired_height ) > number_of_rows( ) ) desired_height--;
                        if( top->the_window->resize( top->width, desired_height ) ) {
                            top->height = desired_height;
                        }
                        break;
          
                    // Try to resize the window horizontally. (Making it narrower).
                    case K_CLEFT:
                        desired_width = top->width - 1;
                        if( desired_width < 1 ) desired_width = 1;
                        if( top->the_window->resize( desired_width, top->height ) ) {
                            top->width = desired_width;
                        }
                        break;
          
                    // Try to resize the window horizontally. (Making it wider).
                    case K_CRIGHT:
                        desired_width = top->width + 1;
                        if( (top->column_position + desired_width ) > number_of_columns( ) ) desired_width--;
                        if( top->the_window->resize( desired_width, top->height ) ) {
                            top->width = desired_width;
                        }
                        break;

                    // Ignore keys we don't recognize.
                    default:
                        break;
                    }
                }
            }
        } // End of while( true ) loop.
    }

    //! Rotate the top window to the back.
    /*!
     * This method is useful for changing the display order. However, since it only allows a
     * rotation of the order, it is quite limited. Eventually the Manager class should provide a
     * more comprehensive way of manipulating display order.
     */
    void Manager::swap_top( )
    {
        // Forget the whole thing if there isn't at least two windows on the screen.
        if( the_windows.size( ) < 2 ) return;

        const WindowInformation temp = the_windows.back( );
        the_windows.pop_back( );
        the_windows.push_front( temp );
    }


    //! This is semantically meaningless. It only exists to satisfy OW's eager instantiation.
    bool Manager::WindowInformation::operator==( const WindowInformation & )
    {
        return( false );
    }

    //! This is semantically meaningless. It only exists to satisfy OW's eager instantiation.
    bool Manager::WindowInformation::operator< ( const WindowInformation & )
    {
        return( false );
    }
  
}
