/*! \file    global.cpp
 *  \brief   Definitions of global objects.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "environ.hpp"

#include <cstdio>
#include <cstdlib>

#include "global.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"

//================================================================
//           Functions to support global initialization
//================================================================


//! Display the version number and date of compilation.
/*!
 * The displayed date is actually the date this file was compiled. This function is called at
 * program exit after the screen has been cleaned up but before the file data is released from
 * memory.
 */
static void credits( )
{
    #if eOPSYS == eDOS
    const char *version = "for DOS";
    #elif eOPSYS == eOS2
    const char *version = "for OS/2";
    #elif eOPSYS == eWIN32
    const char *version = "for Win32";
    #elif eOPSYS == ePOSIX
    const char *version = "for Unix";
    #endif

    std::printf("Y  Version 2.0 %s (ALPHA) Compiled: %s\n"
                "(C) Copyright 2018 by Peter C. Chapin\n", version, adjust_date(__DATE__));
    std::printf("\nWARNING: Development release; bugs likely!\n");
    std::printf("Please report bugs and suggestions to peter@pchapin.org\n");
}

//=================================================================
//           Functions to control global initializations
//=================================================================

void global_setup( )
{
    scr::initialize( );
    scr::refresh_on_key( true );
    box_size     = ( scr::number_of_columns( ) < 71 ) ? scr::number_of_columns( ) - 6 : 65;
    start_row    =   scr::number_of_rows( )/2;
    start_column = ( scr::number_of_columns( ) - box_size ) / 2 + 1;
}

void global_cleanup( )
{
    scr::terminate( );
    credits( );
}

//=======================================================
//          Initialize the global data objects
//=======================================================

bool      yfile_flag   = false;
Parameter search_parameter ( "SEARCH FOR:" );
Parameter replace_parameter( "REPLACE WITH:" );
bool      search_set   = false;     //!< =true when search string is set.
bool      replace_set  = false;     //!< =true when replace string is set.
int       box_size     = 0;         //!< The number of cols used for the input box.
int       start_row    = 0;         //!< The row number of the top row of the box.
int       start_column = 0;         //!< The col number of the left col of the box.
Stack< EditBuffer > parameter_stack;
bool      restricted_mode = false;  //!< =true when restricted mode is active.


//! Table used by the commands that set colors to interpret the color words typed by the user.
ColorInfo colors[] = {
    { "WHITE"  , scr::WHITE  , scr::REV_WHITE   },
    { "BLACK"  , scr::BLACK  , scr::REV_BLACK   },
    { "BLUE"   , scr::BLUE   , scr::REV_BLUE    },
    { "GREEN"  , scr::GREEN  , scr::REV_GREEN   },
    { "RED"    , scr::RED    , scr::REV_RED     },
    { "CYAN"   , scr::CYAN   , scr::REV_CYAN    },
    { "MAGENTA", scr::MAGENTA, scr::REV_MAGENTA },
    { "BROWN"  , scr::BROWN  , scr::REV_BROWN   },
    {  NULL    , -1          , -1               },
};
