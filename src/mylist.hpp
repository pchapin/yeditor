/*! \file    mylist.hpp
 *  \brief   A general purpose list template.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef MYLIST_HPP
#define MYLIST_HPP

#include <algorithm>
#include <cstddef>
#include <cstdlib>

//! Doubly linked list template supporting a "current point."
/*!
 * This template provides a fairly normally doubly linked list. However, unlike std::list, it
 * maintains a "current point" and allows (linear time) random access. This code also predates
 * Standard C++. It was created for Y many years agoe and is retained because eliminating it
 * would be more work than it would be worth (probably).
 */
template< typename T >
class List {
private:

    //! Structure that holds the pointers that form the list.
    struct Link {
        Link *next;
        Link *previous;

        virtual ~Link( );
    };

    //! Derived structure to hold an object of the desired type.
    struct Node : public Link {
        T data;

        explicit Node( const T &existing ) : data( existing ) { }
    };

    Link *head;        //!< Points at head sentinel (Only a Link).
    Link *tail;        //!< Points at tail sentinel (Only a Link).
    Link *current;     //!< Points at current point in list (Normally a Node).
    long  item_count;  //!< Number of items on the list ( >= 0).
    long  index;       //!< Index of current point ( >= 0).

    void initialize( );

public:
    List( );
    List( const List & );
    List &operator=( const List & );
   ~List( );

    void jump_to( long new_index );
    T *next( );
    T *previous( );
    T *insert( const T &new_data );
    void erase( );
    void clear( );

    //! Returns a pointer to the object at the current point.
    T *get( );

    //! Returns the index of the current point.
    long  current_index( ) const
        { return( index ); }

    //! Returns the number of objects in the list.
    long  size( ) const
        { return( item_count ); }

    //! Special index used to represent the next new slot.
    static const long off_end = -1L;

    //! Used to place bookmarks in Lists.
    /*!
     * The constructor of a Mark remembers the given List's current point and the destructor
     * restores it. This allows the current point to be temporarily changed in an exception safe
     * manner.
     */
    class Mark {
    private  : long  old_index; //!< Original index of the list.
    protected: List &the_list;  //!< The list in which the bookmark is placed.
    public   :
        //! Constructor remembers the list and it's current point.
        explicit Mark( List &L ) : old_index( L.current_index( ) ), the_list( L ) { }

        //! Destructor restores the current point.
       virtual ~Mark( ) { the_list.jump_to( old_index ); }
    };

    //! Used to iterate over a list.
    /*!
     * The constructor starts the iteration. Use operator() repeatedly until NULL is returned.
     * Note that the original current point of the list is restored after the iteration has
     * finished.
     */
    class Iterator : private Mark {
    public:
        //! Constructor remembers the list's current point and sets CP to zero.
        explicit Iterator( List &L ) : Mark( L ) { Mark::the_list.jump_to( 0 ); }

        //! Returns pointer to next item on the list or NULL of none left.
        T *operator( )( ) { return Mark::the_list.next( ); }
    };
};

template< typename T >
inline T *List< T >::get( )
  { return( current == tail ? 0 : &( static_cast< Node * >( current )->data ) ); }


/*=====================================*/
/*           Private Members           */
/*=====================================*/

template< typename T > List< T >::Link::~Link( )
  { return; }


//! Prepares the list for use. Called by constructors.
/*!
 * \throws std::bad_alloc if insufficient memory.
 */
template< typename T >
void List< T >::initialize( )
{
    head = new Link;
    tail = new Link;

    // Initialize the members of the list object.
    current    = tail;
    item_count = 0L;
    index      = 0L;

    // Link the sentinels.
    head->previous = head;
    tail->next     = tail;
    head->next     = tail;
    tail->previous = head;
}

/*====================================*/
/*           Public Members           */
/*====================================*/

//! Creates an empty list.
/*!
 * \throws std::bad_alloc if insufficient memory.
 */
template< typename T >
List< T >::List( )
{
    initialize( );
}


//! Copy constructor.
/*!
 * \throws std::bad_alloc if insufficient memory.
 */
template< typename T >
List< T >::List( const List &existing )
{
    T *object_ptr;
  
    initialize( );
    Iterator stepper( const_cast< List & >( existing ) );
    while( ( object_ptr = stepper( ) ) != NULL ) {
        insert( *object_ptr );
    }
}


//! Assignment operator.
/*!
 * If an exception occurs during the copy, the target list is unchanged.
 * \throws std::bad_alloc if insufficient memory.
 */
