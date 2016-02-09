/*! \file    BinomialHeap.hpp
    \brief   Binomial heap container template
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

TODO:

 + Implement the DELETE and "DECREASE"-KEY operations.
*/

#ifndef BINOMIALHEAP_H
#define BINOMIALHEAP_H

#include <cstddef>
#include <functional>
#include <iterator>
#include <queue>

namespace spica {

    //--------------------------------------
    //           Class Definition
    //--------------------------------------

    //! Binomial heap container template.
    /*!
     * Binomial heaps allow for fast insertion, deletion, and extraction of minimum keys. Two
     * binomial heaps can also be combined quickly. These operations can all be done O(lg(n))
     * time where 'n' is the number of nodes in the heap(s). The ordering of the heap in this
     * implementation is controlled by the strict weak ordering comparison type used to
     * instantiate the template. By default this type is std::less< T > and thus this code
     * implements min-heaps by default.
     *
     *  For more information see Introduction to Algorithms, second edition by Cormen,
     *  Leiserson, Rivest, and Stein; published by MIT press, (C) 2001; Chapter 19. This
     *  reference has a very nice presentation on binomial heaps including some excellent
     *  diagrams showing how these functions work. In addition, the reference contains several
     *  suggestions for ways this simple minded implementation might be improved.
     */
    template< typename T, typename StrictWeakOrdering = std::less< T > >
    class BinomialHeap {
    public:

        typedef       T                   key_type;
        typedef       key_type            value_type;
        typedef       StrictWeakOrdering  key_compare;
        typedef       StrictWeakOrdering  value_compare;
        typedef       key_type           *pointer;
        typedef const key_type           *const_pointer;
        typedef       key_type           &reference;
        typedef const key_type           &const_reference;
        typedef       std::size_t         size_type;

    private:

        // A binomial heap is a list of binomial trees.
        struct BinomialTreeNode {
            BinomialTreeNode *parent;
            BinomialTreeNode *child;
            BinomialTreeNode *sibling;
            int               degree;
            key_type          data;

            // Initializes the members of a tree node.
            BinomialTreeNode( const key_type &new_item ) : data( new_item )
                { parent = child = sibling = 0; degree = 0; }
        };

        // The head of the root list.
        BinomialTreeNode *roots;

        // The number of data items in the heap.
        size_type count;

        // Comparison object used to decide heap ordering. The type StrictWeakOrdering must be a
        // model of Strict Weak Ordering.
        //
        StrictWeakOrdering comp;

        // This recursive function destroys the heap below and to the right of the given node.
        // It also destroys the given node.
        //
        static void destroy_heap( BinomialTreeNode * );

        // This function handles the linking of two binomial trees.
        static void binomial_link( BinomialTreeNode *left, BinomialTreeNode *right );

        // This function merges two root lists into a single root list. The root list in the
        // given heap is removed (the heap is made empty) and the root list in 'this' heap is
        // expanded. This function sorts the binomial trees in the root list in order of
        // ascending degree.
        //
        void binomial_merge( BinomialHeap &other );

        // Make copy operations illegal on binomial heaps (for now).
        BinomialHeap( const BinomialHeap & );
        BinomialHeap &operator=( const BinomialHeap & );

    public:

        //! Binomial heap iterator class.
        /*!
         * Binomial heap iterators are forward iterators. They provide O(1) increment and O(n)
         * sweeps through the entire heap. However, because of the structure of a binomial heap,
         * these iterators are relatively "heavyweight." As a consequence they might throw an
         * exception (std::bad_alloc) when they are copied or when they are incremented. In
         * practice I do not expect that this would be a major problem. They might also consume
         * a considerable amount of memory if they are iterating over a large heap.
         *
         * Specifically a binomial heap iterator maintains a queue of node pointes that it uses
         * to keep track of portions of the heap it needs to "go back" and explore further. For
         * a large heap this queue can contain a sizable number of pointers [TODO: Figure out
         * some specifics about how large the queue might get].
         */
        class iterator :
            public std::iterator< std::forward_iterator_tag, const T > {

            friend class BinomialHeap;

        private:
            const BinomialTreeNode *current;
            std::queue< const BinomialTreeNode * > *pQ;

            iterator( const BinomialTreeNode *c, std::queue< const BinomialTreeNode * > *q )
                : current( c ), pQ( q ) { }

        public:
            //! Default constructor.
            iterator( ) : current( 0 ), pQ( 0 ) { }

            //! Copy constructor.
            iterator( const iterator &other )
            {
                current = other.current;
                pQ      = new std::queue< const BinomialTreeNode * >( *other.pQ );
            }

            //! Assignment operator.
            iterator &operator=( const iterator &other )
            {
                delete pQ;
                current = other.current;
                pQ      = new std::queue< const BinomialTreeNode * >( *other.pQ );
                return *this;
            }

            //! Destructor.
           ~iterator( ) { delete pQ; }

            //! Returns true if two iterators point at the same object or both end( ).
            bool operator==( const iterator &other )
                { return current == other.current; }

            //! Returns true if two iterators point at different objects.
            bool operator!=( const iterator &other )
                { return current != other.current; }

            //! Returns a reference to the current object.
            /*! Applying this operation to an end() iterator is undefined. */
            const T &operator*( )
                { return current->data; }

            //! Returns a pointer to the current object.
            /*! Applying this operation to an end( ) iterator is undefined. */
            const T *operator->( )
                { return &current->data; }

            //! Prefix increment.
            /*!
             * Postfix is inefficient because copying an iterator is non-trivial. Therefor
             * postfix increment has not been implemented.
             */
            iterator &operator++( );

        };  // End of BinomialHeap::iterator.

