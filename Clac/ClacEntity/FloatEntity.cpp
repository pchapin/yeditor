/*! \file    FloatEntity.cpp
 *  \brief   Implementation of the clac numeric type FloatEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * TODO:
 *
 * + These functions need to be reviewed more closely to make sure that all the various possible
 *   floating point errors are being checked for and handled appropriately. We do check errno in
 *   a few places, but we're not sure we're doing that properly. In any case there are plenty of
 *   places where we're not doing any checking.
 *
 * + Some of the bounds checking being done in this file make use of the macros in <cfloat>.
 *   Probably we should use the numeric_traits template instead.
 *
 * + We need to think some more about how we want to report errors. Right now we're just
 *   throwing a generic Entity::Error object with a reasonably appropriate message. Do we want
 *   to distinguish between range errors, domain errors, underflow, overflow, etc?
 *
 * + We should improve my handling of pi and e. Should we create a constants library for clac
 *   and store carefully prepared values of various constants there? clac users will, after all,
 *   need some of these values too.
 *
 * + FloatEntity::to_integer is not as good as it should be. It uses the modf function and then
 *   converts the resulting integer part to type int. But since IntegerEntity uses VeryLong for
 *   its representation, it can do better than that. Also the way it handles rounding is a bit
 *   simple minded.
 */

#include <cerrno>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "Entities.hpp"
#include "DisplayState.hpp"
#include "support.hpp"

using namespace std;

// These two constants are fundamental.
const double pi = 3.141592653589793238462643;
const double e  = 2.718281828459045235360287;

//=================================================
//           Internally Linked Functions
//=================================================

//
// The following function takes a floating point number and returns a mantissa and an exponent
// such that number = mantissa * 10^(exponent). The returned exponent is an integer. This
// function is used during the display of FloatEntities (particularly in engineering notation).
//
static double frexp10( double number, int *exp )
{
    int    int_num;  // Exponent of 2.
    double flt_num;  // Mantissa that goes with the exponent of 2.
    double temp;     // Raw exponent of 10.
    double frac_part, int_part; // Parts of the raw exponent of 10.
    double mantissa; // Mantissa that goes with the final exponent of 10.

    // Compute number = flt_num * 2^(int_num). Note that some implementations have problems with
    // this when number is 0.0.
    //
    flt_num = frexp( number, &int_num );

    temp      = int_num * log10( 2.0 );           // Compute raw exponet of 10.
    frac_part = modf( temp, &int_part );          // Find parts of raw exponent.
    mantissa  = flt_num * pow( 10.0, frac_part ); // Get the final mantissa.

    // Normalize all mantissa into the range 1.0 -> 10.0
    if( fabs( mantissa ) < 1.0 ) {
        mantissa *= 10.0;
        int_part -= 1.0;
    }

    // Return results.
    *exp = static_cast< int >( int_part );
    return mantissa;
}


//
// The following function adjusts the mantissa and exponent of a number so that it is in
// engineering notation. Normally numbers are displayed in the form: 3.141e5. But engineering
// notation requires an exponent that is a multiple of 3. Thus: 314.1e3.
//
static void eng_adjust( double &mantissa, int &exponent )
{
    // Loop until the exponent is evenly divisable by three.
    while( abs( exponent ) % 3 != 0 ) {

        // Shift mantissa over one place and adjust exponent.
        mantissa *= 10.0;
        exponent--;
    }
}


//==========================================
//           FloatEntity Members
//==========================================

FloatEntity::FloatEntity( double number )
{
    value = number;
}

EntityType FloatEntity::my_type( ) const
{
    return FLOAT;
}

//
// The following function formats a FloatEntity according to the current formatting flags. It
// uses <cstdio> functions to format the result into a fixed buffer and then returns that buffer
// as a string. This code should probably be updated to use iostreams at some point. However,
// this basically works so I'm not going to worry about it now.
//
std::string FloatEntity::display( ) const
{
    static char buffer[128];

    switch( DisplayState::get_display_mode( ) ) {
    case DisplayState::FIXED:
        sprintf( buffer, "%.*f", DisplayState::get_decimal_count( ), value );
        break;

    case DisplayState::SCIENTIFIC:
        sprintf( buffer, "%.*E", DisplayState::get_decimal_count( ), value );
        break;

    case DisplayState::ENGINEERING: {
        int    exponent;
        double mantissa     = frexp10( value, &exponent );
        int    old_exponent = exponent;

        eng_adjust( mantissa, exponent );

        int mantissa_decimals =
            DisplayState::get_decimal_count( ) - ( old_exponent - exponent );
        if( mantissa_decimals < 0 ) mantissa_decimals = 0;

        if( exponent < 0 )
            sprintf( buffer, "%.*fE%+02d", mantissa_decimals, mantissa, exponent );
        else
            sprintf( buffer, "%.*fE%+03d", mantissa_decimals, mantissa, exponent );
    }
    break;

    default:
        sprintf( buffer, "INTERNAL ERROR: Bad display mode" );
        break;
    }
    return buffer;
}


Entity *FloatEntity::duplicate( ) const
{
    return new FloatEntity( value );
}


//
// Unary operations
//

Entity *FloatEntity::abs( ) const
{
    double new_value = fabs( value );
    return new FloatEntity( new_value );
}


Entity *FloatEntity::acos( ) const
{
    if( value > 1.0 ) {
        return new ComplexEntity( ::acos( 1.0 ), -::log( value + ::sqrt( value * value - 1.0 ) ) );
    }
    else {
        return new FloatEntity( from_radians( ::acos( value ) ) );
    }
}


