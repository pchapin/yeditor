/*! \file    check.hpp
    \brief   This file contains global declarations for the test program.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef CHECK_H
#define CHECK_H

#define CLEAR_SCREEN  scr::clear( 1, 1, 80, 25, scr::WHITE | scr::REV_BLACK )
#define HOME_CURSOR   scr::set_cursor_position( 1, 1 )

void basic_test( );
void key_test( );

#endif