        //! Create an empty heap.
        BinomialHeap( const StrictWeakOrdering &C = StrictWeakOrdering( ) )
            : roots( 0 ), count( 0 ), comp( C )
            { }

        //! Create a heap from the given sequence.
        template< typename InputIterator >
        BinomialHeap(
            InputIterator first,
            InputIterator last,
            const StrictWeakOrdering &C = StrictWeakOrdering( )
        );

        //! Destory the heap and all contained nodes.
       ~BinomialHeap( );

        //! Return the number of data items in the heap. O(1)
        size_type size( ) const
            { return count; }

        //! Returns true if the heap contains no items. O(1)
        bool empty( ) const
            { return count == 0; }

        // These members are provided by other, similar container types.
        key_compare key_comp( ) const
            { return comp; }

        value_compare value_comp( ) const
            { return comp; }

        //! Return an iterator to the first item in the heap.
        iterator begin( ) const;

        //! Return an iterator just past the last item in the heap.
        iterator end( ) const { return iterator( ); }

        //! Insert a copy of the given object into 'this' heap.
        /*!
         * This method returns a pointer to the new copy of the object for future reference.
         * This pointer will be valid until the heap holding the object is destroyed or until
         * the data object is popped from the heap. If 'this' heap is merged into another heap,
         * this pointer will continue to correctly point at the appropriate data object.
         *
         * Note that this method returns a pointer rather than the more usual iterator because
         * binomial heap iterators contain quite a bit of state information about the iteration
         * in progress. Building this state information for a newly inserted item would be
         * expensive.
         */
        const T *insert( const T &new_item );

        //! Wrapper around insert for consistency with pop.
        void push( const T &new_item )
            { insert( new_item ); }

        //! Inserts each item in the range [first, last) into the heap.
        template< typename InputIterator >
        void insert( InputIterator first, InputIterator last );

        //! Return a reference to the object at the front of the heap.
        /*!
         * This is the object with a key that comes before all other keys in the heap (according
         * to the comparison object given to the heap's constructor). If the heap is empty when
         * this method is called the effect is undefined.
         */
        const T &front( ) const;

        //! Extract the object at the front of the heap and throw away its value.
        /*!
         * If the heap is empty when this method is called the effect is undefined.
         */
        void pop( );

        //! Merge the other binomial heap into 'this' heap.
        /*!
         * The other heap is emptied by this operation but not destroyed; it remains in a usable
         * state.
         */
        BinomialHeap &merge( BinomialHeap &other );
    };


    // --------------
    // Static Methods
    // --------------

    //
    // binomial_link
    //
    template< typename T, typename StrictWeakOrdering >
    void BinomialHeap< T, StrictWeakOrdering >::
        binomial_link( BinomialTreeNode *left, BinomialTreeNode *right )
    {
        left->parent  = right;
        left->sibling = right->child;
        right->child  = left;
        right->degree++;
    }


    //
    // destroy_heap
    //
    template< typename T, typename StrictWeakOrdering >
    void BinomialHeap< T, StrictWeakOrdering >::destroy_heap( BinomialTreeNode *p )
    {
        if( p == 0 ) return;
        destroy_heap( p->child );

        // This should be an interation to avoid creating a huge stack when removing large
        // heaps. (How true is this?)
        //
        destroy_heap( p->sibling );

        delete p;
    }


    // ---------------
    // Private Methods
    // ---------------

