/*! \file    Rational.hpp
    \brief   Rational number template.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

Rational is a template that uses any signed integral type to implement rational numbers
(fractions).
*/

#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <iostream>

 //! Rational numbers.
 /*!
  * This template allows you to create rational numbers for any suitable integer type. Here
  * "suitable" means that it supports all the normal integer operations. It is required that
  * certain integer literals (such as "0" and "1") be assignable to an object of type
  * integer_type.
  */
template<typename integer_type>
class Rational {

    //! Writes a Rational to an output stream.
    template<typename T>
    friend std::ostream &operator<<( std::ostream &output, const Rational<T> &number );

    //! Reads a Rational from an input stream.
    template<typename T>
    friend std::istream &operator>>( std::istream &input, Rational<T> &number );

public:
    //! Constructs a Rational from two integers.
    /*!
     * An attempt to construct a Rational with a denominator of zero is undefined.
     */
    Rational( const integer_type &num = 0, const integer_type &denom = 1 );

    //! Return the numerator of this Rational.
    const integer_type &get_numerator( ) const
        { return numerator; }

    //! Return the denominator of this Rational.
    const integer_type &get_denominator( ) const
        { return denominator; }

    //! Adds a Rational into 'this.'
    void operator+=( const Rational &right );

    //! Subtracts a Rational from 'this.'
    void operator-=( const Rational &right );

    //! Multiples a Rational by 'this.'
    void operator*=( const Rational &right );

    //! Divides a Rational into 'this.' Division by zero is undefined.
    void operator/=( const Rational &right );

private:
    integer_type numerator;
    integer_type denominator;

    void reduce( );
    // Reduce number as much as possible.
};


//--------------------------------------
//           Friend Functions
//--------------------------------------

template<typename integer_type>
std::ostream &operator<<( std::ostream &output, const Rational<integer_type> &number )
{
    if( number.denominator == 1 ) {
        output << number.numerator;
    }
    else {
        output << number.numerator << "/" << number.denominator;
    }
    return output;
}


template<typename integer_type>
std::istream &operator>>( std::istream &input, Rational<integer_type> &number )
{
    integer_type numerator;
    integer_type denominator = 1;

    input >> numerator;
    if( input.peek( ) == '/' ) {
        char dummy;
        input.get( dummy );  // Throw away the slash.
        input >> denominator;
    }
    number = Rational<integer_type>( numerator, denominator );
    return input;
}


//------------------------------------------------
//           Inline Non-Member Functions
//------------------------------------------------

//
// Various relational operations. Notice that the first two are not in-line functions (but that
// they are also not friends).
//

template<typename T>
bool operator==( const Rational<T> &left, const Rational<T> &right );

template<typename T>
bool operator< ( const Rational<T> &left, const Rational<T> &right );

template<typename T>
inline
bool operator!=( const Rational<T> &left, const Rational<T> &right )
    {  return !( left == right ); }

template<typename T>
inline
bool operator>=( const Rational<T> &left, const Rational<T> &right )
    {  return !( left < right ); }

template<typename T>
inline
bool operator<=( const Rational<T> &left, const Rational<T> &right )
    {  return ( right >= left ); }

template<typename T>
inline
bool operator>( const Rational<T> &left, const Rational<T> &right )
    {  return ( right < left ); }


//
// The following infix math operations can be easily defined in terms of the math operations
// defined in the class template.
//

template<typename T>
inline
Rational<T> operator+( const Rational<T> &left, const Rational<T> &right )
    { Rational<T> temp( left ); temp += right; return temp; }

template<typename T>
inline
Rational<T> operator-( const Rational<T> &left, const Rational<T> &right )
    { Rational<T> temp( left ); temp -= right; return temp; }

template<typename T>
inline
Rational<T> operator*( const Rational<T> &left, const Rational<T> &right )
    { Rational<T> temp( left ); temp *= right; return temp; }

template<typename T>
inline
Rational<T> operator/( const Rational<T> &left, const Rational<T> &right )
    { Rational<T> temp( left ); temp /= right; return temp; }

template<typename T>
inline
Rational<T> operator%( const Rational<T> &left, const Rational<T> &right )
    { Rational<T> temp( left ); temp %= right; return temp; }



