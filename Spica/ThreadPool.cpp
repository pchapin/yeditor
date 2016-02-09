/*! \file    ThreadPool.cpp
    \brief   Implementation of a thread pool class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "ThreadPool.hpp"

namespace spica {

    // This function is executed by each thread. It waits for work to do. When a ThreadPool is
    // not busy, all its worker threads are blocked waiting for work here.
    //
    // TODO: It seems like it should be possible to declare the parameter as type ThreadPool::
    // ThreadInformation. However, VC++10 complains that the name "ThreadInformation" is
    // inaccessible in the parameter list. Yet dispatching_function is a friend and, in fact,
    // "ThreadInformation" is accessible in the function's body. Is it really true that the
    // special access afforded by friendship does not extend to the parameter list or is this a
    // bug in VC++10?
    //
    void dispatching_function( void *raw_info )
    {
        ThreadPool::ThreadInformation *info =
            static_cast< ThreadPool::ThreadInformation * >( raw_info );
        try {
            while( 1 ) {
                {
                    boost::unique_lock< boost::mutex > guard( info->lock );
                    while( info->fresh_work == false || info->fresh_result == true )
                        info->work_ready.wait( guard );
                }

                info->launcher( info->raw_parameters );

                {
                    boost::lock_guard< boost::mutex > guard( info->lock );
                    info->fresh_result = true;
                    info->result_ready.notify_all( );
                }
            }
        }

        // When a worker is terminated at ThreadPool destruction time, gather here first. This
        // avoids a race condition. The main thread must wait until all workers are here before
        // deleting the ThreadInformation structures. See ThreadPool::~ThreadPool.

        catch( const boost::thread_interrupted & ) {
            info->termination->wait( );
        }
    }


    // Initialize a ThreadInformation's members.
    ThreadPool::ThreadInformation::ThreadInformation( )
        : launcher( 0 ), raw_parameters( 0 ), fresh_work( false ), fresh_result( false )
    { }


    // The default constructor fills the pool with a number of threads equal to the number or
    // processors.
    ThreadPool::ThreadPool( )
        : worker_count( boost::thread::hardware_concurrency( ) ),
          synchronize_termination( boost::thread::hardware_concurrency( ) + 1 )
    {
        pool_size = boost::thread::hardware_concurrency( );

        // Prepare the ThreadInformation structures.
        thread_information = new ThreadInformation[pool_size];
        for( int i = 0; i < pool_size; ++i ) {
            thread_information[i].termination = &synchronize_termination;
            thread_information[i].worker =
                new boost::thread( dispatching_function, &thread_information[i] );
        }
    }


    // Allows a pool to be created with any number of threads (more or less than processor count).
    ThreadPool::ThreadPool( int thread_count )
        : worker_count( thread_count ), synchronize_termination( thread_count + 1 )
    {
        pool_size = thread_count;
        thread_information = new ThreadInformation[pool_size];
    }


    // The destructor cleans up the pool. This code assumes all threads are quiet (waiting for
    // work).
    ThreadPool::~ThreadPool( )
    {
        for( auto info = thread_information; info != thread_information + pool_size; ++info ) {
            info->worker->interrupt( );
            delete info->worker;
        }

        // Wait for all threads to process the interruption before removing thread_information.
        synchronize_termination.wait( );
        delete [] thread_information;
    }


    // This function waits for the result of a previously started unit of work. All work units
    // must have their results picked up. A worker thread is not scheduled for more work until
    // the result of the previous work has been retrieved.
    //
    void ThreadPool::work_result( threadid_t ID )
    {
        boost::unique_lock< boost::mutex > guard( thread_information[ID].lock );

        // Wait until there is a fresh result.
        while( !thread_information[ID].fresh_result )
            thread_information[ID].result_ready.wait( guard );

        // Clear it out of the thread information structure.
        thread_information[ID].fresh_result = false;
        thread_information[ID].fresh_work   = false;

        // Increment the number of available workers.
        worker_count.up( );
    }

}
