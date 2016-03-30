/*! \file    text.h
 *  \brief   Declarations of text manipulation functions.
 *  \author  CIS-2025 at Vermont Technical College, Spring 2016
 *
 */

#ifndef TEXT_H
#define TEXT_H

void initialize_text( void );

int get_current_line( void );
int get_current_column( void );

void insert_character( char ch );
void delete_character( );
void backspace_character( );

void move_left( void );
void move_right( void );
void move_up( void );
void move_down( void );

// TODO: Move draw_screen( ) to its own file.
void draw_screen( void );

#endif
