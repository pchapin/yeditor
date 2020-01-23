/*! \file    Timer.cpp
    \brief   Implementation of a simple timer class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstddef>
#include "Timer.hpp"

typedef spica::Timer::timer_t mytimer_t;

//
// The following function looks up the system time and returns it in the mytimer_t type. Note
// that mytimer_t is intended to be an integer but might be a structure type on some systems.
//
static void get_time_as_integer( mytimer_t &result )
{
    #if eOPSYS == eDOS
    _dos_gettime( &result );
    #endif

    #if eOPSYS == eWIN32
    SYSTEMTIME raw;
    FILETIME   cooked;

    GetSystemTime( &raw );
    SystemTimeToFileTime( &raw, &cooked );
    result.LowPart  = cooked.dwLowDateTime;
    result.HighPart = cooked.dwHighDateTime;
    #endif

    #if eOPSYS == eOS2
    result.ticks = std::clock( );
    #endif

    #if eOPSYS == ePOSIX
    struct timeval time_info;
    gettimeofday( &time_info, NULL );
    result.seconds      = time_info.tv_sec;
    result.milliseconds = time_info.tv_usec / 1000;
    #endif
}


//
// The following function takes the system dependent mytimer_t and returns the number of
// milliseconds it represents as a long integer.
//
static long get_adjusted_time( const mytimer_t &the_time )
{
    #if eOPSYS == eDOS
    long temp = the_time.hsecond * 10L;
    temp += the_time.second * 1000L;
    temp += the_time.minute * 60000L;
    temp += the_time.hour * ( 60 * 60000L );
    return temp;
    #endif

    #if eOPSYS == eWIN32
    long long temp = the_time.QuadPart;
    temp /= 10000;
    return static_cast< long >( temp );
    #endif

    #if eOPSYS == eOS2
    return 1000L * ( the_time.ticks / CLOCKS_PER_SEC );
    #endif

    #if eOPSYS == ePOSIX
    long temp = the_time.seconds;
    temp *= 1000;
    temp += the_time.milliseconds;
    return temp;
    #endif
}


//
// The following two operator functions implement the usual semantics for the mytimer_t type. On
// some systems these functions are trivial.
//
static mytimer_t operator+( const mytimer_t &left, const mytimer_t &right )
{
    mytimer_t result;

    #if eOPSYS == eDOS
    result.hour    = static_cast< char >( left.hour    + right.hour );
    result.minute  = static_cast< char >( left.minute  + right.minute );
    result.second  = static_cast< char >( left.second  + right.second );
    result.hsecond = static_cast< char >( left.hsecond + right.hsecond );
    if( result.hsecond > 100 ) {
        ++result.second;
        result.hsecond -= 100;
    }
    if( result.second > 60 ) {
        ++result.minute;
        result.second -= 60;
    }
    if( result.minute > 60 ) {
        ++result.hour;
        result.minute -= 60;
    }
    #endif

    #if eOPSYS == eWIN32
    result.QuadPart = left.QuadPart + right.QuadPart;
    #endif

    #if eOPSYS == eOS2
    result.ticks = left.ticks + right.ticks;
    #endif

    #if eOPSYS == ePOSIX
    result.seconds      = left.seconds + right.seconds;
    result.milliseconds = left.milliseconds + right.milliseconds;
    if( result.milliseconds > 1000 ) {
        ++result.seconds;
        result.milliseconds -= 1000;
    }
    #endif

    return result;
}


static mytimer_t operator-( const mytimer_t &left, const mytimer_t &right )
{
    mytimer_t result;

    #if eOPSYS == eDOS
    long difference = ( get_adjusted_time( left ) - get_adjusted_time( right ) ) / 10;
    result.hsecond = static_cast< unsigned char >( difference % 100 );
    difference /= 100;
    result.second = static_cast< unsigned char >( difference % 60 );
    difference /= 60;
    result.minute = static_cast< unsigned char >( difference % 60 );
    difference /= 60;
    result.hour = static_cast< unsigned char >( difference );
    #endif

    #if eOPSYS == eWIN32
    result.QuadPart = left.QuadPart - right.QuadPart;
    #endif

    #if eOPSYS == eOS2
    result.ticks = left.ticks - right.ticks;
    #endif

    #if eOPSYS == ePOSIX
    result.seconds      = left.seconds - right.seconds;
    result.milliseconds = left.milliseconds - right.milliseconds;
    if( result.seconds > 0 && result.milliseconds < 0 ) {
        --result.seconds;
        result.milliseconds += 1000;
    }
    if( result.seconds < 0 && result.milliseconds > 0 ) {
        ++result.seconds;
        result.milliseconds -= 1000;
    }
    #endif

    return result;
}

namespace spica {

    //
    // Timer::Timer( )
    //
    Timer::Timer( )
    {
        internal_state = RESET;

        #if eOPSYS == eDOS
        accumulated.hour    = 0;
        accumulated.minute  = 0;
        accumulated.second  = 0;
        accumulated.hsecond = 0;
        #endif

        #if eOPSYS == eWIN32
        accumulated.LowPart  = 0;
        accumulated.HighPart = 0;
        #endif

        #if eOPSYS == eOS2
        accumulated.ticks = 0;
        #endif

        #if eOPSYS == ePOSIX
        accumulated.seconds = 0;
        accumulated.milliseconds = 0;
        #endif
    }


    //
    // void Timer::reset( )
    //
    void Timer::reset( )
    {
        internal_state = RESET;

        #if eOPSYS == eDOS
        accumulated.hour    = 0;
        accumulated.minute  = 0;
        accumulated.second  = 0;
        accumulated.hsecond = 0;
        #endif

        #if eOPSYS == eWIN32
        accumulated.LowPart = 0;
        accumulated.HighPart = 0;
        #endif

        #if eOPSYS == eOS2
        accumulated.ticks = 0;
        #endif

        #if eOPSYS == ePOSIX
        accumulated.seconds = 0;
        accumulated.milliseconds = 0;
        #endif
    }


    //
    // void Timer::start( );
    //
    void Timer::start( )
    {
        internal_state = RUNNING;
        get_time_as_integer( start_time );
        return;
    }


    //
    // void Timer::stop( );
    //
    void Timer::stop( )
    {
        mytimer_t stop_time;

        get_time_as_integer( stop_time );
        internal_state = STOPPED;
        accumulated    = accumulated + ( stop_time - start_time );
        return;
    }


    //
    // long Timer::time( );
    //
    long Timer::time( )
    {
        mytimer_t total_time;
        mytimer_t current_time;

        if( internal_state != RUNNING ) {
            total_time = accumulated;
        }
        else {
            get_time_as_integer( current_time );
            total_time = accumulated + ( current_time - start_time );
        }
        return get_adjusted_time( total_time );
    }

}

