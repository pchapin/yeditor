/*! \file    Timer.hpp
    \brief   Interface to a simple timer class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef TIMER_HPP
#define TIMER_HPP

#include "environ.hpp"

#if eOPSYS == eDOS
#include <dos.h>
#endif

#if eOPSYS == eWIN32
#include <windows.h>
#endif

#if eOPSYS == eOS2
#include <ctime>
#endif

#if eOPSYS == ePOSIX
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#include <sys/time.h>
#endif

namespace spica {

    //! Stopwatch-like timer objects.
    /*!
     * Objects from class Timer are useful for timing relatively long events in programs. They
     * use the system clock as a base for generating time delays and thus are not suitable (in
     * most cases) for short delays.
     *
     * Timers do not load the system in any way while they are timing. Only when they are
     * started and stopped do they check the system clock. They can thus be fooled if the system
     * clock is changed during the timing interval.
     *
     * Timers allow for multiple starts and stops. In addition, their internal state can be
     * obtained by client code.
     */
    class Timer {

    public:
        enum timer_state {
            RESET,      //!< No time accumulated. Timer not keeping time.
            RUNNING,    //!< Timer is active.
            STOPPED     //!< Timer is not active. Accumulated time remembered.
        };

        #if eOPSYS == eDOS
        typedef struct dostime_t timer_t;
        #endif

        #if eOPSYS == eWIN32
        typedef LARGE_INTEGER timer_t;
        #endif

        #if eOPSYS == eOS2
        struct timer_t {
            std::clock_t ticks;
        };
        #endif

        #if eOPSYS == ePOSIX
        struct timer_t {
            time_t seconds;
            int    milliseconds;
        };
        #endif

    private:
        timer_t      start_time;      //!< Time that the timer was last started.
        timer_t      accumulated;     //!< Total accumulated time.
        timer_state  internal_state;  //!< Current state of timer object.

    public:

        //! Constructs a Timer object.
        /*!
         * The constructor ensures that the timer has no accumulated time and is in the reset
         * state.
         */
        Timer( );

        //! Resets the timer.
        /*!
         * Resets the timer and erases the accumulated time. After a reset operation the timer
         * is in the same state as it is after construction.
         */
        void reset( );

        //! Read the timer state.
        /*! Return the internal state of the timer. */
        timer_state state( )
            { return internal_state; }

        //! Start the timer.
        /*!
         * If the timer is already started, it is essentially "retriggered". That is, the old
         * start time is replaced by the new one. This causes the last, partially finished
         * timing interval to be lost. The accumulated time is, however, not changed.
         */
        void start( );

        //! Stop the timer.
        /*!
         * This method stops the timer and updates the value of accumulated time. The old value
         * of accumulated time is not lost; timer objects allow frequent starts and stops.
         */
        void stop( );

        //! Read the timer.
        /*!
         * The following function returns the total accumulated time in milliseconds. Note that
         * if the timer is running when this function is called, it correctly evaluates the
         * time. The state of the timer is unchanged.
         */
        long time( );
    };

}

#endif