Entity *FloatEntity::asin( ) const
{
    if( value > 1.0 ) {
        return new ComplexEntity( ::asin( 1.0 ), ::log( value + ::sqrt( value * value - 1.0 ) ) );
    }
    else {
        return new FloatEntity( from_radians( ::asin( value ) ) );
    }
}


Entity *FloatEntity::atan( ) const
{
    return new FloatEntity( from_radians( ::atan( value ) ) );
}


Entity *FloatEntity::complex_conjugate( ) const
{
    return new FloatEntity( value );
}


Entity *FloatEntity::cos( ) const
{
    return new FloatEntity( ::cos( to_radians( value ) ) );
}


Entity *FloatEntity::exp( ) const
{
    errno = 0;
    double temp = ::exp( value );
    if( errno == ERANGE ) {
        throw Error( "Overflow: Can't compute e^x for such a large x" );
    }
    return new FloatEntity( temp );
}


Entity *FloatEntity::exp10( ) const
{
    errno = 0;
    double temp = pow( 10.0, value );
    if( errno == ERANGE ) {
        throw Error( "Overflow: Can't compute 10^x for such a large x" );
    }
    return new FloatEntity( temp );
}


Entity *FloatEntity::fractional_part( ) const
{
    double dummy;
    return new FloatEntity( modf( value, &dummy ) );
}


Entity *FloatEntity::imaginary_part( ) const
{
    return new FloatEntity( 0.0 );
}


Entity *FloatEntity::integer_part( ) const
{
    double result;
    modf( value, &result );
    return new FloatEntity( result );
}


Entity *FloatEntity::inv( ) const
{
    // NOTE: this function does not worry about overflow or underflow if an attempt is made to
    // invert DBL_MAX or DBL_MIN.

    if( value == 0.0 ) throw Error( "Can't invert zero" );
    return new FloatEntity( 1.0 / value );
}


Entity *FloatEntity::ln( ) const
{
    if( value == 0.0 ) throw Error( "Can't take the natural log of zero" );

    if( value < 0.0 ) {
        return new ComplexEntity( ::log( fabs( value ) ), pi );
    }
    return new FloatEntity( ::log( value ) );
}


Entity *FloatEntity::log( ) const
{
    if( value == 0.0 ) throw Error( "Can't take the log of zero" );

    if( value < 0.0 ) {
        return new ComplexEntity( log10( fabs( value ) ), pi * log10( e ) );
    }
    return new FloatEntity( log10( value ) );
}


Entity *FloatEntity::neg( ) const
{
    return new FloatEntity( -1.0 * value );
}


Entity *FloatEntity::real_part( ) const
{
    return new FloatEntity( value );
}


Entity *FloatEntity::sign( ) const
{
    if( value > 0.0 )
        return new FloatEntity( 1.0 );

    else if( value < 0.0 )
        return new FloatEntity( -1.0 );

    return new FloatEntity( value );
}


Entity *FloatEntity::sin( ) const
{
    return new FloatEntity( ::sin( to_radians( value ) ) );
}


Entity *FloatEntity::sq( ) const
{
    // Be sure the number we are squaring is positive.
    double temp = fabs( value );

    // Deal with the possibility of overflow.
    if( temp > 1.0 ) {
        if( DBL_MAX / temp < temp ) {
            throw Error( "Can't square number with such a large magnitude" );
        }
    }

    // Deal with the possibility of underflow. (Or should I just return 0.0?)
    else {
        if( temp / DBL_MIN  < 1.0 / temp ) {
            throw Error( "Can't square a number with such a small magnitude" );
        }
    }
    return new FloatEntity( temp * temp );
}


Entity *FloatEntity::sqrt( ) const
{
    if( value < 0.0 ) {
        return  new ComplexEntity( 0.0, ::sqrt( fabs( value ) ) );
    }
    else {
        return new FloatEntity( ::sqrt( value ) );
    }
}


Entity *FloatEntity::tan( ) const
{
    errno = 0;
    double temp = ::tan( to_radians( value ) );
    if( errno == ERANGE ) {
        throw Error( "Can't take the tangent of pi/2 + n*pi radians" );
    }
    return new FloatEntity( temp );
}


//
// Binary operations
//

Entity *FloatEntity::divide( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    if( right->value == 0.0 ) throw Error( "Can't divide by zero" );
    return new FloatEntity( value / right->value );
}


Entity *FloatEntity::minus( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new FloatEntity( value - right->value );
}


Entity *FloatEntity::multiply( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new FloatEntity( value * right->value );
}


Entity *FloatEntity::plus( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new FloatEntity( value + right->value );
}


Entity *FloatEntity::power( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new FloatEntity( pow( value, right->value ) );
}


//
// Relational operations
//

Entity *FloatEntity::is_equal( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value == right->value );
}


Entity *FloatEntity::is_notequal( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value != right->value );
}


Entity *FloatEntity::is_less( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value < right->value );
}


Entity *FloatEntity::is_lessorequal( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value <= right->value );
}


Entity *FloatEntity::is_greater( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value > right->value );
}


Entity *FloatEntity::is_greaterorequal( const Entity *R ) const
{
    const FloatEntity *right = dynamic_cast< const FloatEntity * >( R );
    return new IntegerEntity( value >= right->value );
}


//
// Conversions from FloatEntity
//

Entity *FloatEntity::to_complex( ) const
{
    return new ComplexEntity( value );
}

Entity *FloatEntity::to_float( ) const
{
    return duplicate( );
}

Entity *FloatEntity::to_integer( ) const
{
    double I_part;
    double fraction;

    fraction = modf( value, &I_part );
    if( value > 0 && fraction > 0.5 ) I_part += 1.0;
    if( value < 0 && fraction < 0.5 ) I_part -= 1.0;

    int result = static_cast< int >( I_part );
    return new IntegerEntity( result );
}
