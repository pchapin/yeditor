/*! \file    sorters.hpp
    \brief   Various sorting function templates.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

    This file contains function templates that implement various sorting algorithms. Each
    algorithm has its own strengths and weaknesses. See the documentation on each algorithm for
    specifics about its running time, limitations, etc.
*/

#ifndef SORTERS_HPP
#define SORTERS_HPP

#include <algorithm>
#include <iterator>
#include <vector>

namespace spica {

    //! Sorts using the Bubble Sort algorithm.
    /*!
     * The Bubble Sort algorithm runs in O(n^2) on average and sorts in place.
     *
     * \param first Forward iterator pointing at the start of the sequence.
     * \param last  Forward iterator pointing just past the end of the sequence.
     * \param comp  Comparison object that is a model of a strict weak ordering.
     * \todo Review for exception and thread safety.
     */
    template< typename Forward, typename StrictWeakOrdering >
    void bubble_sort( Forward first, Forward last, StrictWeakOrdering comp )
    {
        using std::swap;
        if( first == last ) return;

        bool another_pass = true;
        while( another_pass ) {

            another_pass = false;
            Forward p1 = first;
            Forward p2 = first;
            ++p2;

            while( p2 != last ) {
                if( comp( *p2, *p1 ) ) {
                    swap( *p1, *p2 );
                    another_pass = true;
                }
                ++p1;
                ++p2;
            }
        }
    }


    //! Sorts using the Selection Sort algorithm.
    /*!
     * The Selection Sort algorithm runs in O(n^2) on average and sorts in place.
     *
     * \param first Forward iterator pointing at the start of the sequence.
     * \param last  Forward iterator pointing just past the end of the sequence.
     * \param comp  Comparison object that is a model of a strict weak ordering.
     * \todo Review for exception and thread safety.
     */
    template< typename Forward, typename StrictWeakOrdering >
    void selection_sort( Forward first, Forward last, StrictWeakOrdering comp )
    {
        using std::swap;

        while( first != last ) {
            Forward smallest = first;
            Forward scanner  = first;
            ++scanner;

            while( scanner != last ) {
                if ( comp( *scanner, *smallest ) ) smallest = scanner;
                ++scanner;
            }
            if( first != smallest ) swap( *first, *smallest );
            ++first;
        }
    }

  
    //! Sorts using the Insertion Sort algorithm.
    /*!
     * The Insertion Sort algorithm runs in O(n^2) on average and sorts in place.
     *
     * \param first Bidirectional iterator pointing at the start of the sequence.
     * \param last  Bidirectional iterator pointing just past the end of the sequence.
     * \param comp  Comparison object that is a model of a strict weak ordering.
     * \todo Review for exception and thread safety.
     */
    template< typename Bidirectional, typename StrictWeakOrdering >
    void insertion_sort( Bidirectional first, Bidirectional last, StrictWeakOrdering comp )
    {
        using std::swap;
        if( first == last ) return;

        Bidirectional current = first;
        ++current;
        while( current != last ) {
            Bidirectional p1 = current;
            Bidirectional p2 = current;
            --p2;

            while( comp( *p1, *p2 ) ) {
                swap( *p1, *p2 );
                if( p2 == first ) break;
                --p1;
                --p2;
            }
            ++current;
        }
    }


    //! Sorts using the Merge Sort algorithm.
    /*!
     * The Merge Sort algorithm runs in O(n log(n)) on average but requires O(n) memory.
     *
     * \param first RandomAccess iterator pointing at the start of the sequence.
     * \param last  RandomAccess iterator pointing just past the end of the sequence.
     * \param comp  Comparison object that is a model of a strict weak ordering.
     * \todo Review for exception and thread safety.
     */
    template< typename RandomAccess, typename StrictWeakOrdering >
    void merge_sort( RandomAccess first, RandomAccess last, StrictWeakOrdering comp )
    {
        typename std::iterator_traits< RandomAccess >::difference_type size = last - first;
        if( size <= 1 ) return;

        RandomAccess midpoint = first + size/2;
        merge_sort( first, midpoint, comp );
        merge_sort( midpoint, last, comp );

        // Do a merge operation.
        RandomAccess p1 = first;
        RandomAccess p2 = midpoint;
        std::vector< typename std::iterator_traits< RandomAccess >::value_type > workspace;

        while( p1 != midpoint || p2 != last ) {
            if( p1 == midpoint ) {
                workspace.push_back( *p2++ );
            }
            else if( p2 == last ) {
                workspace.push_back( *p1++ );
            }
            else if( comp( *p1, *p2 ) ) {
                workspace.push_back( *p1++ );
            }
            else {
                workspace.push_back( *p2++ );
            }
        }
        std::copy( workspace.begin( ), workspace.end( ), first );
    }

