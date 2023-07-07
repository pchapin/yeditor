/*! \file    mystack.hpp
 *  \brief   General purpose Stack template.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef MYSTACK_HPP
#define MYSTACK_HPP

#include "mylist.hpp"

template< typename T >
class Stack : private List<T> {
public:
    /*!
     * Put an object of type T onto the stack. Objects of type T must be copyable as with a copy
     * constructor.
     */
    void push( const T & );

    /*!
     * Assigns the object on the top of the stack into the argument. If there is nothing on the
     * stack, the argument is unchanged. The object on the top of the stack is deleted.
     */
    void pop( T & );

    /*!
     * Deletes the element on the top of the stack. If there is nothing in the stack, this
     * function has no effect.
     */
    void delete_top( );

    // Make these names from the private base class public.
    using List<T>::size;
    using List<T>::get;
};


template< typename T >
void Stack<T>::push( const T &new_object )
{
    List<T>::insert( new_object );
    List<T>::previous( );
}


template< typename T >
void Stack<T>::pop( T &old_object )
{
  if( size( ) == 0 ) return;
  old_object = *get( );
  List<T>::erase( );
}


template< typename T >
void Stack<T>::delete_top( )
{
    List<T>::erase( );
}

#endif
