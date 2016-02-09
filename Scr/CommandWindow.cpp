/*! \file    CommandWindow.cpp
    \brief   Implementation of class CommandWindow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstring>
#include <cstdlib>

#include "CommandWindow.hpp"
#include "scr.hpp"
#include "scrtools.hpp"

using namespace std;

namespace scr {

    /*!
     * Changes (or sets) the command prompt.
     *
     * \param new_prompt A pointer to a null terminated string containing the new prompt. This
     * string is copied into the CommandWindow.
     *
     * \throws std::bad_alloc if there is insufficient memory to make the copy of the prompt.
     */
    void CommandWindow::set_prompt( const char *new_prompt )
    {
        prompt = new_prompt;
    }


    bool CommandWindow::process_keystroke( int &key_code )
    {
        std::string::size_type old_cursor_offset;
        int old_cursor_row;
        int width;
        int height;

        my_manager->get_size( this, width, height );

        if( key_code == K_TAB    ) return( false );
        if( key_code == K_CLEFT  ) return( false );
        if( key_code == K_CRIGHT ) return( false );
        if( key_code == K_CUP    ) return( false );
        if( key_code == K_CDOWN  ) return( false );

        switch( key_code ) {

        case K_DEL:
            command_text.erase( cursor_offset, 1 );
            break;

        case K_RETURN:
            break;

        case K_BACKSPACE:
            if( cursor_offset == 0 ) break;
            cursor_offset--;
            command_text.erase( cursor_offset, 1 );
            break;

        case K_HOME:
            old_cursor_row = cursor_row( );
            if( old_cursor_row == 1 ) cursor_offset = 0;
            else {
                cursor_offset =
                    ( width - prompt.length( ) ) + ( width * ( old_cursor_row - 2 ) );
            }
            break;

        case K_END:
            old_cursor_row = cursor_row( );
            if( old_cursor_row == 1 ) cursor_offset = width - prompt.length( ) - 1;
            else {
                cursor_offset =
                    ( width - prompt.length( ) ) + ( width * ( old_cursor_row - 1 ) - 1 );
            }
            if( cursor_offset > command_text.length( ) )
                cursor_offset = command_text.length( );
            break;

        case K_CHOME:
            cursor_offset = 0;
            break;

        case K_CEND:
            cursor_offset = command_text.length( );
            break;

        case K_UP:
            old_cursor_offset = cursor_offset;
            if( static_cast< size_t >( width ) > cursor_offset ) {
                cursor_offset = old_cursor_offset;
            }
            else {
                cursor_offset -= width;
            }
            break;

        case K_DOWN:
            old_cursor_offset = cursor_offset;
            cursor_offset += width;
            if( cursor_offset > command_text.length( ) ) {
                cursor_offset = old_cursor_offset;
                break;
            }
            if( cursor_row( ) > height ) {
                cursor_offset = old_cursor_offset;
                break;
            }
            break;

        case K_LEFT:
            if( cursor_offset == 0 ) break;
            cursor_offset--;
            break;

        case K_RIGHT:
            cursor_offset++;
            if( cursor_offset > command_text.length( ) )
                cursor_offset = command_text.length( );
            break;

        default:
            command_text.insert( cursor_offset, 1, key_code );
            cursor_offset++;
            break;
        }
        return( true );
    }


    /*!
     * \bug If the command line is too long for the window to hold, the behavior is undefined.
     * This is because image.copy does not have any checks on how much material it is copying.
     */
    ImageBuffer *CommandWindow::get_image( )
    {
        int width;
        int height;

        width  = image.get_width( );
        height = image.get_height( );
        image.clear( );

        image.copy( prompt.c_str( ), 1, 1, prompt.size( ), WHITE|REV_BLACK );

        int text_row = static_cast<int>( prompt.length( ) ) / width + 1;
        int text_column = static_cast<int>( prompt.length( ) ) % width + 1;
        image.copy(
            command_text.c_str( ), text_row, text_column, command_text.length( ), WHITE|REV_BLACK );
        return( &image );
    }


    /*!
     * Computes the row relative to the start of the display area where the cursor is currently
     * located.
     *
     * \return The row number. One means the first row of the window's display area.
     */
    int CommandWindow::cursor_row( )
    {
        int width;
        int height;

        my_manager->get_size( this, width, height );
        return ( static_cast<int>( prompt.length( ) + cursor_offset ) / width ) + 1;
    }


    /*!
     * Computes the column relative to the left of the display area where the cursor is
     * currently located.
     *
     * \return The column number. One means the first column of the window's display area.
     */
    int CommandWindow::cursor_column( )
    {
        int width;
        int height;

        my_manager->get_size( this, width, height );
        return ( ( prompt.length( ) + cursor_offset ) % width ) + 1;
    }

}
