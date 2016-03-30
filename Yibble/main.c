/*! \file    main.c
 *  \brief   The main program of the yibble editor.
 *  \author  CIS-2025 at Vermont Technical College, Spring 2016
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <curses.h>

#include "errors.h"
#include "text.h"

void draw_status_bar( int size_y, int size_x, int line_number, int column_number )
{
    int i;

    attron( A_REVERSE );
    move( size_y - 1, 0 );
    for( i = 0; i < size_x; ++i ) {
        addch( ' ' );
    }
    
    mvprintw( size_y - 1, 0, "YIBBLE! (%d, %d)", line_number, column_number );
    attroff( A_REVERSE );
}


int main( void )
{
    int quit = 0;
    int ch;
    int size_y, size_x;

    // Initialize curses.
    initscr( );
    cbreak( );
    noecho( );
    nonl( );
    keypad( stdscr, TRUE );
    getmaxyx( stdscr, size_y, size_x ); // Get the current screen size.

    // Initialize the text to edit.
    initialize_text( );

    while( !quit ) {
        // Update the display.
        erase( );
        draw_status_bar( size_y, size_x, get_current_line( ) + 1, get_current_column( ) + 1 );
        draw_screen( );

        ch = getch( );

        if( isprint( ch ) ) {
            insert_character( ch );
        }
        else {
            switch( ch ) {
            case KEY_DC:
                delete_character( );
                break;

            case KEY_BACKSPACE:
                backspace_character( );
                break;

            case KEY_LEFT:
                move_left( );
                break;

            case KEY_RIGHT:
                move_right( );
                break;

            case KEY_UP:
                move_up( );
                break;

            case KEY_DOWN:
                move_down( );
                break;

            case 'X' - 0x40:
                quit = 1;
                break;

            default:
                error_message( size_y, size_x, "Unknown key!" );
                break;
            }
        }
    }

    // Finialize curses.
    endwin( );
    return 0;
}
