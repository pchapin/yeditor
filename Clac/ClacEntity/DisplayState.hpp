/*! \file    DisplayState.hpp
    \brief   Interface to clac's display state variables.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

Class DisplayState is a wrapper around all of clac's display state variables. Such variables
specify how certain objects are to be displayed. Having all the display state variables in one
place like this makes it easier to keep track of them.

This class manages access to the display state variables and forces them to be refered to only
with qualified names. This helps to organize them. It might be better to use a namespace for
this purpose. However, this class was originally designed before namespaces were a part of C++
and we don't see any reason to change this design now.
*/

#ifndef DISPLAYSTATE_HPP
#define DISPLAYSTATE_HPP

class DisplayState {
public:

    // The following types define the legal values for certain objects below.
    enum angle_display_type   { DEGREES, RADIANS, GRADIANS };
    enum base_type            { DECIMAL, BINARY, HEX, OCTAL };
    enum complex_display_type { RECTANGULAR, POLAR };
    enum float_display_type   { FIXED, SCIENTIFIC, ENGINEERING };

private:

    // The following define all the display state variables used in the program.

    static angle_display_type angle_mode;
      // Method used to display angles

    static base_type bit_base;
      // Base used for display of BinaryEntities.

    static complex_display_type complex_mode;
      // Mode used for the display of ComplexEntities.

    static int decimal_count;
      // Number of decimal places displayed for FloatEntities.

    static float_display_type display_mode;
      // Mode used for the display of floating point numbers.

public:
    // The following methods allow access to the display state variables.
    static angle_display_type   get_angle_mode( )    { return angle_mode;    }
    static base_type            get_base( )          { return bit_base;      }
    static complex_display_type get_complex_mode( )  { return complex_mode;  }
    static int                  get_decimal_count( ) { return decimal_count; }
    static float_display_type   get_display_mode( )  { return display_mode;  }

    // The following methods allow modifications to the display state variables.
    static void set_angle_mode    (angle_display_type   new_mode );
    static void set_base          (base_type            new_base );
    static void set_complex_mode  (complex_display_type new_mode );
    static void set_decimal_count (int                  number   );
    static void set_display_mode  (float_display_type   new_mode );
};

#endif
