/*! \file    editlist.hpp
 *  \brief   Interface to class EditList
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef EDITLIST_HPP
#define EDITLIST_HPP

#include "mylist.hpp"

class EditBuffer;

//! List of pointers to EditBuffer objects.
/*!
 *  This class is a wrapper around List that implements a list of pointers to EditBuffer
 *  objects. The EditBuffers pointed at by the elements of this list must be dynamically
 *  allocated. They should not be shared between two instances of EditList, except perhaps
 *  temporarily. The destructor of EditList deletes all the pointers on the list as a service.
 *
 *  Note that the underlying List template allows one to put NULL pointers on the list. However,
 *  EditList do not allow this, trading in generality for an easier interface. In effect,
 *  EditList removes a level of indirection allowing its clients to deal with pointers to
 *  EditBuffers rather than pointers to pointers to EditBuffers.
 */
class EditList : private List<EditBuffer *> {
public:

    //lint -e{1509} The base class is private so virtualness of its destructor is not important.
    //! Destructor
    /*!
     * Deletes all the EditBuffers pointed at by the elements of the list. Be sure no EditBuffer
     * is shared between two EditList instances.
     */
   virtual ~EditList( )
        { clear( ); }

    //! Returns the next EditBuffer* in the list.
    /*!
     * \return NULL if there are no other elements.
     */
    EditBuffer *next( )
    {
        EditBuffer * const * const result = List<EditBuffer *>::next( );
        return( result == NULL ? NULL : *result );
    }

    //! Returns the previous EditBuffer* in the list.
    /*!
     * \return NULL if there are no other elements.
     */
    EditBuffer *previous( )
    {
        EditBuffer * const * const result = List<EditBuffer *>::previous( );
        return( result == NULL ? NULL : *result );
    }

    //! Inserts a new EditBuffer* before the list's current point.
    /*!
     * \param item A pointer to a dynamically allocated EditBuffer to be added to the list
     * before the list's current point.
     * \return The same pointer it is given.
     * \throws std::bad_alloc if insufficient memory.
     */
    EditBuffer *insert( EditBuffer *const item )
    {
        EditBuffer *const *const result = List<EditBuffer *>::insert( item );
        return( *result );
    }

    //! Returns the EditBuffer* at the list's current point.
    EditBuffer *get( )
    {
        EditBuffer *const *const result = List<EditBuffer *>::get( );
        return( result == NULL ? NULL : *result );
    }

    void clear( );

    //! Moves the list's current point to just past the end.
    void set_end( )
        { List<EditBuffer *>::jump_to( size( ) ); }

    // Make these names from the private base class public.
    using List<EditBuffer *>::current_index;
    using List<EditBuffer *>::erase;
    using List<EditBuffer *>::size;
    using List<EditBuffer *>::jump_to;
};

#endif
