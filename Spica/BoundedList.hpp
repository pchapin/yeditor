/*! \file    BoundedList.hpp
 *  \brief   List template that preallocates memory.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * Bounded lists allocate a block of memory when they are constructed and will only use that
 * allocated block. Thus if construction is successful they will never run out of memory
 * unexpectedly. Of course they may run out of space in their allocated block so checks of that
 * nature may still be necessary in some cases.
 */

#ifndef BOUNDEDLIST_HPP
#define BOUNDEDLIST_HPP

#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>

namespace spica {

    //! Linked list with preallocated memory and upper bound on size.
    /*!
     * Bounded lists store elements in an array that is allocated at the time of construction.
     * The size of this array is never changed once it is allocated. This means that the number
     * of elements that can be stored in a bounded list has an upper bound that is set when the
     * list object is initialized. It also means, however, that memory allocation errors are
     * impossible (with respect to the list at least) once the constructor has returned.
     *
     * Internally, bounded lists reserve the first location of the storage array (index 0) to
     * hold a sentinel node. This node is never initialized but its pointers to the next and
     * previous node are used.
     */
    template< typename T >
    class BoundedList {

        BoundedList( const BoundedList & ) = delete;
        BoundedList &operator=( const BoundedList & ) = delete;

    public:

        // The usual typedef names.
        typedef       T  value_type;
        typedef       T *pointer;
        typedef const T *const_pointer;
        typedef       T  &reference;
        typedef const T  &const_reference;
        typedef std::size_t    size_type;
        typedef std::ptrdiff_t difference_type;

    private:
        T         *raw;       // Pre-allocated block of raw memory.
        size_type *next;      // Array of "next" indicies.
        size_type *previous;  // Array of "previous" indicies.
        size_type  count;     // Number of items in list.
        size_type  capacity;  // Size of pre-allocated block.
        size_type  free;      // Front of the free list.

    public:

        //! Bounded list iterators.
        /*!
         * The nested class iterator describes objects that know how to step up and down a
         * bounded_list. The iterators for bounded_list are bidirectional.
         */
        class iterator :
            public std::iterator< std::bidirectional_iterator_tag, T > {

            friend class BoundedList;

        private:
            BoundedList *my_list;                     // BoundedList into which we are pointing.
            typename BoundedList::size_type my_node;  // Node to which we are pointing.

        public:
            //! Preincrement.
            /*!
             * Advances iterator to next item in the list and returns a reference to itself.
             */
            iterator &operator++( )
                { my_node = my_list->next[my_node]; return( *this ); }

            //! Postincrement.
            /*!
             * Advances iterator to next item in the list and returns original iterator.
             */
            iterator operator++( int )
                { iterator temp( *this ); my_node = my_list->next[my_node]; return temp; }

            //! Predecrement.
            /*!
             * Backs up iterator to previous item in list and returns a reference to itself.
             */
            iterator &operator--( )
                { my_node = my_list->previous[my_node]; return( *this ); }

            //! Postdecrement.
            /*!
             * Backs up iterator to previous item in the list and returns original iterator.
             */
            iterator operator--( int )
                { iterator temp( *this ); my_node = my_list->previous[my_node]; return temp; }

            //! Returns true if *this and other point at the same object.
            bool operator==( const iterator &other )
                { return( my_node == other.my_node ); }

            //! Returns true if *this and other point at different objects.
            bool operator!=( const iterator &other )
                { return( my_node != other.my_node ); }

            //! Returns a reference to the item the iterator is pointing at.
            reference operator*( )
                { return( my_list->raw[my_node] ); }

            //! Returns a pointer to the item the iterator is pointing at.
            pointer operator->( )
                { return( &my_list->raw[my_node] ); }

            // Constructs a bounded list iterator.
            /*!
             * \param list Pointer to the list into which the iterator will point.
             * \param index Index into the storage array used by the list where the target of
             * the iterator is located.
             */
            iterator( BoundedList *list, size_type index ) :
                my_list( list ), my_node( index )
                { }

            //! Default constructor.
            iterator( ) : my_list( 0 ), my_node( 0 )
                { }

        }; // End of BoundedList<T>::iterator

        friend class BoundedList::iterator;

        BoundedList( size_type max_count );
       ~BoundedList( ) noexcept;

        //! Return the number of elements currently on the list. O(1)
        size_type size( ) const noexcept
            { return( count ); }

        //! Return true if the list contains no elements.
        bool empty( ) const noexcept
            { return( count == 0 ); }

        //! Returns the maximum size the list can be.
        size_type max_size( ) const noexcept
            { return( capacity ); }

        iterator begin( )
            { return( iterator( this, next[0] ) ); }

        iterator end( )
            { return( iterator( this, 0 ) ); }

        //! Returns a reference to the first item on the list. O(1)
        reference front( )
            { return( raw[next[0]] ); }

