/*! \file    test_semaphore.cpp
 *  \brief   Program for CIS-4230, Homework #2.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 *  Simple test program to exercise the Semaphore class.
 */

#include <cstdlib>
#include <iostream>
#include <boost/thread.hpp>
#include "Semaphore.hpp"

using namespace spica;

// Keep track of the number of available resource items.
Semaphore resource_counter;
const int COUNT = 10000;

// Used to synchronize access to the console (to prevent interleaved output lines).
boost::mutex console_lock;

void upper( )
{
    // Produce COUNT resources and signal the Semaphore for each one.
    for( int i = 0; i < COUNT; ++i ) {
        resource_counter.signal( );
        if( (i + 1) % 1000 == 0 ) {
            boost::lock_guard< boost::mutex > guard( console_lock );
            std::cout << "Produced " << i << " items\n";
        }
    }
}


void downer( )
{
    // Consume COUNT resources.
    for( int i = 0; i < COUNT; ++i ) {
        resource_counter.wait( );
        if( (i + 1) % 1000 == 0 ) {
            boost::lock_guard < boost::mutex > guard( console_lock );
            std::cout << "Consumed " << i << " items\n";
        }
    }

    // Try to consume one more resource (not available... I block here)
    resource_counter.wait( );
}


int main( )
{
    boost::thread thread1( upper );
    boost::thread thread2( downer );

    // Delay a bit so that the loops above end and downer is blocked waiting.
    boost::this_thread::sleep( boost::posix_time::milliseconds( 2000 ) );

    // Give downer what he seeks (so he'll end cleanly).
    resource_counter.signal( );

    // Be sure both threads have fully ended before terminating the program.
    thread1.join( );
    thread2.join( );
    return EXIT_SUCCESS;
}
