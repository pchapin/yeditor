/*! \file    date.cpp
 *  \brief   Implementation of calendar dates.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * The implementation is based on the "obvious" representation. This representation makes it
 * easy to do I/O on dates (the construction and access functions are trivial), but difficult to
 * do calculations on dates (the math functions are complicated and slow).
 *
 * This class artificially limits the date to the range Jan 1, 1800 through Dec 31, 2099. There
 * is no technical reason for this range. It is aribitrary. The assumption is that dates outside
 * of this range are probably errors and should be handled as such. Also this range avoids the
 * whole calendar reform issue.
 *
 * The constructor functions insure that the date object is initialized to be inside the
 * required range. However, the advance() function does not deal with out of range errors. This
 * is a bug and should be fixed.
 *
 * This implementation is rather crude. The math functions do not have to be as slow as they
 * are, but to do better would be too much like work. On the other hand, this class is very easy
 * to use because it is so simple (some might say stupid).
 */

#include <ctime>
#include <iostream>
#include <iomanip>

#include "Date.hpp"

namespace spica {

    //---------------------------------------------
    //           Private Member Functions
    //---------------------------------------------

    bool Date::is_leap( ) const
    {
        bool result = false;

        if(           ( Y %   4 == 0 ) ) result = true;
        if( result && ( Y % 100 == 0 ) ) result = false;
        if(!result && ( Y % 400 == 0 ) ) result = true;

        return result;
    }


    int Date::month_length( ) const
    {
        // Lookup table gives number of days in each month.
        static const int month_lengths[] = {
            31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        };

        int length = month_lengths[M - 1];

        // Now perhaps make a correction if the month is Feb.
        if( M == 2 && is_leap( ) ) length++;

        return length;
    }


    // This method has to deal with the fact that the number of days in each month is different.
    // Furthermore, the number of days in Feb is a function of the year.
    //
    void Date::next( )
    {
        // Try incrememting the day. If it overflows the month, make a correction.
        if( ++D > month_length( ) ) {
            D = 1;
            if( ++M > 12 ) {
                M = 1;
                ++Y;
            }
        }
    }


    // This method works similarly to Date::next().
    //
    void Date::previous( )
    {
        bool fix_day = false;

        // Try decrementing the day.
        if( --D < 1 ) {
            fix_day = true;
            if( --M < 1 ) {
                M = 12;
                --Y;
            }
        }

        // If we wrapped over a month boundary, set the day to the last day of the new (and now
        // current) month.
        //
        if( fix_day ) D = month_length( );
    }


    //--------------------------------------------
    //           Public Member Functions
    //--------------------------------------------

    // This function initializes the date to values given to us by the client. We don't trust
    // the client to give us sensible values.
    //
    void Date::set( int year, int month, int day )
    {
        Y = year; M = month; D = day;

        // If we get a year < 100 it probably means the client is giving us just a two digit
        // year. We will take any two digit year below 50 to mean 2000 to 2050. This hack will
        // help make this date class well behaved even when two digit years are used (by silly
        // people). This behavior is intentionally not documented.
        //
        if( Y < 100 ) {
            if( Y < 50 ) Y += 2000;
            else Y += 1900;
        }

        if( Y < 1800 ) Y = 1800;
        if( Y > 2099 ) Y = 2099;
        if( M < 1    ) M = 1;  //! \todo Deal with out of bounds values by adjusting entire date.
        if( M > 12   ) M = 12;
        if( D < 1    ) D = 1;
        if( D > month_length( ) ) D = month_length( );
    }


    Date::weekday_t Date::day_of_week( ) const
    {
        struct tm now;

        // This follows the example in the C99 standard. See section 7.23.2.3
        now.tm_year  = Y - 1900;
        now.tm_mon   = M - 1;
        now.tm_mday  = D;
        now.tm_hour  = 0;
        now.tm_min   = 0;
        now.tm_sec   = 1;
        now.tm_isdst = -1;

        // TODO: Check for errors.
        mktime( &now );
        return static_cast<weekday_t>( now.tm_wday );
    }


    void Date::advance( long delta )
    {
        long count;

        // If delta == 0 we're done already.
        if( delta == 0 ) return;

        // If we're trying to step forward...
        if( delta >  0 ) {
            for( count = delta; count > 0; count-- ) next( );
        }

        // We're trying to step backward...
        else {
            for( count = -delta; count > 0; count-- ) previous( );
        }
    }


    //-----------------------------------------
    //           Non-Member Functions
    //-----------------------------------------

    bool operator==( const Date &left, const Date &right )
    {
        if( left.year( )  == right.year( )  &&
            left.month( ) == right.month( ) &&
            left.day( )   == right.day( )     ) return true;

        return false;
    }


    bool operator<( const Date &left, const Date &right )
    {
        if( left.year( ) <  right.year( ) ) return true;
        if( left.year( ) == right.year( ) ) {
            if( left.month( ) <  right.month( ) ) return true;
            if( left.month( ) == right.month( ) ) {
                if( left.day( ) <  right.day( ) ) return true;
            }
        }
        return false;
    }


    long operator-( const Date &left, const Date &right )
    {
        long result = 0;

        // Handle the simple case first.
        if( left == right ) return 0;

        // Make copies of the parameters that I can change.
        Date my_left  = left;
        Date my_right = right;

        // If my_right is small, advance it until it matches my_left.
        while( my_left > my_right ) {
            my_right.advance( 1 );
            result++;
        }

        // If my_left is small, advance it until it matches my_right.
        while( my_right > my_left ) {
            my_left.advance( 1 );
            result--;
        }

        return result;
    }


    long workday_difference( const Date &left, const Date &right )
    {
        long result = 0;

        // Handle the simple case first.
        if( left == right ) return 0;

        // Make copies of the parameters that I can change.
        Date my_left  = left;
        Date my_right = right;

        // If my_right is small, advance it until it matches my_left.
        // Count the number of work days.
        while( my_left > my_right ) {
            my_right.advance( 1 );

            Date::weekday_t current_day = my_right.day_of_week( );
            if( current_day != Date::SAT && current_day != Date::SUN )
                result++;
        }

        // If my_left is small, advance it until it matches my_right.
        while( my_right > my_left ) {
            my_left.advance( 1 );

            Date::weekday_t current_day = my_left.day_of_week( );
            if( current_day != Date::SAT && current_day != Date::SUN )
                result--;
        }

        return result;
    }


    std::ostream &operator<<( std::ostream &output, const Date &right )
    {
        char original_fill = output.fill( );
        int  overall_width = output.width( );

        output << std::setfill( '0' );
        output << std::setw( 4 ) << right.year( )  << '-';
        output << std::setw( 2 ) << right.month( ) << '-';
        output << std::setw( 2 ) << right.day( );
        output << std::setfill( original_fill );

        for( int i = 10; i < overall_width; ++i ) {
            output << original_fill;
        }

        return output;
    }


    std::istream &operator>>( std::istream &input, Date &right )
    {
        int  day, month, year;
        char dummy;

        input >> year >> dummy >> month >> dummy >> day;
        right.set( year, month, day );

        return input;
    }

}