template< typename T >
List< T > &List< T >::operator=( const List &existing )
{
    T *object_ptr;
    List  temp;

    Iterator stepper( const_cast< List & >( existing ) );
    while( ( object_ptr = stepper( ) ) != NULL ) {
        temp.insert( *object_ptr );
    }
    std::swap( head,       temp.head       );
    std::swap( tail,       temp.tail       );
    std::swap( current,    temp.current    );
    std::swap( item_count, temp.item_count );
    std::swap( index,      temp.index      );

    return *this;
}


//! Destructor
template< typename T >
List< T >::~List( )
{
    clear();
    delete head;
    delete tail;
}


//! Moves the current point to the specified index.
/*!
 * This function allows random indexing into a list. It is "smart" in that it will use next() or
 * previous() from the closest of three possible points: The head, the tail, or the current
 * point. For example, if the desired index is only 100 nodes from the current point and 10000
 * nodes from either end, this function will start at the current point. Yet, if the desired
 * index in only 100 nodes from the tail (and 10000 nodes from the current point), it will start
 * at the tail. If the given index is out of bounds, the current point is left just past the end
 * of the list.
 *
 * \param new_index The desired location of the current point (zero based).
 */
template< typename T >
void List< T >::jump_to( const long new_index )
{
    enum start_type { HEAD, CURRENT, TAIL };

    // Handle case of out of bounds index.
    if( new_index < 0L  ||  new_index >= item_count ) {
        current = tail;
        index = item_count;
        return;
    }

    // Compute distance between desired index and the three reference points.
    long distances[3];
    distances[HEAD]    = new_index;
    distances[CURRENT] = std::labs(new_index - index);
    distances[TAIL]    = item_count - new_index;

    // Find out which is minimum.
    start_type best_start = HEAD;
    long min = distances[HEAD];
    if( distances[CURRENT] < min ) {
        best_start = CURRENT;
        min        = distances[CURRENT];
    }
    if( distances[TAIL] < min ) {
        best_start = TAIL;
        min        = distances[TAIL];
    }

    // Do the actual work of moving the current point.
    switch( best_start ) {
    case HEAD:
        current = head->next;
        for ( ; min != 0L; min--) current = current->next;
        break;

    case CURRENT:
        if (new_index - index >= 0L) {
            for ( ; min != 0L; min--) current = current->next;
        }
        else {
            for ( ; min != 0L; min--) current = current->previous;
        }
        break;

    case TAIL:
        current = tail;
        for ( ; min != 0L; min--) current = current->previous;
        break;

    }

    // Set the current index.
    index = new_index;
}


//! Like *p++.
/*!
 * \return NULL if nothing left in the list. Current point not changed in that case.
 */
template< typename T >
T *List< T >::next( )
{
    if( index == item_count ) return NULL;

    T *const result = &( static_cast< Node * >( current )->data );
    current = current->next;
    index++;
    return( result );
}


//! Like *--p.
/*!
 * \return NULL if nothing left in the list. Current point not changed in that case.
 */
template< typename T >
T *List< T >::previous( )
{
    if ( index == 0 ) return NULL;
  
    current = current->previous;
    index--;
    return &( static_cast< Node * >( current )->data );
}


//! Inserts a new data item before the current point.
/*
 * \param new_data Data item to be inserted. This object is copied into the list.
 * \return A pointer to the new copy of the object.
 * \throws std::bad_alloc if there is insufficient memory.
 */
template< typename T >
T *List< T >::insert( const T &new_data )
{
    Node *const fresh = new Node( new_data );
    item_count++;
    fresh->next             = current;
    fresh->previous         = current->previous;
    current->previous->next = fresh;
    current->previous       = fresh;
    index++;
    return( &fresh->data );
}


//! Erases object at the current point and advances the current point.
/*!
 * The current point is advanced to the next item on the list.
 */
template< typename T >
void List< T >::erase( )
{
    if( current == tail ) return;
    Link *old = current;

    current = current->next;
    old->previous->next = current;
    current->previous = old->previous;
    item_count--;
    delete old;
}


//! Removes all elements in the list.
/*!
 * The list is usable after this method executes.
 */
template< typename T >
void List< T >::clear( )
{
    Link *temp;
    
    // Trash all the nodes in the list.
    current = head->next;
    while( current->next != current ) {
        temp = current;
        current = current->next;
        delete temp;
    }

    // Make sure these members are correct.
    item_count = 0L;
    index      = 0L;

    // Make sure the head and tail sentinels are linked together!
    head->next     = tail;
    tail->previous = head;
}

#endif
