/*! \file    macro_stack.cpp
 *  \brief   Implementation of the macro stack.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the implementation of the macro stack. This stack holds WordSource objects
 * of various types that provide macro text for execution by the editor's macro engine.
 * Interactive commands are managed by a special WordSource that converts keystrokes into macro
 * text by way of a (mutable) key association table. See WordSource.cpp for more information.
 *
 * The macro stack is a fully dynamic data structure so it can grow to arbitrary height. This
 * is necessary to support macros that make use of arbitrary recursion.
 */

#include <cstdlib>

#include "EditBuffer.hpp"
#include "macro_stack.hpp"
#include "WordSource.hpp"

Stack<WordSource *> macro_stack;

/*!
 * To be sure the macro_stack is properly initialized, the constructor of StackInitializer will
 * push a KeyboardWord object onto it. Creating a global object of this type just insures this
 * is done at program startup.
 */
class StackInitializer {
public:
    StackInitializer( );
};


StackInitializer::StackInitializer( )
{
    KeyboardWord *keyboard = new KeyboardWord;
    macro_stack.push( keyboard );
}

static StackInitializer setup_stack;


/*!
 * Get the next macro word. This function invokes the get_word method for the object currently
 * at the top of the macro stack. If that function returns false, it tries to pop that object
 * off the stack and kill it.
 */
void get_word( EditBuffer &next_word )
{
    // Ask the object at the top of the stack to get the next word. Keep trying until we find an
    // object that can comply. The KeyboardWord object at the bottom of the stack will comply;
    // this loop can't run forever.
    //
    bool result;
    do {
        WordSource *current_source = *macro_stack.get( );
        result = current_source->get_word( next_word );

        // If it couldn't do it, kill this WordSource and try the next.
        if( result == false ) {
            macro_stack.pop( current_source );
            delete current_source;
        }

    } while( result == false );
}


void start_macro_string( const char *macro_text )
{
    // Create a new StringWord containing the macro and push it onto the macro stack.
    StringWord *new_source = new StringWord( macro_text );
    macro_stack.push( new_source );
}


void start_macro_file( const char *file_name )
{
    // Create a FileWord containing the macro and push it onto the macro stack.
    FileWord *new_source = new FileWord( file_name );
    macro_stack.push( new_source );
}
