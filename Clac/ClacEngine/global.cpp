/*! \file    global.cpp
    \brief   This file contains the implementation of clac's global state.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "global.hpp"

namespace global {

    // The global variables themselves.
    BaseType        current_base;
    AngleModeType   current_angle_mode;
    VeryLong        current_decimal_count;
    DisplayModeType current_display_mode;
    ComplexModeType current_complex_mode;
    VeryLong        current_bit_count;
    MasterStream    current_word_source;
    Stack           current_stack;

    // Accessor functions.
    void set_base( BaseType new_base )
        { current_base = new_base; }

    void set_angle_mode( AngleModeType new_angle_mode )
        { current_angle_mode = new_angle_mode; }

    void set_decimal_count( const VeryLong &new_decimal_count )
        {  current_decimal_count = new_decimal_count; }

    void set_display_mode( DisplayModeType new_display_mode )
        { current_display_mode = new_display_mode; }

    void set_complex_mode( ComplexModeType new_complex_mode )
        { current_complex_mode = new_complex_mode; }

    void set_bit_count( const VeryLong &new_bit_count )
        { current_bit_count = new_bit_count; }

    AngleModeType get_angle_mode( )
        { return current_angle_mode; }

    BaseType get_base( )
        { return current_base; }

    ComplexModeType get_complex_mode( )
        { return current_complex_mode; }

    DisplayModeType get_display_mode( )
        { return current_display_mode; }

    MasterStream &word_source( )
        { return current_word_source; }

    Stack &the_stack( )
        { return current_stack; }

}
