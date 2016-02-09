/*! \file    synchronize.hpp
    \brief   Cross-platform semaphore classes.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This file defines the interface to a number of semphore classes. These classes have been
implemented with the same interface on several different platforms. Currently, this
implementation only supports anonymous semaphores that can be used inside a single process to
synchronize multiple threads in that process. Future versions of these classes may support named
semaphores that can be used to synchronize threads in different processes.

These classes currently have no error handling. They assume that all their primitive operations
work. This assumption should be removed eventually.
*/

#ifndef SYNCHRONIZE_HPP
#define SYNCHRONIZE_HPP

#include "environ.hpp"

#if eOPSYS == ePOSIX
#include <pthread.h>
#endif

#if eOPSYS == eOS2
#define INCL_DOSSEMAPHORES
#include <os2.h>
#endif

#if eOPSYS == eWIN32
#include <windows.h>
#endif

namespace spica {

    //! Mutex semaphores provide for exclusive access to a shared resource.
    //
    class mutex_sem {
    public:
        //! Constructor puts semaphore into an initially unowned state.
        mutex_sem( );

        //! Destructor releases semaphore if it is currently owned.
       ~mutex_sem( );

        //! Locks the semaphore (puts it into an owned state).
        void lock( );

        //! Releases ownwership of the semaphore.
        void unlock( );

       //! Provides for locking and unlocking using RAI idiom.
        class grabber {
        public:
            grabber( mutex_sem &sem, bool lock_now = true ) : the_sem( sem )
                { if( lock_now ) the_sem.lock( ); locked = lock_now; }

            void lock( )
                { if( !locked ) { the_sem.lock( ); locked = true; } }

            void unlock( )
                { if( locked ) { the_sem.unlock( ); locked = false; } }

           ~grabber( )
                { if ( locked ) the_sem.unlock( ); }

        private:
            mutex_sem &the_sem;
            bool       locked;

            // Inhibit copying.
            grabber( const grabber & );
            grabber &operator=( const grabber & );
        };

    private:
        #if eOPSYS == ePOSIX
        pthread_mutex_t  raw_object;
        #endif

        #if eOPSYS == eOS2
        HMTX raw_object;
        #endif

        #if eOPSYS == eWIN32
        CRITICAL_SECTION raw_object;
        #endif

        // Inhibit copying.
        mutex_sem( const mutex_sem & );
        mutex_sem &operator=( const mutex_sem & );
    };


    //! Counting semaphores.
    /*!
        Counting semaphores are simple counters. If a thread tries to
        decrement a zero, the thread is blocked until some other thread
        tries to increment the semaphore. At that time the blocked thread
        is allowed to complete its decrement operation. These semaphores
        are useful for keeping track of a limited resource that is being
        used by multiple threads. A thread should decrememt a counting
        semaphore before trying to use a unit of the resource to first
        "reserve" a unit for itself.
    */
    class counting_sem {
    public:
        counting_sem( int initial = 0 );
       ~counting_sem( );

        //! Increments the count.
        /*!
            This method never blocks (for long). It might unblock another
            thread.
         */
        void up( );

        //! Decrements the count.
        /*!
            Blocks the calling thread if the count is zero until some other
            thread does an up operation.
        */
        void down( );

    private:

        #if eOPSYS == ePOSIX
        pthread_mutex_t lock;
        pthread_cond_t  non_zero;
        int             raw_count;
        #endif

        #if eOPSYS == eOS2
        HMTX            lock;
        HEV             non_zero;
        int             raw_count;
        #endif

        #if eOPSYS == eWIN32
        HANDLE          the_sem;
        #endif

        // Inhibit copying.
        counting_sem( const counting_sem & );
        counting_sem &operator=( const counting_sem & );
    };


    //! Read/Write semaphore
    /*!
        Read/Write semaphores allow multiple readers to access a shared
        resource but give exclusive access to a single writer. This type of
        locking is appropriate when a shared resource can be read
        simultaneously safely and when most access is, in fact, read
        access. This implementation gives writers priority over readers.

        THIS CLASS IS CURRENTLY UNIMPLEMENTED!
    */
    class rw_sem {
    public:
        rw_sem( );
       ~rw_sem( );

        //! Read lock.
        /*!
            Asking for a read lock always returns at once unless a writer has
            locked.
        */
        void r_lock( );

        //! Release a read lock.
        void r_unlock( );

        //! Write lock.
        /*!
            Write locks are exclusive. When a thread has a write lock no other
            thread (either readers or writers) are granted access. When a
            write lock is requested, it will be granted as soon as possible.
        */
        void w_lock( );

        //! Release a write lock.
        void w_unlock( );

        //! Grabs a read lock using RAI idiom.
        class r_grabber {
        public:
            r_grabber( rw_sem &sem ) : the_sem( sem )
                { the_sem.r_lock( ); }

           ~r_grabber( )
                { the_sem.r_unlock( ); }

        private:
            rw_sem &the_sem;
        };

        //! Grabs a write lock using RAI idiom.
        class w_grabber {
        public:
            w_grabber( rw_sem &sem ) : the_sem( sem )
                { the_sem.w_lock( ); }

           ~w_grabber( )
                { the_sem.w_unlock( ); }

        private:
            rw_sem &the_sem;
        };

    private:

        #if eOPSYS == ePOSIX
        #endif

        #if eOPSYS == eOS2
        #endif

        #if eOPSYS == eWIN32
        #endif

        // Inhibit copying.
        rw_sem( const rw_sem & );
        rw_sem &operator=( const rw_sem & );
    };


   //==============================
    //           mutex_sem
    //==============================

    #if eOPSYS == ePOSIX

    inline mutex_sem::mutex_sem( )
        { pthread_mutex_init( &raw_object, 0 ); }

    inline mutex_sem::~mutex_sem( )
        { pthread_mutex_destroy( &raw_object ); }

    inline void mutex_sem::lock( )
        { pthread_mutex_lock( &raw_object ); }

    inline void mutex_sem::unlock( )
        { pthread_mutex_unlock( &raw_object ); }

    #endif


    #if eOPSYS == eOS2

    inline mutex_sem::mutex_sem( )
        { DosCreateMutexSem( 0, &raw_object, 0, FALSE ); }

    inline mutex_sem::~mutex_sem( )
        { DosCloseMutexSem( raw_object ); }

    inline void mutex_sem::lock( )
        { DosRequestMutexSem( raw_object, SEM_INDEFINITE_WAIT ); }

    inline void mutex_sem::unlock( )
        { DosReleaseMutexSem( raw_object ); }

    #endif


    #if eOPSYS == eWIN32

    inline mutex_sem::mutex_sem( )
        { InitializeCriticalSection( &raw_object ); }

    inline mutex_sem::~mutex_sem( )
        { DeleteCriticalSection( &raw_object ); }

    inline void mutex_sem::lock( )
        { EnterCriticalSection( &raw_object ); }

    inline void mutex_sem::unlock( )
        { LeaveCriticalSection( &raw_object ); }

    #endif


    //=================================
    //           counting_sem
    //=================================

    #if eOPSYS == eWIN32

    inline counting_sem::~counting_sem( )
        { CloseHandle( the_sem ); }

    inline void counting_sem::up( )
        { ReleaseSemaphore( the_sem, 1, 0 ); }

    inline void counting_sem::down( )
        { WaitForSingleObject( the_sem, INFINITE ); }

    #endif

}

#endif
