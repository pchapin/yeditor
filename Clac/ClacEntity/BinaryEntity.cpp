/*! \file    BinaryEntity.cpp
 *  \brief   Implementation of the clac numeric type BinaryEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * Objects of this class are unsigned integers which can be manipulated on a bitwise bases. The
 * current implementation allows for a maximum of 32 bits. The actual number of bits being used
 * is controlled by the user and available through Global::get_bit_count. Every time an
 * operation is applied to a BinaryEntity, the most significant bits (which are unused) are
 * masked to zero by the function normalize.
 */

#include <memory>
#include "Entities.hpp"
#include "DisplayState.hpp"
#include "support.hpp"

using std::auto_ptr;

// Number of decimal digits required to display the binaries with bit counts from 1 to 32. This
// is used in the display method. (currently not used)
//
#ifdef NEVER
static int power_sizes[] = {
     1,  1,  1,  1,  2,  2,
     2,  3,  3,  3,  4,  4,
     4,  4,  5,  5,  5,  6,
     6,  6,  7,  7,  7,  7,
     8,  8,  8,  9,  9,  9,
    10, 10, 10
  };
#endif


// Zero out most significant bits depending on the number of bits in the entity.
static unsigned long bit_mask[] = {
  0x00000000UL, 0x00000001UL, 0x00000003UL, 0x00000007UL, 0x0000000FUL,
                0x0000001FUL, 0x0000003FUL, 0x0000007FUL, 0x000000FFUL,
                0x000001FFUL, 0x000003FFUL, 0x000007FFUL, 0x00000FFFUL,
                0x00001FFFUL, 0x00003FFFUL, 0x00007FFFUL, 0x0000FFFFUL,
                0x0001FFFFUL, 0x0003FFFFUL, 0x0007FFFFUL, 0x000FFFFFUL,
                0x001FFFFFUL, 0x003FFFFFUL, 0x007FFFFFUL, 0x00FFFFFFUL,
                0x01FFFFFFUL, 0x03FFFFFFUL, 0x07FFFFFFUL, 0x0FFFFFFFUL,
                0x1FFFFFFFUL, 0x3FFFFFFFUL, 0x7FFFFFFFUL, 0xFFFFFFFFUL
};


void BinaryEntity::normalize( )
{
    // value &= bit_mask[Global::get_bit_count( )];
    // What should be done about global settings in the entity library?

    value &= bit_mask[16];
}

#ifdef NEVER

// The following function was used in the original Clac to format binary entities. It needs to
// be updated for the current system.

static char *format_binary( unsigned long number )
{
    // Contains the final string. String is loaded in reverse order.
    static char number_buffer[128];

    char *buffer_pointer = number_buffer;
    int   digit_count;

    switch( DisplayState::get_base( ) ) {

    case DisplayState::HEX: {

        // In HEX, there are four bits per digit.
        digit_count = ( Global::get_bit_count( ) - 1 ) / 4 + 1;

        // Install suffix character and loop over all required digits.
        *buffer_pointer++ = 'h';
        for( int counter = 0; counter < digit_count; counter++ ) {

            // Compute digit and install in number_buffer.
            int digit = static_cast< int >( number % 16 );
            if( digit >= 10 )
                *buffer_pointer++ = static_cast< char >( digit + 'A' - 10 );
            else
                *buffer_pointer++ = static_cast< char >( digit + '0' );
            number /= 16;
        }
    }
    break;

    case DisplayState::BINARY: {

        // In binary, the number of digits IS the digit count.
        digit_count = Global::get_bit_count( );

        // Process digits.
        *buffer_pointer++ = 'b';
        for( int counter = 0; counter < digit_count; counter++ ) {
            int digit = static_cast< int >( number % 2 );
            *buffer_pointer++ = static_cast< char >( digit + '0' );
            number /= 2;
        }
    }
    break;

    case DisplayState::OCTAL: {

        // In octal, the are three bits per digit.
        digit_count = ( Global::get_bit_count( ) - 1 ) / 3 + 1;

        // Process digits.
        *buffer_pointer++ = 'o';
        for( int counter = 0; counter < digit_count; counter++ ) {
            int digit = static_cast< int >( number % 8 );
            *buffer_pointer++ = static_cast< char >( digit + '0' );
            number /= 8;
        }
    }
    break;

    case DisplayState::DECIMAL: {

        // In decimal, look up the required number of digits in power_sizes.
        digit_count = power_sizes[Global::get_bit_count( )];

        // Process digits.
        *buffer_pointer++ = 'd';
        for( int counter = 0; counter < digit_count; counter++ ) {
            int digit = static_cast< int >( number % 10 );
            *buffer_pointer++ = static_cast< char >( digit + '0' );
            number /= 10;
        }
    }
    break;
    }

    // Install the "# " prefix.
    *buffer_pointer++ =  ' ';
    *buffer_pointer++ =  '#';
    *buffer_pointer++ = '\0';

    // Reverse the string so that the number is displayed properly.
    strrev( number_buffer );
    return number_buffer;
}
#endif

EntityType BinaryEntity::my_type( ) const
{
    return BINARY;
}

std::string BinaryEntity::display( ) const
{
    return "BinaryEntity::display( ) not implemented!";
}

Entity *BinaryEntity::duplicate( ) const
{
    return new BinaryEntity( value );
}


//
// Unary operations
//

Entity *BinaryEntity::abs( ) const
{
    return duplicate( );
}

Entity *BinaryEntity::acos( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->acos( );
}

Entity *BinaryEntity::asin( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->asin( );
}

Entity *BinaryEntity::atan( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->atan( );
}

Entity *BinaryEntity::complex_conjugate( ) const
{
    return duplicate( );
}

Entity *BinaryEntity::cos( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->cos( );
}

Entity *BinaryEntity::exp( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->exp( );
}

Entity *BinaryEntity::exp10( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->exp10( );
}

Entity *BinaryEntity::fractional_part( ) const
{
    return new BinaryEntity( 0UL );
}

Entity *BinaryEntity::imaginary_part( ) const
{
    return new BinaryEntity( 0UL );
}

Entity *BinaryEntity::integer_part( ) const
{
    return duplicate( );
}

Entity *BinaryEntity::inv( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->inv( );
}

Entity *BinaryEntity::ln( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->ln( );
}

Entity *BinaryEntity::log( ) const
{
    auto_ptr< FloatEntity > converted( static_cast< FloatEntity * >( to_float( ) ) );
    return converted->log( );
}

Entity *BinaryEntity::logical_not( ) const
{
    unsigned long new_value = ~value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::neg( ) const
{
    unsigned long new_value = ~value;
    new_value++;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}


//
// Binary operations
//

Entity *BinaryEntity::divide( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value / right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::logical_and( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value & right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::logical_or( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value | right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::logical_xor( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value ^ right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::minus( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value - right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::multiply( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value * right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}

Entity *BinaryEntity::plus( const Entity *R ) const
{
    const BinaryEntity *right = dynamic_cast< const BinaryEntity * >( R );
    unsigned long new_value = value + right->value;
    BinaryEntity *result = new BinaryEntity( new_value );
    return result;
}


//
// Conversions from BinaryEntity
//

Entity *BinaryEntity::to_binary( ) const
{
    return duplicate( );
}

Entity *BinaryEntity::to_complex( ) const
{
    return new ComplexEntity( static_cast< double >( value ) );
}

Entity *BinaryEntity::to_float( ) const
{
    return new FloatEntity( static_cast< double >( value ) );
}

Entity *BinaryEntity::to_integer( ) const
{
    return new IntegerEntity( value );
}
