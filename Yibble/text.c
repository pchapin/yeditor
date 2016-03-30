/*! \file    text.c
 *  \brief   The representation of the text being edited.
 *  \author  CIS-2025 at Vermont Technical College, Spring 2016
 *
 */

#include <curses.h>  // TODO: Remove curses.h from this file's list of includes.
#include "text.h"

#define TEXT_SIZE 65536

char text[TEXT_SIZE];
int current;        // The index in 'text' where the current is located.
int gap;            // The index in 'text' where the gap above the cursor starts.
int current_line;   // The current line number in the file (zero based).
int current_column; // The current column position in the current line (zero based).
int count;          // The number of characters in the text.

void initialize_text( void )
{
    current_line = 0;
    current_column = 0;
    gap = 0;
    text[TEXT_SIZE - 1] = '\n';  // The text contains a single, empty line.
    count = 1;
    current = TEXT_SIZE - 1;
}

int get_current_line( void )
{
    return current_line;
}

int get_current_column( void )
{
    return current_column;
}

int get_count( void )
{
    return count;
}

void insert_character( char ch )
{
    // Do nothing if the text array is full.
    if( count == TEXT_SIZE) return;

    text[gap] = ch;
    gap++;
    count++;
    if( ch != '\n' ) {
        current_column++;
    }
    else {
        current_column = 0;
        current_line++;
    }
}

void delete_character( )
{
}

void backspace_character( )
{
}

void move_left( void )
{
    // Do nothing if we are already on the left edge of the line.
    if( current_column == 0 ) return;

    current--;
    gap--;
    text[current] = text[gap];
    current_column--;
}

void move_right( void )
{
    // Do nothing if we are already at the end of the line.
    if( text[current] == '\n' ) return;

    text[gap] = text[current];
    current++;
    gap++;
    current_column++;
}

void move_up( void )
{
}

void move_down( void )
{
}


void draw_screen( void )
{
    int current_character = 0;
    int cursor_column = 0;

    // For now just draw the first line of text...
    move( 0, 0 );
    while( 1 ) {
        if( current_character == gap ) {
            cursor_column = current_character;
            current_character = current;
        }

        if( text[current_character] == '\n' ) break;

        addch( text[current_character] );
        current_character++;
    }
    move( 0, cursor_column );
}