    //
    // binomial_merge
    //
    template< typename T, typename StrictWeakOrdering >
    void BinomialHeap< T, StrictWeakOrdering >::binomial_merge( BinomialHeap &other )
    {
        // Update the item count information.
        count += other.count;
        other.count = 0;

        // If the other heap is empty, we are done already.
        if( other.roots == 0 ) return;

        // If this heap is empty, it is easy.
        if( roots == 0 ) {
            roots            = other.roots;
            other.roots      = 0;
            return;
        }

        // Now we have to think.
        BinomialTreeNode *this_walker  = roots;
        BinomialTreeNode *other_walker = other.roots;

        // We no longer need other.roots. Let's set it to 0. At this point the other heap is
        // emptied and ready to be reused.
        //
        other.roots = 0;

        // First, let's set an initial value to roots.
        if( this_walker->degree < other_walker->degree ) {
            roots       = this_walker;
            this_walker = this_walker->sibling;
        }
        else {
            roots        = other_walker;
            other_walker = other_walker->sibling;
        }

        BinomialTreeNode *new_walker = roots;

        // Now process the two existing root lists by walking down them as necessary.
        while( true ) {

            // If we have come to the end of one (we won't have come to the end of both at the
            // same time)...
            //
            if( this_walker == 0 ) {
                new_walker->sibling = other_walker;
                return;
            }
            if( other_walker == 0 ) {
                new_walker->sibling = this_walker;
                return;
            }

            // Otherwise set the correct sibling on the end of the new root list based on the
            // degrees at the front of the old root lists. The lines marked with '**' are
            // corrections added by Staffan Schroder and Mikael Bjork to fix a bug in my
            // original implementation.
            //
            if( this_walker->degree < other_walker->degree ) {
                new_walker->sibling = this_walker;
                new_walker          = this_walker;            // **
                this_walker         = this_walker->sibling;
            }
            else {
                new_walker->sibling = other_walker;
                new_walker          = other_walker;           // **
                other_walker        = other_walker->sibling;
            }
        }
    }

    // ----------------
    // Iterator Methods
    // ----------------

    //
    // operator++
    //
    template< typename T, typename StrictWeakOrdering >
    typename BinomialHeap< T, StrictWeakOrdering >::iterator &
        BinomialHeap< T, StrictWeakOrdering >::iterator::operator++( )
    {
        if( current->sibling != 0 ) {
            current = current->sibling;
            if( current->child != 0 ) pQ->push( current->child );
        }
        else if( !pQ->empty( ) ) {
            current = pQ->front( );
            pQ->pop( );
            if( current->child != 0 ) pQ->push( current->child );
        }
        else { // We are done. Make *this into a valid end iterator.
            current = 0;
            delete pQ;
            pQ = 0;
        }
        return *this;
    }


    // --------------
    // Public Methods
    // --------------

    //
    // Template constructor
    //
    template< typename T, typename StrictWeakOrdering >
    template< typename InputIterator >
        BinomialHeap< T, StrictWeakOrdering >::BinomialHeap(
            InputIterator first, InputIterator last, const StrictWeakOrdering &C)
                : roots( 0 ), count( 0 ), comp( C )
    {
        insert( first, last );
    }


    //
    // Destructor
    //
    template< typename T, typename StrictWeakOrdering >
    BinomialHeap< T, StrictWeakOrdering >::~BinomialHeap( )
    {
        // Destroying a NULL pointer is safe.
        destroy_heap( roots );
    }


    //
    // begin
    //
    template< typename T, typename StrictWeakOrdering >
    typename BinomialHeap< T, StrictWeakOrdering >::iterator
        BinomialHeap< T, StrictWeakOrdering >::begin( ) const
    {
        if( roots == 0 ) return iterator( );
        std::queue< const BinomialTreeNode * > *q = new std::queue< const BinomialTreeNode * >;
        if( roots->child != 0 ) q->push( roots->child );
        return iterator( roots, q );
    }


    //
    // insert
    //
    template< typename T, typename StrictWeakOrdering >
    const T *BinomialHeap< T, StrictWeakOrdering >::insert( const T &new_item )
    {
        BinomialHeap< key_type, StrictWeakOrdering > temp_heap( comp );

        BinomialTreeNode *new_node = new BinomialTreeNode( new_item );

        temp_heap.roots = new_node;
        temp_heap.count = 1;
        merge( temp_heap );

        return &new_node->data;
    }


    //
    // Template insert
    //
    template< typename T, typename StrictWeakOrdering >
    template< typename InputIterator >
        void BinomialHeap< T, StrictWeakOrdering >::
            insert( InputIterator first, InputIterator last )
    {
        while( first != last ) {
            insert( *first );
            first++;
        }
    }


