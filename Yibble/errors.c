/*! \file    errors.c
 *  \brief   Definitions of various error handling facilities.
 *  \author  CIS-2025 at Vermont Technical College, Sprin 2016
 *
 * This file declares the interface to femto's error handling services.
 */
#include <string.h>
#include <curses.h>
#include "errors.h"

void error_message( int size_y, int size_x, char *message )
{
    int i;
    int dismiss = 0;
    int start_position = size_x - strlen( message );

    attron( A_REVERSE );
    while( !dismiss ) {
        mvaddstr( size_y - 1, start_position, message );
        dismiss = (getch( ) == 27 );
        if( dismiss ) {
            move( size_y - 1, start_position );
            for( i = 0; i < size_x - start_position; ++i ) {
                addch( ' ' );
            }
        }
    }
    attroff( A_REVERSE );
}
