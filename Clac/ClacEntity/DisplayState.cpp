/*! \file    DisplayState.cpp
    \brief   Implementation of clac's display state variables.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "DisplayState.hpp"

// Definitions of display state variables.
// 
DisplayState::angle_display_type   DisplayState::angle_mode    = DisplayState::DEGREES;
DisplayState::base_type            DisplayState::bit_base      = DisplayState::HEX;
DisplayState::complex_display_type DisplayState::complex_mode  = DisplayState::RECTANGULAR;
int                                DisplayState::decimal_count = 3;
DisplayState::float_display_type   DisplayState::display_mode  = DisplayState::FIXED;


void DisplayState::set_angle_mode( angle_display_type new_mode )
{
    angle_mode = new_mode;
}


void DisplayState::set_base( base_type new_base )
{
    bit_base = new_base;
}


void DisplayState::set_complex_mode( complex_display_type new_mode )
{
    complex_mode = new_mode;
}


void DisplayState::set_decimal_count( int number )
{
    decimal_count = number;
}


void DisplayState::set_display_mode( float_display_type new_mode )
{
    display_mode = new_mode;
}
