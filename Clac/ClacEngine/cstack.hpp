/*! \file    name.cpp
    \brief   This file contains the interface to class Stack.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef CSTACK_H
#define CSTACK_H

#include "Entity.hpp"
#include "VeryLong.hpp"

class Stack {
private:
    enum { STACK_SIZE = 100 };

    Entity *data[STACK_SIZE];

public:
    Stack( );    // Initialize stack object to something sensible.
   ~Stack( );    // Get rid of all the objects on the stack as well as the stack itself.

    // Push an entity on the stack. Return ERROR on failure.
    bool push( Entity *item );

    // Pop an entity off the stack.
    Entity *pop( );

    // Get a copy of an Entity* from the stack. Return NULL if index out of bounds.
    Entity *get( const VeryLong &index );

    // Put a copy of the argument into stack level zero. The old object is deleted.
    void put( Entity *new_object );

    // Stack operations.
    void clear( );           // Drop everything off of the stack.
    void drop( );            // Delete level 0 of stack.
    bool dup( const VeryLong &level = 0 ); // Make a copy of 'level'. Return ERROR on failure.
    long height( );          // Depth of the stack.
    void roll_down( const VeryLong & );   // Roll stack down.
    void roll_up( const VeryLong & );     // Roll stack up.
    void rotate( );          // Exchange level 0 and 2.
    void swap( );            // Exchange level 0 and 1.
  };

#endif

