/*! \file    support.hpp
 *  \brief   Declarations of various supporting functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include <string>
#include "environ.hpp"
#include "EditBuffer.hpp"

char *adjust_date( const char *raw_date );

//! Given offset into string, returns offset of next word to the right.
unsigned word_right( const EditBuffer &, unsigned );

//! Given offset into string, returns offset of next word to the left.
unsigned word_left( const EditBuffer &, unsigned );

//! Given list of file names, wildcard sequences, and switches, loads all indicated files.
bool load_files( const char **argv );

//! Adds the string "awk -f " in front of every word which has a .awk in it.
void insert_AWK( std::string &working_buffer );

// Similar to _stricmp and _strnicmp in the Open watcom library, but more cross-platform.
int my_stricmp( const char *s1, const char *s2 );
int my_strnicmp( const char *s1, const char *s2, int n );

void info_message( const char *format, ... );
bool confirm_message( const char *string, char non_default, bool ESC_default );
void warning_message( const char *format, ... );
void error_message( const char *format, ... );
void memory_message( const char *string );

#endif

