/*! \file    macro_stack.hpp
 *  \brief   Interface to the macro stack.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef MACRO_STACK_HPP
#define MACRO_STACK_HPP

#include "EditBuffer.hpp"
#include "mystack.hpp"
#include "WordSource.hpp"

// TODO: Make this global variable private to the macro_stack module.
extern Stack<WordSource *> macro_stack;

void get_word( EditBuffer &next_word );
void start_macro_string( const char *macro_text );
void start_macro_file( const char *file_name );

#endif
