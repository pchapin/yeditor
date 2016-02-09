/*! \file    words.hpp
    \brief   This file contains functions that recognize numeric literals.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef WORDS_HPP
#define WORDS_HPP

extern bool is_complex ( const char * );
extern bool is_integer ( const char * );
extern bool is_float   ( const char * );
extern bool is_rational( const char * );

#endif