    //
    // front
    //
    template< typename T, typename StrictWeakOrdering >
    const T &BinomialHeap< T, StrictWeakOrdering >::front( ) const
    {
        // Add error handling?
        // if( roots == 0 ) ...

        // To start, assume the first root in the root list contains the min.
        key_type *min_object = &roots->data;

        // Walk down the rest of the root list...
        BinomialTreeNode *consider = roots->sibling;
        while( consider != 0 ) {

            // If the node we are considering is "less" than our current min, update our current
            // min.
            //
            if( comp( consider->data, *min_object ) ) {
                min_object = &consider->data;
            }

            // In any case, keep walking.
            consider = consider->sibling;
        }

        return *min_object;
    }


    //
    // pop
    //
    template< typename T, typename StrictWeakOrdering >
    void BinomialHeap< T, StrictWeakOrdering >::pop( )
    {
        // Add error handling?
        // if( roots == 0 ) ...

        // To start, I need to find the item with the minimum key and remove it from the root
        // list. Let's first assume the minimum object is in the first node on the list. We have
        // to remember a pointer to the node previous to the minimum as well in order to
        // properly remove the minimum node.
        //
        BinomialTreeNode *front_node   = roots;
        BinomialTreeNode *min_previous = 0;

        // Walk down the rest of the root list...
        BinomialTreeNode *consider = roots->sibling;
        BinomialTreeNode *previous = roots;
        while( consider != 0 ) {

            // If the node we are considering is "less" than our current min, update our current
            // min. Keep track of the previous node as well.
            //
            if( comp( consider->data, front_node->data ) ) {
                front_node = consider;
                min_previous = previous;
            }

            // In any case, keep walking.
            previous = consider;
            consider = consider->sibling;
        }

        // Now unlink the minimum root node from the root list.
        if( min_previous == 0 ) roots = front_node->sibling;
            else min_previous->sibling = front_node->sibling;
        front_node->sibling = 0;

        // Isolate the minimum node from the rest of the binomial tree for which it is (was) a
        // root.
        //
        BinomialTreeNode *leftovers = front_node->child;
        front_node->child = 0;

        // Create a temporary heap.
        BinomialHeap< key_type > temp_heap( comp );

        // Walk down the sibling list of the leftovers and prepend the nodes to the temp heap's
        // root list. Note that this works fine if the minimum node has no children.
        //
        while( leftovers != 0 ) {
            BinomialTreeNode *temp = leftovers->sibling;
            leftovers->sibling = temp_heap.roots;
            temp_heap.roots = leftovers;
            leftovers = temp;
        }

        // Combine the leftovers back into 'this' heap.
        merge( temp_heap );

        // Blow away the front node.
        delete front_node;

        // Update our records on how many things are in this heap.
        count--;
    }


    //
    // merge
    //
    template< typename T, typename StrictWeakOrdering >
    BinomialHeap< T, StrictWeakOrdering > &
        BinomialHeap< T, StrictWeakOrdering >::merge( BinomialHeap &other )
    {
        // Combine the root lists in sorted order and empty the 'other' heap. This operation
        // takes O(Lg(n)) time.
        //
        binomial_merge( other );

        // If there is nothing in the combined heaps we are done.
        if( roots == 0 ) return *this;

        // Set up three pointers into the merged root list.
        BinomialTreeNode *previous = 0;
        BinomialTreeNode *current  = roots;
        BinomialTreeNode *next     = current->sibling;

        // Keep processing as long as there is something else past current.
        while( next != 0 ) {

            // If there are not two in a row with the same degree or if there are three in a row
            // with the same degree, just skip over the current entry in the root list.
            //
            if( current->degree != next->degree ||
                ( next->sibling != 0 && next->sibling->degree == current->degree ) ) {
                previous = current;
                current  = next;
            }

            // Otherwise there are two in a row with the same degree that need to be combined.
            // If the current root is smaller than the next root, it becomes the new root and
            // the next root becomes it's left child. previous is unchanged.
            //
            else if( comp( current->data, next->data ) ) {
                current->sibling = next->sibling;
                binomial_link(next, current);
            }

            // Apparently the next root is the smaller. Make the current root the left child of
            // the next root. The next root becomes the new current root. We have to use
            // previous here to patch the sibling pointer in the previous root so that it
            // correctly skips over the old current root.
            //
            else {
                if( previous == 0 ) roots = next;
                    else previous->sibling = next;
                binomial_link( current, next );
                current = next;
            }

            // Figure out a new next.
            next = current->sibling;
        }
        return *this;
    }

}

#endif

