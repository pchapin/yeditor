/*! \file    ThreadPool.hpp
 *  \brief   Interface to a thread pool class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * The thread pool defined here uses boost::threads as the underlying thread API.
 */

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <boost/thread.hpp>
#include "Semaphore.hpp"

namespace spica {

    // Templates to hold thread parameters
    // ===================================

    template< class Callable >
    struct Parameters0 {
        const Callable f;

        Parameters0( Callable initial_f )
            : f( initial_f )
        { }
    };

    template< class Callable, class Parameter1Type >
    struct Parameters1 {
        const Callable f;
        Parameter1Type param1;

        Parameters1( Callable initial_f, Parameter1Type initial_param1 )
            : f( initial_f ), param1( initial_param1 )
        { }
    };

    template< class Callable, class Parameter1Type, class Parameter2Type >
    struct Parameters2 {
        const Callable f;
        Parameter1Type param1;
        Parameter2Type param2;

        Parameters2( Callable initial_f, Parameter1Type initial_param1, Parameter2Type initial_param2 )
            : f( initial_f ), param1( initial_param1 ), param2( initial_param2 )
        { }
    };


    // Thread launching functions. All have type void(void *)
    // ======================================================
    
    template< class Callable >
    void thread_launcher0( void *raw_parameters )
    {
        typedef Parameters0<Callable> parameters_t;
        parameters_t *p = static_cast< parameters_t * >( raw_parameters );
        p->f( );
    }

    template< class Callable, class Parameter1Type >
    void thread_launcher1( void *raw_parameters )
    {
        typedef Parameters1<Callable, Parameter1Type> parameters_t;
        parameters_t *p = static_cast< parameters_t * >( raw_parameters );
        p->f( p->param1 );
    }

    template< class Callable, class Parameter1Type, class Parameter2Type >
    void thread_launcher2( void *raw_parameters )
    {
        typedef Parameters2<Callable, Parameter1Type, Parameter2Type > parameters_t;
        parameters_t *p = static_cast< parameters_t * >( raw_parameters );
        p->f( p->param1, p->param2 );
    }


    // ThreadPool class
    // ================

    class ThreadPool {
    private:

        // Describes the state of one of the worker threads.
        struct ThreadInformation {
            boost::mutex   lock;             // Used to enforce mutual exclusion to this ThreadInformation.
            boost::thread *worker;           // Pointer to actual thread object for this slot.
            void   ( *launcher )( void * );  // Pointer to helper function that launches thread into its callable.
            void     *raw_parameters;        // Points at an object containing the callable and its params.
            bool      fresh_work;            // true when this worker thread has work ready (or being processed).
            bool      fresh_result;          // true when there is a new result to pick up.
            boost::barrier *termination;     // Points at object used to synchronize thread termination.
            boost::condition_variable work_ready;
            boost::condition_variable result_ready;

            ThreadInformation( );

        private:
            // Disable copying.
            ThreadInformation( const ThreadInformation & );
            ThreadInformation &operator=( const ThreadInformation & );
        };

        int                pool_size;
        Semaphore          worker_count;
        ThreadInformation *thread_information;
        boost::barrier     synchronize_termination;

        // Disable copying.
        ThreadPool( const ThreadPool & );
        ThreadPool &operator=( const ThreadPool & );

    public:
        friend void dispatching_function( void * );
        typedef std::vector< ThreadInformation >::size_type threadid_t;

        ThreadPool( );
        ThreadPool( int thread_count );
        ~ThreadPool( );

        int count( ) { return pool_size; }

        template< typename Callable >
        threadid_t start_work(       Callable        work_function );

        template< typename Callable, typename Parameter1Type >
        threadid_t start_work(       Callable        work_function,
                                     const Parameter1Type &parameter_1 );

        template< typename Callable, typename Parameter1Type, typename Parameter2Type >
        threadid_t start_work(       Callable        work_function,
                               const Parameter1Type &parameter_1,
                               const Parameter2Type &parameter_2 );

        void work_result( threadid_t ID );
    };


    // These functions are used to start a unit of work. It returns a "thread ID" that can be
    // used later to "join" with the thread running work_function. Note that the thread that
    // executes the work function does not terminate when it is finished. Instead that thread
    // returns to the pool to execute other work.
    //
    
    template< typename Callable >
    ThreadPool::threadid_t ThreadPool::start_work( Callable work_function )
    {
        // Be sure there is a free worker thread.
        worker_count.down( );

        // Loop over all threads in the pool looking for the first available one.
        auto info = thread_information;
        for( ; info != thread_information + pool_size; ++info ) {
            boost::lock_guard< boost::mutex > guard( info->lock );

            // Does the thread need work?
            if( info->fresh_work == false ) {

                // Set up the parameters structure.
                auto raw_parameters  = new Parameters0< Callable >( work_function );

                // Initialize the thread information structure.
                info->launcher       = thread_launcher0< Callable >;
                info->raw_parameters = raw_parameters;
                info->fresh_work     = true;
                info->fresh_result   = false;

                // Tell the thread that it is ready to go.
                info->work_ready.notify_one( );
                break;
            }
        }
        return info - thread_information;
    }


    template< typename Callable, typename Parameter1Type >
    ThreadPool::threadid_t
        ThreadPool::start_work( Callable work_function, const Parameter1Type &param_1 )
    {
        // Be sure there is a free worker thread.
        worker_count.down( );

        // Loop over all threads in the pool looking for the first available one.
        auto info = thread_information;
        for( ; info != thread_information + pool_size; ++info ) {
            boost::lock_guard< boost::mutex > guard( info->lock );

            // Does the thread need work?
            if( info->fresh_work == false ) {

                // Set up the parameters structure.
                auto raw_parameters  =
                    new Parameters1< Callable, Parameter1Type >( work_function, param_1 );

                // Initialize the thread information structure.
                info->launcher       = thread_launcher1< Callable, Parameter1Type >;
                info->raw_parameters = raw_parameters;
                info->fresh_work     = true;
                info->fresh_result   = false;

                // Tell the thread that it is ready to go.
                info->work_ready.notify_one( );
                break;
            }
        }
        return info - thread_information;
    }


    template< typename Callable, typename Parameter1Type, typename Parameter2Type >
    ThreadPool::threadid_t ThreadPool::start_work(       Callable        work_function,
                                                   const Parameter1Type &param_1,
                                                   const Parameter2Type &param_2 )
    {
        // Be sure there is a free worker thread.
        worker_count.down( );
        
        // Loop over all threads in the pool looking for the first available one.
        auto info = thread_information;
        for( ; info != thread_information + pool_size; ++info ) {
            boost::lock_guard< boost::mutex > guard( info->lock );

            // Does the thread need work?
            if( info->fresh_work == false ) {

                // Set up the parameters structure.
                auto raw_parameters =
                    new Parameters2< Callable, Parameter1Type, Parameter2Type >( work_function, param_1, param_2 );

                // Initialize the thread information structure.
                info->launcher =
                    thread_launcher2< Callable, Parameter1Type, Parameter2Type >;
                info->raw_parameters = raw_parameters;
                info->fresh_work     = true;
                info->fresh_result   = false;
                
                // Tell the thread that it is ready to go.
                info->work_ready.notify_one( );
                break;
            }
        }
        return info - thread_information;
    }

}

#endif
