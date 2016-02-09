/*! \file    cstack.cpp
    \brief   This file contains...
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "cstack.hpp"
#include "support.hpp"

Stack::Stack( )
{
    for( size_t i = 0; i < STACK_SIZE; i++ ) {
        data[i] = NULL;
    }
}


Stack::~Stack( )
{
    for( size_t i = 0; i < STACK_SIZE; i++ ) {
        delete data[i];
    }
}


bool Stack::push( Entity *item )
{
    delete data[STACK_SIZE-1];
    for( size_t i = STACK_SIZE-1; i > 0; i-- ) {
        data[i] = data[i-1];
    }
    data[0] = item;
    return true;
}


Entity *Stack::pop( )
{
    Entity *return_value = data[0];

    for( size_t i = 0; i < STACK_SIZE-1; i++ ) {
        data[i] = data[i+1];
    }
    data[STACK_SIZE-1] = NULL;
    return return_value;
}


Entity *Stack::get( const VeryLong &index )
{
    Entity *return_value = NULL;

    if( index < STACK_SIZE ) {
        return_value = data[index.to_long( )];
    }
    return return_value;
}


void Stack::put( Entity *new_object )
{
    delete data[0];
    data[0] = new_object;
}


void Stack::clear( )         // Drop everything from the stack.
{
    size_t i;
    for( i = 0; i <= STACK_SIZE && data[i] != NULL; i++ ) {
        delete data[i];
        data[i] = NULL;
    }
}


void Stack::drop( )           // Discard level 1 of the stack.
{
    Entity *temp = pop( );    // pop item off of the stack.
    if( temp == NULL )
        error_message( "Can't drop from an empty stack" );
    delete temp;
}


long Stack::height( )  // Depth of the stack.
{
    long C = 0;
    for( size_t i = 0; i < STACK_SIZE-1; i++ ) {
        if( data[i] != NULL )
            C++;
    }
    return C;
}


void Stack::roll_down( const VeryLong &C )
{
    long count = C.to_long( );
    if( count <= 0 ) count = 1;
    if( count > height( ) )
        error_message( "Stack not high enough to roll" );
    else {
        Entity *temp  = data[0];
        for( size_t i = 0; i < count-1; i++ ) {
            data[i] = data[i+1];
        }
        data[count-1] = temp;
    }
}


void Stack::roll_up( const VeryLong &C )
{
    long count = C.to_long( );
    if( count <= 0 ) count = 1;
    if( count > height( ) )
        error_message( "Stack not high enough to roll" );
    else {
        Entity *temp  = data[count-1];
        for( size_t i = count-1; i > 0; i-- ) {
            data[i] = data[i-1];
        }
        data[0] = temp;
    }
}


void Stack::rotate( )    // Perform a 3 Roll_Up.
{
    if( data[0] != NULL && data[1] != NULL && data[2] != NULL ) {
        Entity *temp = data[2];
        data[2] = data[1];
        data[1] = data[0];
        data[0] = temp;
    }
    else {
        error_message( "Too few arguments" );
    }
}


void Stack::swap( )    // Perform a swap of levels 1 and 2.
{
    if( data[0] != NULL && data[1] != NULL ) {
        Entity *temp = data[0];
        data[0] = data[1];
        data[1] = temp;
    }
    else {
        error_message( "Too few arguments" );
    }
}
