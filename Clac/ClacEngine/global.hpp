/*! \file    global.hpp
    \brief   This file contains the interface to clac's global state.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "VeryLong.hpp"

#include "cstack.hpp"
#include "WordStream.hpp"

namespace global {

    enum BaseType         { DECIMAL, HEX, OCTAL, BINARY };
    enum AngleModeType    { RAD, DEG, GRAD };
    enum DisplayModeType  { FIXED, SCIENTIFIC, ENGINEERING };
    enum ComplexModeType  { RECTANGULAR, POLAR };

    void set_angle_mode   (       AngleModeType   new_angle_mode    );
    void set_base         (       BaseType        new_base          );
    void set_bit_count    ( const VeryLong       &new_bit_count     );
    void set_complex_mode (       ComplexModeType new_complex_mode  );
    void set_decimal_count( const VeryLong       &new_decimal_count );
    void set_display_mode (       DisplayModeType new_display_mode  );

    AngleModeType   get_angle_mode( );
    BaseType        get_base( );
    ComplexModeType get_complex_mode( );
    DisplayModeType get_display_mode( );

    MasterStream &word_source( );
    Stack        &the_stack( );
}

#endif
