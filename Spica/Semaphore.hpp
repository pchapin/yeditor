/*! \file    Semaphore.hpp
 *  \brief   Interface to a semaphore class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * The semaphores defined here are implemented in terms of boost::thread facilities.
 */

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <boost/thread.hpp>

namespace spica {

    class Semaphore {
        Semaphore( const Semaphore & );
        Semaphore &operator=( const Semaphore & );

    public:
        //! Initializes a semaphore.
        /*!
         * \param initial_value The value used to initialize the semaphore. Any value less than
         * zero causes the semaphore to be initialized to zero.
         */
        Semaphore( int initial_count );

        //! Increments the semaphore.
        /*!
         * This method advances the semaphore by one. If it was zero before this operation
         * starts, then a single thread (if any) waiting on the semaphore is released.
         */
        void up( );

        //! Decrements the semaphore.
        /*!
         * If the semaphore is not zero this method reduces the semaphore value by one. If the
         * semaphore is zero this method blocks the caller. Once the caller is released due to a
         * signal made by another thread, wait will return with the semaphore still zero in that
         * case.
         */
        void down( );

    private:
        boost::mutex lock;
        boost::condition_variable non_zero;
        int raw_count;
    };

}
#endif
