/*! \file    EditList.cpp
 *  \brief   Implementation of class EditList
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "EditBuffer.hpp"
#include "EditList.hpp"

//! Removes all the EditBuffers in the list.
/*!
 * This function clears the list without destroying it. The EditBuffers pointed at by the list
 * elements are removed.
 */
void EditList::clear( )
{
    EditBuffer *p;

    jump_to( 0 );
    while ( (p = next( ) ) != NULL ) {
        delete p;
    }
    List< EditBuffer * >::clear( );
}
