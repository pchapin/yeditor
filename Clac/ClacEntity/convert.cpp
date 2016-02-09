/*! \file    convert.cpp
 *  \brief   Type conversion functions.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <cmath>

#include "convert.hpp"
#include "Entities.hpp"

using namespace std;

//
// The follow array defines the automatic conversions done when operands of differing types meet
// in a binary expression. This array must be symmetric about the diagonal.
//
//
// FINISH ME! (When all the necessary conversion functions are defined).
#define E &Entity
Entity *( Entity::*convert_table[type_count][type_count] )( ) const = {
//           Bin            Cpx            Dir   Flt            Int            Lbl   Lst   Mat   Prg   Rat             Str           Vec
/* Bin */  { E::to_binary,  E::to_complex, NULL, E::to_float,   E::to_integer, NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Cpx */  { E::to_complex, E::to_complex, NULL, E::to_complex, NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Dir */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Flt */  { E::to_float,   E::to_complex, NULL, E::to_float,   E::to_float,   NULL, NULL, NULL, NULL, E::to_float,    NULL,         NULL },
/* Int */  { E::to_integer, NULL,          NULL, E::to_float,   E::to_integer, NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Lbl */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Lst */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Mat */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Prg */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL },
/* Rat */  { NULL,          NULL,          NULL, E::to_float,   NULL,          NULL, NULL, NULL, NULL, E::to_rational, NULL,         NULL },
/* Str */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           E::to_string, NULL },
/* Vec */  { NULL,          NULL,          NULL, NULL,          NULL,          NULL, NULL, NULL, NULL, NULL,           NULL,         NULL }
};
