/*! \file    scrtools.cpp
    \brief   Library of useful tools based on scr.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cctype>
#include <cstring>

#include "scr.hpp"
#include "scrtools.hpp"

namespace scr {

    //! Centers text in a particular region.
    /*!
     *
     * If the given text is larger than the width of the region, the text is left justified in
     * the region and truncated at the region's width.
     *
     * \param row Row coordinate of the region.
     * \param column Column coordinate of left side of region.
     * \param width Width of the region.
     * \param attribute Color attribute used for the text.
     * \param text Pointer to null terminated string containing the text to display.
     */
    void center( int row, int column, int width, int attribute, const char *text )
    {
        int offset;
        std::size_t text_length = std::strlen( text );

        if( static_cast< std::size_t >( width ) < text_length ) {
            print( row, column, width, attribute, "%s", text );
        }
        else {
            offset = ( width - text_length ) / 2;
            print( row, column + offset, text_length, attribute, "%s", text );
        }
    }


    //! Draws a box around the specified region.
    /*!
     *
     * The box is drawn using the specified region coordinates. The interior of the box has a
     * width and height that are two less than the given values.
     *
     * \param row Row coordinate of the region's upper left corner.
     * \param column Column coordinate of the region's upper left corner.
     * \param width Width of the region.
     * \param height Height of the region.
     * \param the_type The type of box to draw.
     * \param attribute Color attribute used for the box.
     */
    void draw_box(
        int row, int column, int width, int height, enum BoxType the_type, int attribute)
    {
        int i;
        struct BoxChars *box_type = get_box_characters( the_type );

        for( i = column + 1; i < column + width - 1; i++ ) {
            print( row, i, 1, attribute, "%c", box_type->horizontal );
            print( row + height - 1, i, 1, attribute, "%c", box_type->horizontal );
        }
        for( i = row + 1; i < row + height - 1; i++ ) {
            print( i, column, 1, attribute, "%c", box_type->vertical );
            print( i, column + width - 1, 1, attribute, "%c", box_type->vertical );
        }
        print( row, column, 1, attribute, "%c", box_type->upper_left );
        print( row, column + width - 1, 1, attribute, "%c", box_type->upper_right );
        print( row + height - 1, column, 1, attribute, "%c", box_type->lower_left );
        print( row + height - 1, column + width - 1, 1, attribute, "%c", box_type->lower_right );
    }


    //! Get a string of text.
    /*!
     * Allows the user to enter a line of text into a buffer with basic editing features. This
     * function prevents the user from entering too much text. The initial contents of the
     * buffer are restored if the user types the ESC key. This implies that the buffer must be
     * well defined initially.
     *
     * \param row Row coordinate of where the text will be entered.
     * \param column Column coordinate of where the text will be entered.
     * \param length The size of the given buffer. Includes space for null byte.
     * \param buffer Pointer to a buffer to receive teh entered text.
     */
    int gets( int row, int column, int length, int attribute, char *buffer )
    {
        static char holding[128+1];
        int     buffer_offset;
        int     key_pressed;
        bool    first_key = true;
        int     i;

        buffer[length] = '\0';           // In case initial string too long.
        std::strcpy( holding, buffer );  // Save in case user wants to abort.
        buffer_offset = std::strlen( buffer );

        do {
            // Print as much from the buffer as there is right now and position cursor.
            print( row, column, std::strlen( buffer ), attribute, "%s", buffer );
            set_cursor_position( row, column + buffer_offset );

            // Clear the rest (to show size of buffer in attribute).
            if( length - std::strlen( buffer ) != 0 ) {
                clear( row, column + std::strlen( buffer ), length - std::strlen( buffer ), 1, attribute );
            }

            // Handle editing.
            switch( refresh( ), key_pressed = key( ) ) {

            case K_ESC:
                std::strcpy( buffer, holding );  // Restore original string.
                break;

            case K_HOME:
                buffer_offset = 0;
                break;

            case K_END:
                buffer_offset = std::strlen( buffer );
                break;

            case K_RIGHT:
                if( static_cast< std::size_t >( buffer_offset ) != std::strlen( buffer ) )
                    buffer_offset++;
                break;

            case K_LEFT:
                if( buffer_offset != 0 )
                    buffer_offset--;
                break;

            case K_BACKSPACE:
                if( buffer_offset != 0 ) {
                    for( i = buffer_offset; ( buffer[i - 1] = buffer[i] ) != '\0'; i++ ) ;
                    buffer_offset--;
                }
                break;

            case K_DEL:
                if( static_cast< std::size_t >( buffer_offset ) != std::strlen( buffer ) ) {
                    for( i = buffer_offset; ( buffer[i] = buffer[i + 1] ) != '\0'; i++ ) ;
                }
                break;

            default:
                // Ignore other special keys.
                if( key_pressed < 128  &&  std::isprint( key_pressed ) ) {

                    // If first key is a letter, erase any initial buffer contents.
                    if( first_key ) {
                        *buffer = '\0';
                        buffer_offset = 0;
                    }

                    // Do stuff only if there's space.
                    if( std::strlen( buffer ) < static_cast< std::size_t >( length ) ) {
                        i = std::strlen( buffer );
                        do {
                            buffer[i + 1] = buffer[i];
                            i--;
                        } while( i >= buffer_offset );
                        buffer[buffer_offset++] = static_cast< char >( key_pressed );
                    }
                }
                break;

            }   // End of switch

            // Turn off first key flag.
            first_key = false;

        } while( key_pressed != K_RETURN  &&  key_pressed != K_ESC  &&  key_pressed != K_TAB );

        // Move cursor to start of where buffer was printed.
        set_cursor_position( row, column );
        refresh( );

        // Return either K_RETURN or K_ESC or K_TAB.
        return key_pressed;
    }

}
