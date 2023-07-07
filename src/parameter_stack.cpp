/*! \file    parameter_stack.cpp
 *  \brief   Implementation of the parameter stack.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cctype>
#include <cstdlib>
#include <cstring>

#include "EditBuffer.hpp"
#include "global.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "Shadow.hpp"
#include "support.hpp"
#include "Window.hpp"


Parameter::Parameter( const char *prompt_string )
{
    this->prompt_string = prompt_string;
}


static bool add( EditBuffer *line, EditList &data )
{
    // Insert into the EditList.
    data.jump_to( 0 );
    data.insert( line );

    // If the list is too long, get rid of one of the parameter strings.
    long item_count = data.size( );
    if( item_count > 8 ) {
        data.jump_to( item_count - 1 );
        delete data.get( );
        data.erase( );
    }
    return true;
}


int Parameter::edit( EditBuffer &workspace )
{
    int      key;
    bool     first_key      = true;
    unsigned cursor_offset  = workspace.length( );
    unsigned display_offset = 0;
    int      text_column    = start_column + std::strlen( prompt_string ) + 3;
    int      text_width     = start_column + box_size - text_column - 1;
    bool     replace_mode   = false;

    do {
        std::string workspace_string = workspace.to_string( );

        // Make sure our limits are ok. We want the display to look good!
        if( cursor_offset < display_offset )
            display_offset = cursor_offset;

        if( cursor_offset > display_offset + text_width )
            display_offset = cursor_offset - text_width;

        std::size_t displayed_length = std::strlen( workspace_string.c_str( ) + display_offset );
        if( static_cast< int >( displayed_length ) > text_width )
            displayed_length = text_width;

        scr::print_text(
            start_row + 1,
            text_column,
            text_width,
            "%s",
            workspace_string.c_str() + display_offset
        );
        scr::set_cursor_position( start_row + 1, text_column + cursor_offset - display_offset );

        // Clear the rest.
        if( text_width - displayed_length != 0 ) {
            scr::clear(
                start_row   + 1,
                text_column + displayed_length,
                text_width  - displayed_length,
                1,
                scr::REV_WHITE
            );
        }

        // Handle editing.
        switch( key = scr::key( ) ) {

        case scr::K_DOWN:
        case scr::K_ESC:
        case scr::K_RETURN:
        case scr::K_UP:
            break;

        case scr::K_INS:
            if( replace_mode ) replace_mode = false;
            else replace_mode = true;
            break;

        case scr::K_HOME:
            cursor_offset = 0;
            break;

        case scr::K_END:
            cursor_offset = workspace.length( );
            break;

        case scr::K_RIGHT:
            if( cursor_offset != workspace.length( ) ) cursor_offset++;
            break;

        case scr::K_CRIGHT:
            cursor_offset = word_right( workspace, cursor_offset );
            break;

        case scr::K_LEFT:
            if( cursor_offset != 0 ) cursor_offset--;
            break;

        case scr::K_CLEFT:
            cursor_offset = word_left( workspace, cursor_offset );
            break;

        case scr::K_BACKSPACE:
            if( cursor_offset != 0 ) {
                cursor_offset--;
                if( !replace_mode ) workspace.erase( cursor_offset );
            }
            break;

        case scr::K_DEL:
            if( cursor_offset != workspace.length( ) ) {
                workspace.erase( cursor_offset );
            }
            break;

        default:

            // Ignore other special keys.
            if( key < 128  &&  std::isprint( key ) ) {

                // If first key is a letter, erase any initial buffer contents.
                if( first_key ) {
                    workspace.erase( );
                    display_offset = 0;
                    cursor_offset  = 0;
                }

                char letter = static_cast< char >( key );
                if( replace_mode ) workspace.replace( letter, cursor_offset );
                else workspace.insert( letter, cursor_offset );
                cursor_offset++;
            }
            break;

        }   // End of switch.

        // Turn off first key flag.
        first_key = false;

    } while(key != scr::K_ESC    &&
            key != scr::K_RETURN &&
            key != scr::K_UP     &&
            key != scr::K_DOWN );

    return key;
}


int Parameter::get( bool pop )
{
    int return_value = true;

    // Pop parameter directly off the parameter stack if pop == true and if there's something to
    // pop.
    // 
    if( pop == true && parameter_stack.size( ) != 0 ) {
        EditBuffer *inserted_line = new EditBuffer( *parameter_stack.get( ) );
        add( inserted_line, input_data );
        parameter_stack.delete_top( );
    }

    // Otherwise, let the user enter something.
    else {
        scr::SimpleWindow box;
        scr::Shadow   box_shadow;
        EditBuffer   *workspace;
        long          item_number = 0L;
        int           key;

        // Draw the input box on the screen.
        box_shadow.open( start_row+1, start_column + 2, box_size, 3 );
        box.open( start_row, start_column, box_size, 3, scr::REV_WHITE, scr::SINGLE_LINE);

        // Write the appropriate prompt into the box.
        scr::print_text( start_row + 1, start_column + 2, box_size - 3, "%s", prompt_string );

        do {
            input_data.jump_to( item_number );
            if( input_data.get( ) == NULL )
                workspace = new EditBuffer;
            else
                workspace = new EditBuffer( *input_data.get( ) );

            switch( key = edit( *workspace ) ) {
            case scr::K_UP:
                item_number++;
                if( item_number >= input_data.size( ) ) item_number--;
                delete workspace;
                break;

            case scr::K_DOWN:
                item_number--;
                if( item_number < 0 ) item_number++;
                delete workspace;
                break;

            case scr::K_RETURN:
                // If the parameter's list is not empty and the first item is what the user just
                // typed, do nothing. Otherwise add the item to the list.
                //
                input_data.jump_to( 0 );
                if( input_data.size() != 0 && *input_data.get() == *workspace ) delete workspace;
                else add( workspace, input_data );
                break;

            case scr::K_ESC:
                delete workspace;
                break;
            }

        } while( key != scr::K_ESC && key != scr::K_RETURN );

        box.close( );
        box_shadow.close( );
        if( key == scr::K_ESC ) return_value = false;
    }
  
    return return_value;
}


std::string Parameter::value( )
{
    input_data.jump_to( 0 );
    return input_data.get( )->to_string( );
}
