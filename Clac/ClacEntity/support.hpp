/*! \file    support.hpp
    \brief   Declarations of various support functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include "Entity.hpp"

void    error_message( const char *, ... );
void    underflow( );
Entity *type_mismatch( Entity * );
Entity *type_mismatch( Entity *, Entity * );

double  to_radians( double );
double  from_radians( double );

// These functions are not standard, but they are common. We are implementing them ourselves to
// ensure they are available.
//
int  stricmp( char *, char * );

#endif

