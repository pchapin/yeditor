/*! \file    Semaphore.cpp
 *  \brief   Implementation of a semaphore class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Semaphore.hpp"

namespace spica {

    Semaphore::Semaphore( int initial_count )
    {
        if( initial_count < 0 ) initial_count = 0;
        raw_count = initial_count;
    }


    void Semaphore::up( )
    {
        lock.lock( );
        raw_count++;
        lock.unlock( );
        non_zero.notify_one( );
    }


    void Semaphore::down( )
    {
        boost::unique_lock< boost::mutex > guard( lock );
        while( raw_count == 0 )
            non_zero.wait( guard );

        raw_count--;
    }

}
