/*! \file    IntegerEntity.cpp
 *  \brief   Implementation of the clac numeric type IntegerEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * + IntegerEntity::to_float doesn't deal with the possibility of overflow and it should.
 */

#include <memory>
#include <sstream>

#include "Entities.hpp"

using namespace std;

IntegerEntity::IntegerEntity( const VeryLong &number ) :
  value( number )
  {  }

EntityType IntegerEntity::my_type( ) const
{
    return INTEGER;
}

std::string IntegerEntity::display( ) const
{
    std::ostringstream formatter;

    formatter << value;
    return formatter.str( );
}


Entity *IntegerEntity::duplicate( ) const
{
    return new IntegerEntity( *this );
}


//
// Unary operations.
//

Entity *IntegerEntity::abs( ) const
{
    IntegerEntity *result;
    if( value < VeryLong::zero ) result = new IntegerEntity( -value );
        else result = new IntegerEntity( value );
    return result;
}


Entity *IntegerEntity::acos() const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->acos( );
}


Entity *IntegerEntity::asin( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->asin( );
}


Entity *IntegerEntity::atan( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->atan( );
}


Entity *IntegerEntity::complex_conjugate( ) const
{
    return duplicate( );
}


Entity *IntegerEntity::cos( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->cos( );
}


Entity *IntegerEntity::exp( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->exp( );
}


Entity *IntegerEntity::exp10( ) const
{
    auto_ptr< IntegerEntity > base( new IntegerEntity( VeryLong::ten ) );
    return base->power( this );
}


Entity *IntegerEntity::fractional_part( ) const
{
    return new IntegerEntity( VeryLong::zero );
}


Entity *IntegerEntity::imaginary_part( ) const
{
    return new IntegerEntity( VeryLong::zero );
}


Entity *IntegerEntity::integer_part( ) const
{
    return duplicate( );
}


//
// Would it be better to have inv return a RationalEntity? My guess is no. I think Rationals
// will only be used in specialized situations and in a vast majority of cases the user will
// want 1/x to become a floating value when x is an integer. Perhaps this is something that
// could eventually be controlled by a setting.
//
Entity *IntegerEntity::inv( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->inv( );
}


Entity *IntegerEntity::ln( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->ln( );
}


Entity *IntegerEntity::log( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->log( );
}


Entity *IntegerEntity::neg( ) const
{
    return new IntegerEntity( -value );
}


Entity *IntegerEntity::real_part( ) const
{
    return duplicate( );
}


Entity *IntegerEntity::sign( ) const
{
    IntegerEntity *result;

    if( value == VeryLong::zero )
        result = new IntegerEntity( VeryLong::zero );

    else if( value > VeryLong::zero )
        result = new IntegerEntity( VeryLong::one );

    else  /* value < 0 */
        result = new IntegerEntity( VeryLong::negative_one );

    return result;
}


Entity *IntegerEntity::sin( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->sin( );
}


Entity *IntegerEntity::sq( ) const
{
    auto_ptr< IntegerEntity > temp( static_cast< IntegerEntity * >( duplicate( ) ) );
    temp->value *= value;
    return temp.release( );
}


Entity *IntegerEntity::sqrt( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->sqrt( );
}


Entity *IntegerEntity::tan( ) const
{
    auto_ptr< Entity > converted( to_float( ) );
    return converted->tan( );
}


//
// Binary operations.
//

Entity *IntegerEntity::divide( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    auto_ptr< IntegerEntity > copy( static_cast< IntegerEntity * >( duplicate( ) ) );
    copy->value /= right->value;
    return copy.release( );
}


Entity *IntegerEntity::minus( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    auto_ptr< IntegerEntity > copy( static_cast< IntegerEntity * >( duplicate( ) ) );
    copy->value -= right->value;
    return copy.release( );
}


Entity *IntegerEntity::modulo( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    auto_ptr< IntegerEntity > copy( static_cast< IntegerEntity * >( duplicate( ) ) );
    copy->value %= right->value;
    return copy.release( );
}


Entity *IntegerEntity::multiply( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    auto_ptr< IntegerEntity > copy( static_cast< IntegerEntity * >( duplicate( ) ) );
    copy->value *= right->value;
    return copy.release( );
}


Entity *IntegerEntity::plus( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    auto_ptr< IntegerEntity > copy( static_cast< IntegerEntity * >( duplicate( ) ) );
    copy->value += right->value;
    return copy.release( );
}


//
// There is a faster way of computing a power using recursion than the method I'm using here.
// This method is O(N) where N is the size of the exponent. The faster method is O(log(N)).
// Since VeryLongs can be large, I should probably change this function someday to use the more
// efficient method. It might make a difference for some applications.
// 
Entity *IntegerEntity::power( const Entity *R ) const
{
  const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
  auto_ptr< IntegerEntity > result( new IntegerEntity( VeryLong::one ) );
  VeryLong exponent( right->value );

  // If we are using a negative exponent, compute the appropriate power and then invert it. Note
  // that inv currently returns a (pointer to a) FloatEntity. Most likely this is what the user
  // wants when applying a negative exponent to an integer anyway.
  //
  if( exponent < VeryLong::zero ) {
    while( exponent < VeryLong::zero ) {
      result->value *= value;
      ++exponent;
    }
    return result->inv( );
  }

  // Otherwise it's a positive (or zero) exponent.
  while( exponent > VeryLong::zero ) {
    result->value *= value;
    --exponent;
  }

  return result.release( );
}


//
// Relational operations.
//

Entity *IntegerEntity::is_equal( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value == right->value );
}


Entity *IntegerEntity::is_notequal( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value != right->value );
}


Entity *IntegerEntity::is_less( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value < right->value );
}


Entity *IntegerEntity::is_lessorequal( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value <= right->value );
}


Entity *IntegerEntity::is_greater( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value > right->value );
}


Entity *IntegerEntity::is_greaterorequal( const Entity *R ) const
{
    const IntegerEntity *right = dynamic_cast< const IntegerEntity * >( R );
    return new IntegerEntity( value >= right->value );
}


//
// Conversions from IntegerEntity
//

Entity *IntegerEntity::to_float( ) const
{
    VeryLong::size_type bit_count = value.number_bits( );
    double result = 0.0;

    for( VeryLong::size_type i = 0; i < bit_count; i++ ) {
        result *= 2.0;
        result += value.get_bit( bit_count - i - 1 );
    }
    if( value < 0 ) result = -result;

    return new FloatEntity( result );
}

Entity *IntegerEntity::to_integer( ) const
{
    return duplicate( );
}