        const_reference front( ) const
            { return( raw[next[0]] ); }

        //! Appends item to the end of the list. O(1)
        /*!
         * \param item Reference to the new item. The type T must be copyable.
         * \exception std::length_error if the list is full. In this case, the new item is not
         * copied. Bounded lists never increase in capacity once they are created.
         */
        void push_back( const T &item );

        template<typename ForwardIterator>
        void push_back( ForwardIterator first, ForwardIterator last );

        //! Returns a reference to the last item on the list. O(1)
        reference back( )
            { return( raw[previous[0]] ); }

        const_reference back( ) const
            { return( raw[previous[0]] ); }

        //! Removes the last item on the list. O(1)
        void pop_back( );

        //! Inserts a new item into the list.
        /*!
         * This function executes in constant time.
         *
         * \param pos An iterator pointing into the list. The new item will be inserted before
         * the element pointed at by this iterator.
         * \param item The new item to be inserted. The type T must be copyable.
         * \return An iterator that points at the new item as it exists on the list.
         * \exception std::length_error if there is insufficient space in the list for the new
         * item. In this case, the new item is not copied. Bounded lists never increase in
         * capacity once they are created.
         */
        iterator insert( iterator pos, const T &item );

        //! Erase an item from the list.
        /*!
         * This function executes in constant time.
         *
         * \param pos An iterator pointing at the element to erase. If this is an off-the-end
         * iterator, the effect is undefined.
         * \return An iterator that points at the element that was just past the element erased.
         */
        iterator erase( iterator pos );

    };  // End of BoundedList<T>


    template< typename T >
    BoundedList< T >::BoundedList( size_type max_count )
    {
        std::unique_ptr<char[]> temp_raw( new char[(max_count + 1) * sizeof(T)] );
        std::unique_ptr<size_type> temp_next( new size_type[max_count+1] );
        std::unique_ptr<size_type> temp_previous( new size_type[max_count+1] );

        raw      = reinterpret_cast<T *>( temp_raw.release( ) );
        next     = temp_next.release( );
        previous = temp_previous.release( );
        count    = 0;
        capacity = max_count;
        free     = 0;

        // Prepare the free list.
        for( size_type i = 1; i <= capacity; ++i ) {
            next[i] = i + 1;
        }
        if( capacity > 0 ) {
            next[capacity] = 0;
            free = 1;
        }

        // Configure the sentinel.
        next[0] = 0;
        previous[0] = 0;
    }


    template< typename T >
    BoundedList< T >::~BoundedList( ) noexcept
    {
        // Destroy all the active items.
        size_type current = next[0];
        while( current != 0 ) {
            raw[current].~T( );
            current = next[current];
        }

        // Release the memory.
        delete [] previous;
        delete [] next;
        delete [] reinterpret_cast< char * >( raw );
    }


    template< typename T >
    void BoundedList< T >::push_back( const T &item )
    {
        if( free == 0 )
            throw std::length_error( "BoundedList: full; can't increase capacity" );

        // Pull a slot off the free list.
        size_type new_item = free;

        // Construct a copy of the item in the desired slot.
        new ( &raw[new_item] ) T( item );

        free = next[free];
        next[previous[0]]  = new_item;
        previous[new_item] = previous[0];
        next[new_item]     = 0;
        previous[0]        = new_item;

        ++count;
    }

    template< typename T >
    template< typename ForwardIterator >
    void BoundedList<T>::push_back( ForwardIterator first, ForwardIterator last )
    {
        while( first != last ) {
            push_back( *first );
            ++first;
        }
    }


    template< typename T >
    void BoundedList< T >::pop_back( )
    {
        // Remove the last item (I assume one is present).
        size_type p = previous[previous[0]];
        raw[previous[0]].~T( );
        next[previous[0]] = free;
        free = previous[0];

        previous[0] = p;
        next[p] = 0;
        count--;
    }


    template< typename T >
    typename BoundedList< T >::iterator
        BoundedList< T >::insert( iterator pos, const T &item )
    {
        size_type p;

        // If there is no space for the new item, just throw and die.
        if( count == capacity )
            throw std::length_error( "BoundedList: full; can't increase capacity" );

        // Locate a free slot and construct the incoming item into it.
        p = free;
        new ( &raw[p] ) T( item );

        free = next[free];
        previous[p] = previous[pos.my_node];
        next[p] = pos.my_node;
        next[previous[pos.my_node]] = p;
        previous[pos.my_node] = p;

        count++;
        return( iterator( this, p ) );
    }


    template< typename T >
    typename BoundedList< T >::iterator
        BoundedList< T >::erase( iterator pos )
    {
        size_type p = pos.my_node;
        size_type temp = next[p];
        next[previous[p]] = next[p];
        previous[next[p]] = previous[p];

        raw[p].~T( );
        next[p] = free;
        free = p;
        count--;
        return( iterator(this, temp) );
    }
}

#endif
