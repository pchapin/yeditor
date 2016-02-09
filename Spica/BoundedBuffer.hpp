/*! \file    BoundedBuffer.hpp
 *  \brief   Interface to a bounded buffer template.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef BOUNDEDBUFFER_HPP
#define BOUNDEDBUFFER_HPP

#include <boost/thread.hpp>
#include "Semaphore.hpp"

namespace spica {

    template< typename ItemType, int Buffer_Size = 8 >
    class BoundedBuffer {
        BoundedBuffer( const BoundedBuffer & );
        BoundedBuffer &operator=( const BoundedBuffer & );

    public:
        BoundedBuffer( );
        void     push( const ItemType &item );
        ItemType pop( );

    private:
        ItemType     buffer[Buffer_Size];
        boost::mutex lock;      //!< Enforces mutually exclusive access.
        Semaphore    used;      //!< Number of used slots.
        Semaphore    free;      //!< Number of free slots.
        int          next_in;   //!< Next available slot.
        int          next_out;  //!< Oldest used slot.
    };


    template< typename ItemType, int Buffer_Size >
    BoundedBuffer< ItemType, Buffer_Size >::BoundedBuffer( )
        : used( 0 ), free( Buffer_Size ), next_in( 0 ), next_out( 0 )
    { }


    template< typename ItemType, int Buffer_Size >
    void BoundedBuffer< ItemType, Buffer_Size >::push( const ItemType &item )
    {
        // TODO: The behavior is not correct if an exception is thrown when the item is copied.
        free.down( );
        {
            boost::lock_guard< boost::mutex > guard( lock );
            buffer[next_in] = item;
            next_in++;
            if( next_in >= Buffer_Size ) next_in = 0;
        }
        used.up( );
    }


    template< typename ItemType, int Buffer_Size >
    ItemType BoundedBuffer< ItemType, Buffer_Size >::pop( )
    {
        ItemType item;

        // TODO: The behavior is not correct if an exception is thrown when the item is copied.
        used.down( );
        {
            boost::lock_guard< boost::mutex > guard( lock );
            item = buffer[next_out];
            next_out++;
            if( next_out >= Buffer_Size ) next_out = 0;
        }
        free.up( );
        
        return item;
    }
    
}
#endif