//
// void Rational::reduce( );
//
// This function reduces a Rational number into lowest terms. For example, a number such as 4/2
// becomes 2/1, etc. This function is used after each operation on Rational numbers to insure
// that they stay in as "small" a form as possible.
//
template<typename integer_type>
void Rational<integer_type>::reduce( )
{
    // Deal with negative values. We can't allow a negative numerator to participate in Euclid's
    // algorithm below.
    //
    bool negative = false;
    if( numerator < 0 ) {
        negative  = true;
        numerator = -numerator;
    }

    integer_type u = numerator;
    integer_type v = denominator;
    integer_type temp;
    integer_type zero = 0;

    // Find the GCD of the numerator and the denominator (using Euclid's algorithm).
    while( v != zero ) {
        temp  = u;
        temp %= v;
        u     = v;
        v     = temp;
    }

    // Divide both numerator and denominator by the GCD.
    numerator   /= u;
    denominator /= u;

    // If the numerator was negative, put the sign back.
    if( negative ) numerator = -numerator;
}


// The constructor allows a Rational to be initialized with two integer_type objects. Notice
// that if the resulting Rational is negative the numerator holds the sign. Note that the
// compiler generated copy constructor, assignment operator, and destructor should all be
// satisfactory.
//
template<typename integer_type>
Rational<integer_type>::Rational( const integer_type &num, const integer_type &denom ) :
    numerator( num ), denominator( denom )
{
    if( denominator < 0 ) {
        numerator   = -numerator;
        denominator = -denominator;
    }
    reduce( );
}


//
// void Rational::operator+=( const Rational &right );
// void Rational::operator-=( const Rational &right );
// void Rational::operator*=( const Rational &right );
// void Rational::operator/=( const Rational &right );
//
// These functions perform the basic operations on Rational numbers.
//
template<typename integer_type>
void Rational<integer_type>::operator+=( const Rational<integer_type> &right )
{
    // Compute the new denominator.
    integer_type new_denominator = denominator;
    new_denominator *= right.denominator;

    // Compute the new numerator.
    integer_type new_numerator  = numerator;
    integer_type temp_numerator = right.numerator;
    new_numerator  *= right.denominator;
    temp_numerator *= denominator;
    new_numerator  += temp_numerator;

    // Make 'this' number the new number.
    numerator   = new_numerator;
    denominator = new_denominator;

    reduce( );
}

template<typename integer_type>
void Rational<integer_type>::operator-=( const Rational<integer_type> &right )
{
    // Compute the new denominator.
    integer_type new_denominator = denominator;
    new_denominator *= right.denominator;

    // Compute the new numerator.
    integer_type new_numerator  = numerator;
    integer_type temp_numerator = right.numerator;
    new_numerator  *= right.denominator;
    temp_numerator *= denominator;
    new_numerator  -= temp_numerator;

    // Make 'this' number the new number.
    numerator   = new_numerator;
    denominator = new_denominator;

    reduce( );
}

template<typename integer_type>
void Rational<integer_type>::operator*=( const Rational<integer_type> &right )
{
    numerator   *= right.numerator;
    denominator *= right.denominator;

    reduce( );
}

template<typename integer_type>
void Rational<integer_type>::operator/=( const Rational<integer_type> &right )
{
    if( right.numerator != 0 ) {
        numerator   *= right.denominator;
        denominator *= right.numerator;

        // If a negative shows up in the denominator, move it to the numerator.
        if( denominator < 0 ) {
            numerator   = -numerator;
            denominator = -denominator;
        }
        reduce( );
    }
}

template<typename integer_type>
bool operator<( const Rational<integer_type> &left, const Rational<integer_type> &right )
{
    integer_type top_left  = left.get_numerator( );
    integer_type top_right = right.get_numerator( );

    top_left  *= right.get_denominator( );
    top_right *= left.get_denominator( );

    return top_left < top_right;
}

template<typename integer_type>
bool operator==( const Rational<integer_type> &left, const Rational<integer_type> &right )
{
    integer_type top_left  = left.get_numerator( );
    integer_type top_right = right.get_numerator( );

    top_left  *= right.get_denominator( );
    top_right *= left.get_denominator( );

    return top_left == top_right;
}

#endif
