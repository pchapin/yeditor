/*! \file    WorkQueue.hpp
    \brief   Interface to a queue class for use in multithreaded programs.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This class facilitates thread communication. One thread can add entities to the queue while
another takes entities off the queue. This is the classic producer/consumer problem. This
solution uses the library queue template to provide a large work area. The constructor function
sets a maximum size to prevent a very fast producer thread from overwhelming a slow consumer
thread and filling memory with queued work. This code works correctly when there are multiple
producers or multiple consumers.

This code is exception safe in the sense that if an exception is thrown during either the push
or pop operations the threads will release any locks that they own and leave the queue in a
consistent state. This code currently does very little error checking on the various functions
that it uses. This is an area where it could be improved.
*/

#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include "environ.hpp"
#include <list>
#include <queue>
#include "synchronize.hpp"

namespace spica {

    template< typename T > class WorkQueue {
    public:
        WorkQueue( int max_size );
        // Although the queue grows and shrinks dynamically, max_size sets an upper bound on the
        // size of the queue. This is to prevent a fast producer thread from filling all of
        // memory with unprocessed queue elements.

        void push( const T & );
        void pop (       T & );
        // The obvious operations. The push() function blocks if the queue is full and waits for
        // a free slot to become available. The pop() function blocks if the queue is empty and
        // waits for something to arrive.
        //
        // Unlike the way the standard queue works, this pop() gives both access to the next
        // element and removes it from the queue. This was done to minimize locking overhead. It
        // does mean, however, that if an error occurs during the copy of the object, the object
        // might be lost. (That won't happen if the copy operation throws an exception on error.
        // In that case the original object is left on the queue).

        int size( );
        // Returns the number of items waiting in the queue.

        bool empty( );
        // Returns true if there is nothing in the queue.

    private:
        typedef std::list< T > supporting_container;

        std::queue< T, supporting_container > the_queue;
        mutex_sem     mutex;
        counting_sem  free_slots;
        counting_sem  used_slots;

        // Make copying WorkQueues illegal.
        WorkQueue( const WorkQueue< T > & );
        WorkQueue< T > &operator=( const WorkQueue< T > & );
    };


    //
    // WorkQueue<T>::WorkQueue()
    //
    // This function initializes the syncronization primitives needed to make this work. Do not
    // attempt to use a WorkQueue object until the thread calling its constructor has returned.
    //
    template< typename T > inline WorkQueue< T >::WorkQueue( int max_size ) :
        free_slots( max_size ), used_slots( 0 )
    { }


    //
    // void WorkQueue<T>::push(const T &incoming)
    //
    // This function pushes an object of type T on the queue. It does it in such a way that
    // there will be no races inside the queue. If a thread is waiting in pop(), then it will be
    // released. If an exception occurs during the push operation, this function will make sure
    // that the queue is properly unlocked and that the counting semaphores are in an
    // appropriate state.
    //
    // Here I assume that if push throws an exception, the incoming object is not left on the
    // queue in some sort of partially copied state. How true this is will depend on the
    // exception safety of the library queue class.
    //
    template< typename T > void WorkQueue< T >::push( const T &incoming )
    {
        free_slots.down( );
        {
            mutex_sem::grabber critical( mutex );
            try { the_queue.push( incoming ); }
            catch( ... ) {
                free_slots.up( );
                throw;
            }
        }
        used_slots.up( );
    }

    
    //
    // void WorkQueue<T>::pop(T &outgoing)
    //
    // This function pops an object of type T. It does it in such a way that there will be no
    // races inside the queue. If there is nothing in the queue, the thread will be blocked
    // until something appears. If an exception is thrown when the object is copied out of the
    // queue this function leaves the object on the queue (so that a future pop can try again).
    // In that case, the queue is properly unlocked and the counting semaphores are set to
    // appropriate states.
    //
    template< typename T > void WorkQueue< T >::pop( T &outgoing )
    {
        used_slots.down( );
        {
            mutex_sem::grabber critical( mutex );
            try {
                outgoing = the_queue.front( );
                the_queue.pop( );
            }
            catch( ... ) {
                used_slots.up( );
                throw;
            }
        }
        free_slots.up( );
    }



    //
    // int WorkQueue<T>::size()
    //
    // Returns the number of items waiting in the queue.
    //
    template< typename T > inline int WorkQueue< T >::size( )
    {
        mutex_sem::grabber critical( mutex );
        return the_queue.size( );
    }


    //
    // bool WorkQueue<T>::empty()
    //
    // Returns true if the queue is empty; false otherwise.
    //
    template< typename T > inline bool WorkQueue< T >::empty( )
    {
        mutex_sem::grabber critical( mutex );
        return the_queue.empty( );
    }


} // End of namespace scope.

#endif
