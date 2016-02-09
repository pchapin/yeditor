/*! \file    HashtableOpen.hpp
    \brief   An open addressing (with linear probing) style hash table.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef HASHTABLEOPEN_HPP
#define HASHTABLEOPEN_HPP

#include <cstddef>
#include <iterator>

namespace spica {

    template< typename T, typename Hasher = int ( * )( const T & ) >
    class HashtableOpen {

    public:
        // Some of the types normally made available by associative containers. This list is
        // incomplete, but it's good enough for now.
        //
        typedef const T     value_type;
        typedef const T     key_type;
        typedef std::size_t size_type;

        // To use this container with the generic algorithms, I will need to create an iterator
        // class. This particular iterator class has the semantics of a const_iterator. This is
        // necessary because I can not allow the user to modify items while they are in the
        // table.
        //
        class iterator :
            public std::iterator< std::bidirectional_iterator_tag, const T > {

            friend class HashtableOpen;

        public:
            typename HashtableOpen< T, Hasher >::value_type &operator*( ) const;
            // Overloaded indirection.

            iterator &operator++( );
            iterator &operator--( );
            // Step forwards and backwards through the table. These functions implement the
            // prefix operations. They return a reference to the object they are invoked against
            // *after* they have modified that object. Postfix operations can also be
            // implemented but are not part of this version.

            bool operator==( const iterator & ) const;
            // Two iterators are equal if they point to the same object.

            bool operator!=( const iterator & ) const;
            // The obvious meaning.

        private:
            const HashtableOpen *table;    // Points at the table object.
                  size_type      current;  // Index to current item in the table.
        };

        friend class HashtableOpen::iterator;

        HashtableOpen( size_type size, Hasher function );
        // Initializes the hash table by creating 'size' buckets and remembering the hash
        // function.

       ~HashtableOpen( );
        // Destroys objects in the table.

        size_type size( ) const;
        // Returns the number of items in the table.

        size_type bucket_count( ) const;
        // Returns the number of buckets in the table.

        bool empty( ) const;
        // Returns true if the size() is zero and false otherwise.

      std::pair< iterator, bool > insert( const key_type & );
        // Copies the argument into the table. If it is already in the table (according to
        // operator==() for type T), there is no effect on the table. It returns an iterator to
        // the object as it exists in the table. The second member of the pair is a flag. It has
        // the value true if the new item was inserted and false if the item was already in the
        // table.
        
        iterator find( const key_type & ) const;
        // Finds an object in the table that "matches" the argument according to operator==()
        // for type T. It returns an iterator to the object if it exists in the table. Otherwise
        // it returns an iterator that compares equal to end().

        iterator begin( ) const;
        iterator end( ) const;
        // Functions for producing scanning over the table items.

    private:
        T        *table;         // Points at the table itself.
        size_type nbuckets;      // Number of buckets in the table.
        size_type item_count;    // Number of items in the table.
        bool     *freemap;       // Entries are marked trued if they are free.
        Hasher    hash_function; // The hash function.
    };


    //
    // HashtableOpen<T, Hasher>::HashtableOpen
    //
    // The constructor initializes the members of the hash table.
    //
    template< typename T, typename Hasher >
    HashtableOpen< T, Hasher >::HashtableOpen( size_type size, Hasher function )
    {
        table         = reinterpret_cast< T * >( new char[size * sizeof( T )] );
        freemap       = new bool[size];
        nbuckets      = size;
        item_count    = 0;
        hash_function = function;

        // Initialize the free map.
        for( size_type i = 0; i < nbuckets; ++i ) {
            freemap[i] = true;
        }
    }
    

    //
    // HashtableOpen<T, Hasher>::~HashtableOpen
    //
    // The destructor removes the objects currently in the table.
    //
    template< typename T, typename Hasher >
    HashtableOpen< T, Hasher >::~HashtableOpen( )
    {
        // Explicitly destroy existing table items.
        for( size_type i = 0; i < nbuckets; ++i ) {
            if( !freemap[i] ) table[i].~T( );
        }

        // Don't forget to release the memory allocated earlier.
        delete [] freemap;
        delete [] reinterpret_cast< char * >( table );
    }


    //
    // HashtableOpen<T, Hasher>::size
    //
    // This function returns the number of items in the table.
    //
    template< typename T, typename Hasher >
    inline
    typename HashtableOpen< T, Hasher >::size_type
        HashtableOpen< T, Hasher >::size( ) const
    {
        return item_count;
    }


    //
    // HashtableOpen<T, Hasher>::bucket_count
    //
    // This function returns the number of buckets that have been allocated for the hash table.
    //
    template< typename T, typename Hasher >
    inline
    typename HashtableOpen< T, Hasher >::size_type
        HashtableOpen< T, Hasher >::bucket_count( ) const
    {
        return nbuckets;
    }


    //
    // HashtableOpen<T, Hasher>::empty
    //
    // This function returns true if the hash table is empty and false otherwise.
    //
    template< typename T, typename Hasher >
    inline
    bool HashtableOpen< T, Hasher >::empty( ) const
    {
        return !size( );
    }


    //
    // HashtableOpen<T, Hasher>::insert
    //
    // This function inserts a new key value into the table.
    //
    template< typename T, typename Hasher >
    std::pair< typename HashtableOpen< T, Hasher >::iterator, bool >
        HashtableOpen< T, Hasher >::insert( const key_type &key )
    {
        size_type index;
        iterator  result;

        // Is there space?
        if( item_count == nbuckets )
            throw std::length_error( "hash table: full; can't increase capacity" );

        // Compute the table index where this object might be.
        index  = hash_function( key );
        index %= nbuckets;

        // Search for a free bucket using linear probing. Since there is at least one free slot,
        // this loop will terminate eventually. In the normal case, this loop should be skipped
        // immediately (that is, the desired bucket should be available most of the time).
        // 
        while( !freemap[index] ) {

            // If the items is already in the table, indicate as much to the caller.
            if( table[index] == key ) {
                result.table = this;
                result.current = index;
                return std::pair< iterator, bool >( result, false );
            }

            // Try the next bucket.
            ++index;
            if( index > nbuckets ) index = 0;
        }

        // Install new item.
        new ( &table[index] ) T( key );
        freemap[index] = false;
        ++item_count;

        // Set up iterator for return.
        result.table = this;
        result.current = index;

        return std::pair< iterator, bool >( result, true );
    }


    //
    // HashtableOpen<T, Hasher>::find
    //
    // This function looks up an item by its value and returns an iterator into the table of
    // where it is located. If the item is not in the table, it returns an iterator that
    // compares equal to end().
    //
    template< typename T, typename Hasher >
    typename HashtableOpen< T, Hasher >::iterator
        HashtableOpen< T, Hasher >::find( const key_type &key ) const
    {
        size_type index;
        size_type original_index;
        iterator  result;

        // Initialize result to be the same as the end() iterator.
        result.table = this;
        result.current = nbuckets;

        // Compute the table index where this object might be.
        index  = hash_function( key );
        index %= nbuckets;
        original_index = index;

        while( freemap[index] == false ) {
            if( table[index] == key ) {
                result.current = index;
                break;
            }
            ++index;
            if( index == nbuckets ) index = 0;
            if( index == original_index ) break;
        }

        return result;
    }


    //
    // HashtableOpen<T, Hasher>::begin
    //
    // This function returns an iterator to the first element in the table. If the table is
    // empty, it will return an iterator that compares equal to that returned by end().
    //
    template< typename T, typename Hasher >
    typename HashtableOpen< T, Hasher >::iterator
        HashtableOpen< T, Hasher >::begin( ) const
    {
        iterator result;

        // Start at the top of the table and look for the first filled bucket.
        size_type i;
        for( i = 0; i < nbuckets; ++i ) {
            if( !freemap[i] ) break;
        }

        // Prepare iterator. Notice that if the table is empty, i will be nbuckets as desired.
        // See comments in end() for more information.
        //
        result.table = this;
        result.current = i;

        return result;
    }


    //
    // HashtableOpen<T, Hasher>::end
    //
    // This function returns an iterator that points just past the last
    // list in the table.
    //
    template< typename T, typename Hasher >
    typename HashtableOpen< T, Hasher >::iterator
        HashtableOpen< T, Hasher >::end( ) const
    {
        iterator result;

        // The "just off the end" iterator will use a just off the end bucket index. Other
        // representations are probably possible, but this one certainly seems natural.
        // 
        result.table = this;
        result.current = nbuckets;

        return result;
    }


    //
    // HashtableOpen<T, Hasher>::iterator::operator*
    //
    // Implements the indirection operation as applied to a hash table iterator. It returns a
    // reference to the currently "pointed at" item in the table.
    //
    template< typename T, typename Hasher >
    inline
    typename HashtableOpen< T, Hasher >::value_type &
        HashtableOpen< T, Hasher >::iterator::operator*( ) const
    {
        return table->table[current];
    }


    //
    // HashtableOpen<T, Hasher>::iterator::operator++
    //
    // Increments the iterator so that it points at the next item and then returns a reference
    // to the modified iterator.
    //
    template< typename T, typename Hasher >
    typename HashtableOpen< T, Hasher >::iterator &
        HashtableOpen< T, Hasher >::iterator::operator++( )
    {
        ++current;

        // Now look for a new filled bucket.
        while( current < table->nbuckets ) {
            if( !table->freemap[current] ) break;
            ++current;
        }
        return *this;
    }


    //
    // HashtableOpen<T, Hasher>::iterator::operator--
    //
    // Decrements the iterator so that it points at the previous item and then returns a
    // reference to the modified iterator.
    //
    template< typename T, typename Hasher >
    typename HashtableOpen< T, Hasher >::iterator &
        HashtableOpen< T, Hasher >::iterator::operator--( )
    {
        // Notice that this method will do bad things if someone attempts to decrement an
        // iterator that points at the first item in the table. This is okay, however, because
        // such an operation is undefined anyway. It is necessary to support incrementing just
        // past the end, but it is not necessary to support decrementing just before the
        // beginning.
        // 
        while( table->freemap[--current] ) ;
        return *this;
    }


    //
    // HashtableOpen<T, Hasher>::iterator::operator==
    //
    // Compares two hash table iterators and returns true if they point at the same element.
    //
    template< typename T, typename Hasher >
    bool HashtableOpen< T, Hasher >::iterator::operator==( const iterator &other ) const
    {
        if( table   == other.table   &&
            current == other.current ) return true;
        return false;
    }


    //
    // HashtableOpen<T, Hasher>::iterator::operator!=
    //
    // The obvious meaning.
    //
    template< typename T, typename Hasher >
    inline
    bool HashtableOpen< T, Hasher >::iterator::operator!=( const iterator &other ) const
    {
        return !( *this == other );
    }

}

#endif
