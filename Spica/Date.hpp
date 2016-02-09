/*! \file    date.hpp
 *  \brief   Interface to calendar dates.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef DATE_HPP
#define DATE_HPP

#include <iosfwd>

namespace spica {

    //! Class representing calendar dates.
    /*!
     * Date objects will handle dates between Jan 1, 1800 and Dec 31, 2099. Any attempt to set a
     * date object outside that range, either directly or by way of a computation, will have
     * undefined effects.
     *
     * The interface to date objects uses 1-based numbering: January is month 1, February is
     * month 2, and so on. Furthermore, the first day of the month is day 1. Years must be full,
     * four digit years.
     */
    class Date {
    public:
        // The implementation of day_of_week depends on this enumeration order.
        enum weekday_t { SUN, MON, TUE, WED, THU, FRI, SAT };

        //! Default constructor initializes the date to January 1, 1970.
        Date( )
            { Y = 1970; M = 1; D = 1; }
    
        //! Allows initializing dates to arbitrary values.
        /*!
         * The year should be a full four digits. This method also performs some sanity checks
         * to ensure that the components of the date are all in range.
         *
         * \param year The year in the range 1800 .. 2099
         * \param month The month in the range 1 .. 12
         * \param day The day in the range 1 .. 31
         */
        Date( int year, int month, int day )
            { set( year, month, day ); }

        //! Allows setting a date after it has been constructed.
        /*!
         * Semantics are just like those of the constructor. The parameters have the same
         * meaning.
         */
        void set( int year, int month, int day );

        //! Returns the day of this date (range 1 .. 31).
        int day( ) const
            { return D; }

        //! Returns the month of this date (range 1 .. 12).
        int month( ) const
            { return M; }

        //! Returns the year of this date (range 1800 .. 2099).
        int year( ) const
            { return Y; }

        //! Returns the day of the week of this date.
        weekday_t day_of_week( ) const;

        //! Advance the date by given number of days.
        /*!
         * If delta is negative this function will back the date up.
         *
         * \param delta The number of days to advance the date. Negative values are allowed.
         */
        void advance( long delta = 1 );

    private:

        // The "obvious" implementation.
        int D, M, Y;

        bool is_leap( ) const;       //!< Return true if the current year is a leap year.
        int  month_length( ) const;  //!< Return the number of days in the current month.
        void next( );                //!< Advances the date by one day.
        void previous( );            //!< Backs up the date by one day.
    };

    //
    // Non-member Functions
    //

    //! Returns true if two dates are the same.
    bool operator==( const Date &left, const Date &right );

    //! Returns true if left comes before right.
    bool operator< ( const Date &left, const Date &right );

    //! Returns true if two dates are different.
    inline bool operator!=( const Date &left, const Date &right )
        { return !( left == right ); }

    //! Returns true if left is the same as right or comes before right.
    inline bool operator<=( const Date &left, const Date &right )
        { return left < right || left == right; }

    //! Returns true if left comes after right.
    inline bool operator>( const Date &left, const Date &right )
        { return !( left <= right ); }

    //! Returns true if left is the same as right or comes after right.
    inline bool operator>=( const Date &left, const Date &right )
        { return !( left < right ); }


    //! Computes how many days difference there is between two dates.
    /*!
     * The return value is positive if left comes after right and negative if left comes before
     * right.
     */
    long operator-( const Date &left, const Date &right );


    //! Computes how many workdays difference there is between two dates.
    /*!
     * Saturdays, Sundays, and holidays are not considered in the count.
     * \todo Implement proper support for holidays.
     */
    long workday_difference( const Date &left, const Date &right );


    //! Writes a date to an output stream in the format yyyy-mm-dd.
    std::ostream &operator<<( std::ostream &output, const Date &right );

    //! Reads a date from an input stream in the format yyyy-mm-dd.
    /*!
     * Is is permitted to use any non-digit separator character between the date components.
     * Although the format yyyy-mm-dd is nominal, this function will also accept, for example,
     * yyyy/mm/dd or yyyy:mm:dd, etc.
     */
    std::istream &operator>>( std::istream &input, Date &right );
}

#endif