    // Quick Sort helper function templates.

    template< typename RandomAccess, typename StrictWeakOrdering >
    RandomAccess quick_sort_partition( RandomAccess p, RandomAccess r, StrictWeakOrdering comp )
    {
        using std::swap;

        typename std::iterator_traits< RandomAccess >::value_type pivot = *r;
        RandomAccess i = p - 1;  // I don't like this.
        for( RandomAccess j = p; j <= r - 1; ++j ) {
            if( comp( *j, pivot ) ) {
                ++i;
                swap( *i, *j );
            }
        }
        swap( *(i + 1), *r );
        return( i + 1 );
    }

    template< typename RandomAccess, typename StrictWeakOrdering >
    void quick_sort_guts( RandomAccess p, RandomAccess r, StrictWeakOrdering comp )
    {
        if( p < r ) {
            RandomAccess q = quick_sort_partition( p, r, comp );
            quick_sort_guts( p, q - 1, comp );
            quick_sort_guts( q + 1, r, comp );
        }
    }

    //! Sorts using the Quick Sort algorithm.
    /*!
     * The Quick Sort algorithm runs in O(n log(n)) on average and sorts in place. This
     * implementation does not choose the pivot value in an "intelligent" way and is not well
     * behaved on already sorted data.
     *
     * \param first RandomAccess iterator pointing at the start of the sequence.
     * \param last  RandomAccess iterator pointing just past the end of the sequence.
     * \param comp  Comparison object that is a model of a strict weak ordering.
     * \todo Review for exception and thread safety.
     */
    template< typename RandomAccess, typename StrictWeakOrdering >
    void quick_sort( RandomAccess first, RandomAccess last, StrictWeakOrdering comp )
    {
        if( first == last ) return;
        --last;
        quick_sort_guts( first, last, comp );
    }
    
    // Radix Sort helper function templates.
    
    template< typename Bidirectional >
    void counting_sort( Bidirectional first, Bidirectional last, int shift_count )
    {
        Bidirectional current = first;
        int sequence_size = 0;
        int counters[256];
        
        if( first == last ) return;
        
        // Initialize counters.
        for( int i = 0; i < 256; ++i ) counters[i] = 0;
        
        // Count the number of occurances of each element value.
        while( current != last ) {
            ++counters[( *current >> shift_count ) & 0x000000FF];
            ++sequence_size;
            ++current;
        }
        
        // Compute accumulations.
        for( int i = 0; i < 255; ++i ) counters[i+1] = counters[i] + counters[i+1];
        
        // Sort the sequence into a temporary workspace.
        typedef typename std::iterator_traits< Bidirectional >::value_type value_type;
        value_type *workspace = new value_type[sequence_size];
        do {
            --last;
            workspace[--counters[( *last >> shift_count ) & 0x000000FF]] = *last;
        } while( first != last );
        std::copy( workspace, workspace + sequence_size, first );
        delete [] workspace;
    }

    
    //! Sorts a sequence of unsigned 32 bit integers using Radix Sort.
    /*!
     * Although this algorithm is parameterized on iterator type, it assumes the value type of
     * the given iterator is a 32 bit unsigned integer. This restriction is related to the Radix
     * Sort algorithm. It would be possible to generalize the implementation to allow for other
     * value types, however.
     *
     * \param first Bidirectional iterator pointing at the start of the sequence.
     * \param last  Bidirectional iterator pointing just past the end of the sequence.
     */
     
    template< typename Bidirectional >
    void radix_sort( Bidirectional first, Bidirectional last )
    {
        counting_sort( first, last,  0 );
        counting_sort( first, last,  8 );
        counting_sort( first, last, 16 );
        counting_sort( first, last, 24 );
    }
    
} // End of namespace spica.

#endif
