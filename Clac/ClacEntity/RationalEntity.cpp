/*! \file    RationalEntity.cpp
 *  \brief   Implementation of the clac numeric type RationalEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <memory>
#include <sstream>
#include <string>

#include "Entities.hpp"

using std::auto_ptr;

//
// The constructor allows a rational entity to be initialized with a Rational object.
//
RationalEntity::RationalEntity( const Rational< VeryLong > &number ) :
  value( number )
  {  }

#ifdef NEVER

// This following code was used in the original clac. It has been partially updated but much
// more work is needed before it is really ready to use. The current version of display is very
// lame so this work should be done eventually.

static char *format_rational( const Rational< VeryLong > &number )
{
    // Make sure we have a buffer big enough to hold whatever comes from format_integer.
    static char  buffer[2 * format_size + 2 + 1];
    char *temp;

    // Get the numerator (with sign if there is one).
    temp = format_integer( sign, number.get_numerator( ) );
    strcpy( buffer, temp );
    strcat( buffer,  "/" );

    // Format the denominator as positive in all cases.
    temp = format_integer(   1, number.get_denominator( ) );
    strcat( buffer, temp );
    return buffer;
}
#endif

EntityType RationalEntity::my_type( ) const
{
    return RATIONAL;
}

std::string RationalEntity::display( ) const
{
    std::ostringstream formatter;

    formatter << value.get_numerator( ) << '/' << value.get_denominator( );
    return formatter.str( );
}

Entity *RationalEntity::duplicate( ) const
{
    return new RationalEntity( value );
}


//
// Unary operations
//
Entity *RationalEntity::abs( ) const
{
    VeryLong new_numerator = value.get_numerator( );
    if( new_numerator < VeryLong::zero ) new_numerator = -new_numerator;

    Rational< VeryLong > new_value( new_numerator, value.get_denominator( ) );
    return new RationalEntity( new_value );
}

Entity *RationalEntity::acos( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->acos( );
}


Entity *RationalEntity::asin( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return  converted->asin( );
}


Entity *RationalEntity::atan( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->atan( );
}


Entity *RationalEntity::complex_conjugate( ) const
{
    return duplicate( );
}


Entity *RationalEntity::cos( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->cos( );
}


Entity *RationalEntity::exp() const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->exp( );
}


Entity *RationalEntity::exp10( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->exp10( );
}


Entity *RationalEntity::imaginary_part( ) const
{
    Rational< VeryLong > new_value( VeryLong::zero, VeryLong::one );
    return new RationalEntity( new_value );
}


Entity *RationalEntity::inv( ) const
{
    if( value.get_numerator( ) == VeryLong::zero )
        throw Error( "Can't divide by zero" );

    Rational< VeryLong > new_value( value.get_denominator( ), value.get_numerator( ) );
    return new RationalEntity( new_value );
}


Entity *RationalEntity::ln( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->ln( );
}


Entity *RationalEntity::log( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->log( );
}


Entity *RationalEntity::neg( ) const
{
    Rational< VeryLong > new_value( -value.get_numerator( ), value.get_denominator( ) );
    return new RationalEntity( new_value );
}


Entity *RationalEntity::real_part( ) const
{
    return duplicate( );
}


Entity *RationalEntity::sign( ) const
{
    VeryLong result( VeryLong::zero );

    if( value.get_numerator( ) < VeryLong::zero )
        result = VeryLong::negative_one;
    if( value.get_numerator( ) > VeryLong::zero )
        result = VeryLong::one;

    return new IntegerEntity( result );
}


Entity *RationalEntity::sin( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->sin( );
}


Entity *RationalEntity::sq( ) const
{
    VeryLong new_numerator = value.get_numerator( );
    VeryLong new_denominator = value.get_denominator( );

    new_numerator *= value.get_numerator( );
    new_denominator *= value.get_denominator( );
    Rational< VeryLong > new_value( new_numerator, new_denominator );

    return new RationalEntity( new_value );
}


Entity *RationalEntity::sqrt( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->sqrt( );
}


Entity *RationalEntity::tan( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->tan( );
}

//
// Binary operations
//

Entity *RationalEntity::plus( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new RationalEntity( value + right->value );
}

Entity *RationalEntity::minus( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new RationalEntity( value - right->value );
}

Entity *RationalEntity::multiply( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new RationalEntity( value * right->value );
}

Entity *RationalEntity::divide( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new RationalEntity( value / right->value );
}

Entity *RationalEntity::power( const Entity *R ) const
{
    // TODO: Implement me!
    throw "RationalEntity::power not implemented!";
    return NULL;
}

//
// Relational operations
//

Entity *RationalEntity::is_equal( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value == right->value ) );
}

Entity *RationalEntity::is_notequal( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value != right->value ) );
}

Entity *RationalEntity::is_less( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value < right->value ) );
}

Entity *RationalEntity::is_lessorequal( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value <= right->value ) );
}

Entity *RationalEntity::is_greater( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value > right->value ) );
}

Entity *RationalEntity::is_greaterorequal( const Entity *R ) const
{
    const RationalEntity *right = dynamic_cast< const RationalEntity * >( R );
    return new IntegerEntity( static_cast< VeryLong >( value >= right->value ) );
}


//
// Conversions from RationalEntity
//

Entity *RationalEntity::to_float( ) const
{
    // TODO: We can do better than this but VeryLong will need a to_double( ) method first.
    // TODO: What happens if the value of the VeryLong is outside the range of double( )?
    long numerator = value.get_numerator( ).to_long( );
    long denominator = value.get_denominator( ).to_long( );

    return new FloatEntity( static_cast<double>( numerator ) / static_cast<double>( denominator ) );
}

Entity *RationalEntity::to_rational( ) const
{
    return duplicate( );
}
