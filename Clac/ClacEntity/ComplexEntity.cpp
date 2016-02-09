/*! \file    ComplexEntity.cpp
 *  \brief   Implementation of the clac numeric type ComplexEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <cmath>
#include <complex>
#include <cstdio>
#include <sstream>
#include <string>

#include "Entities.hpp"
#include "DisplayState.hpp"
#include "support.hpp"

static const std::complex< double > j( 0.0, 1.0 );
  // Square root of -1. (We are electrical engineers)

EntityType ComplexEntity::my_type( ) const
{
    return COMPLEX;
}

std::string ComplexEntity::display( ) const
{
    static char  buffer[128];    // Array to hold the resulting string.
    const char *format_string;   // Pointer to a "printf style" format string.

    // Check the complex mode.
    if( DisplayState::get_complex_mode( ) == DisplayState::RECTANGULAR ) {
        switch( DisplayState::get_display_mode( ) ) {
        case DisplayState::FIXED:
            format_string = "(%.*f, %.*f)";
            break;

        case DisplayState::SCIENTIFIC:
            format_string = "(%.*E, %.*E)";
            break;

        default:
            format_string = "INTERNAL ERROR: Bad display mode";
            break;
        }
        std::sprintf( buffer, format_string,
            DisplayState::get_decimal_count( ), value.real( ),
            DisplayState::get_decimal_count( ), value.imag( ) );
    }

    // The complex mode must be POLAR.
    else {
        switch( DisplayState::get_display_mode( ) ) {
        case DisplayState::FIXED:
            format_string = "(%.*f @ %.*f)";
            break;

        case DisplayState::SCIENTIFIC:
            format_string = "(%.*E @ %.*E)";
            break;

        default:
            format_string = "INTERNAL ERROR: Bad display mode";
            break;
        }
        std::sprintf( buffer, format_string,
            DisplayState::get_decimal_count( ), std::abs( value ),
            DisplayState::get_decimal_count( ), from_radians( std::arg( value ) ) );
    }
    return buffer;
}


Entity *ComplexEntity::duplicate( ) const
{
    return new ComplexEntity( value );
}


//
// Unary operations
//

Entity *ComplexEntity::abs( ) const
{
    return new FloatEntity( std::abs( value ) );
}

Entity *ComplexEntity::complex_conjugate( ) const
{
    return new ComplexEntity( std::conj( value ) );
}

Entity *ComplexEntity::cos( ) const
{
    return new ComplexEntity( std::cos( value ) );
}

Entity *ComplexEntity::exp( ) const
{
    return new ComplexEntity( std::exp( value ) );
}

Entity *ComplexEntity::exp10( ) const
{
    return new ComplexEntity( std::pow( 10.0, value ) );
}

Entity *ComplexEntity::imaginary_part( ) const
{
    return new FloatEntity( value.imag( ) );
}

Entity *ComplexEntity::inv( ) const
{
    return new ComplexEntity( 1.0 / value );
}

Entity *ComplexEntity::ln( ) const
{
    // TODO: Implement me!
    throw "ComplexEntity::ln not implemented!";
    return NULL;
}

Entity *ComplexEntity::log( ) const
{
    // TODO: Implement me!
    throw "ComplexEntity::log not implemented!";
    return NULL;
}

Entity *ComplexEntity::neg( ) const
{
    return new ComplexEntity( -value );
}

Entity *ComplexEntity::real_part( ) const
{
    return new FloatEntity( value.real( ) );
}

Entity *ComplexEntity::sign( ) const
{
    // TODO: Implement me!
    throw "ComplexEntity::sign not implemented!";
    return NULL;
}

Entity *ComplexEntity::sin( ) const
{
    return new ComplexEntity( std::sin( value ) );
}

Entity *ComplexEntity::sqrt( ) const
{
    return new ComplexEntity( std::sqrt( value ) );
}

Entity *ComplexEntity::tan( ) const
{
    return new ComplexEntity( std::tan( value ) );
}

//
// Binary operations
//

Entity *ComplexEntity::divide( const Entity *R ) const
{
    const ComplexEntity *right = dynamic_cast< const ComplexEntity * >( R );
    return new ComplexEntity( value / right->value );
}

Entity *ComplexEntity::minus( const Entity *R ) const
{
    const ComplexEntity *right = dynamic_cast< const ComplexEntity * >( R );
    return new ComplexEntity( value - right->value );
}

Entity *ComplexEntity::multiply( const Entity *R ) const
{
    const ComplexEntity *right = dynamic_cast< const ComplexEntity * >( R );
    return new ComplexEntity( value * right->value );
}

Entity *ComplexEntity::plus( const Entity *R ) const
{
    const ComplexEntity *right = dynamic_cast< const ComplexEntity * >( R );
    return new ComplexEntity( value + right->value );
}

Entity *ComplexEntity::power( const Entity *R ) const
{
    // TODO: Implement me!
    throw "ComplexEntity::power not implemented!";
    return NULL;
}


//
// Conversions from ComplexEntity
//

Entity *ComplexEntity::to_complex( ) const
{
    return duplicate( );
}


