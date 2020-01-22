/*! \file    basic_test.cpp
    \brief   This file contains test code for the low level screen handling library.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "check.hpp"
#include "scr.hpp"

using namespace scr;

/*=================================*/
/*           Global Data           */
/*=================================*/

static char  holding_area[1000];
static short bland_color = WHITE|REV_BLACK;

static int FG_colors[] = {
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE, -1
};

static int BG_colors[] = {
    REV_BLACK, REV_BLUE, REV_GREEN, REV_CYAN, REV_RED, REV_MAGENTA, REV_BROWN, REV_WHITE, -1
};

static const char *colors[]={
    "BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "WHITE", 0
};

/*=========================================*/
/*           Function Prototypes           */
/*=========================================*/

static void fill_screen();
static void position_test();
static void color_test(int attribute);
static void clear_test();
static void scroll_test(direction_t direction);
static void read_write_test();
static void read_write_text_test();
static void misc_test();

void basic_test( )
{
    print_text( 1, 1, 80, "Testing scr" );
    print_text( 2, 1, 80, "This sould be in the upper left corner of the screen." );
    refresh( );
    key( );
    initialize( );
    print_text( 3, 1, 80, "Screen should not have just cleared." );
    refresh( );
    key( );

    position_test( );
    color_test( 0 );
    color_test( BLINK );
    color_test( BRIGHT );
    color_test( BLINK|BRIGHT );
    clear_test( );
    scroll_test( UP );
    scroll_test( DOWN );
    read_write_test( );
    read_write_text_test( );
    misc_test( );
    key( );
}

/*==========================================*/
/*           Function Definitions           */
/*==========================================*/

static void fill_screen( )
{
    int  i, j;
    char junk[80+1];
    char ch = 'A';

    junk[80] = '\0';
    for( i = 1; i <= 25; i++ ) {
        for( j = 0; j < 80; j++ ) junk[j] = ch;
        ch++;
        print_text( i, 1, 80, junk );
    }
}

/*---------------------------------------------------------------------------*/

static void position_test( )
{
    int i;
    int R_count = number_of_rows( );
    int C_count = number_of_columns( );

    CLEAR_SCREEN;
    HOME_CURSOR;

    for( i = 5; i <= 20; i++ ) {
        print_text( i, ( 3 * i ) + 2, 80, "- (%d,%d)", i, ( 3 * i )+2 );
    }
    print_text(       1,       1, 1, "1" );
    print_text(       1, C_count, 1, "2" );
    print_text( R_count,       1, 1, "3" );
    print_text( R_count, C_count, 1, "4" );

    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void color_test( int attribute )
{
    int  i, j;
    int  row, column;

    CLEAR_SCREEN;
    HOME_CURSOR;

    for( i = 0; BG_colors[i] != -1; i++ ) {
        row = ( ( i % 2 ) ? 16 : 3 );
        column = ( i / 2 ) * 19 + 3;
        for( j = 0; FG_colors[j] != -1; j++ ) {
            print(row++, column, 18, FG_colors[j] + BG_colors[i] + attribute, "%7s on %7s", colors[j], colors[i] );
        }
    }

    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void clear_test( )
{
    int i;
    int row;
    int column;

    CLEAR_SCREEN;
    HOME_CURSOR;

    fill_screen( );
    for( i = 0; BG_colors[i] != -1; i++ ) {
        row = ( (i % 2 ) ? 16 : 3 );
        column = ( i / 2 ) * 19 + 3;
        clear( row, column, 18, 8, BG_colors[i] );
    }

    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void scroll_test( direction_t direction )
{
    int i;
    int row;
    int column;

    CLEAR_SCREEN;
    HOME_CURSOR;

    fill_screen( );
    for( i = 0; BG_colors[i] != -1; i++ ) {
        row = ( ( i % 2 ) ? 16 : 3 );
        column = ( i / 2 ) * 19 + 3;
        scroll( direction, row, column, 18, 8, i+1, BG_colors[i] );
    }

    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void read_write_test( )
{
    int i;

    for( i = 0; BG_colors[i] != -1; i++ ) {
        clear( 1, ( 10 * i ) + 1, 10, 25, BG_colors[i] );
    }
    fill_screen( );
    clear( 25, 1, 80, 1, bland_color );
    print_text( 25, 1, 80, "Ready to save a rectangle from the middle of the screen..." );
    refresh( );
    key( );

    read( 8, 25, 30, 11, holding_area );
    CLEAR_SCREEN;
    clear( 25, 1, 80, 1, bland_color );
    print_text( 25, 1, 80, "Ready to restore the rectangle to several different places..." );
    refresh( );
    key( );

    write(  1,  6, 30, 11, holding_area );
    write(  1, 46, 30, 11, holding_area );
    write( 13,  6, 30, 11, holding_area );
    write( 13, 46, 30, 11, holding_area );
    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void read_write_text_test( )
{
    int i;

    for( i = 0; BG_colors[i] != -1; i++ ) {
        clear( 1, ( 10 * i ) + 1, 10, 25, BG_colors[i] );
    }
    fill_screen( );
    clear( 25, 1, 80, 1, bland_color );
    print_text( 25, 1, 80, "Ready to save a text rectangle from the middle of the screen..." );
    refresh( );
    key( );

    read_text( 8, 25, 30, 11, holding_area );
    for( i = 0; BG_colors[i] != -1; i++ ) {
        clear( ( 3 * i ) + 1, 1, 80, 3, BG_colors[i] );
    }
    clear( 25, 1, 80, 1, bland_color );
    print_text( 25, 1, 80, "Ready to restore the text rectangle to several different places..." );
    refresh( );
    key( );

    write_text(  1,  6, 30, 11, holding_area );
    write_text(  1, 46, 30, 11, holding_area );
    write_text( 13,  6, 30, 11, holding_area );
    write_text( 13, 46, 30, 11, holding_area );
    refresh( );
    key( );
}

/*---------------------------------------------------------------------------*/

static void misc_test( )
{
    const char *message = "This is a rather long message.";

    CLEAR_SCREEN;
    print_text( 1, 1, 80, message );
    print_text( 2, 1, 15, message );
    print_text( 3, 1, 1,  message );
    print_text( 4, 1, 0,  message );
    print_text( 5, 1, 80, "Stike any key..." );
    refresh( );
    key( );
}
