/*! \file    get_switch.hpp
    \brief   Functions to read command line switches.
    \author  Allen I Holub

(C) Copyright 1985, 1987 Allen I Holub. All rights reserved. This program may be copied for
personal, non-profit use only. Information on this program can be found in the book "C chest and
Other C Treasures" by Allen Holub. (C) 1987. M & T Books. Pages 41-51.

I have made extensive modifications to this code. Most of my modifications are stylistic, but
some are more significant. In particular, I converted the code to C++.

Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef GET_SWITCH_HPP
#define GET_SWITCH_HPP

#include <iosfwd>

const int int_switch = 0;
const int bin_switch = 1;
const int chr_switch = 2;
const int str_switch = 3;

const int bad_switch = 1; // Errorlevel for bad switch found.

struct SwitchInfo{
    char   name;               // Name of the command line switch.
    int    type;               // Switch type (see above).
    int   *value;              // Pointer to variable.
    const char **pvalue;        // Pointer to variable for str_switch.
    const char  *help_message;  // Pointer to help message.
};

extern void print_usage( SwitchInfo *, int, std::ostream & );
extern int  get_switchs( int, char **, SwitchInfo *, int );

#endif
