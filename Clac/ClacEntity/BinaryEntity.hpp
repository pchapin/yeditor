/*! \file    BinaryEntity.hpp
    \brief   Interface to the clac numeric type BinaryEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef BINARYENTITY_HPP
#define BINARYENTITY_HPP

#include <string>
#include "Entity.hpp"

class BinaryEntity : public Entity {
public:
    BinaryEntity( )  { value = 0UL; }
    BinaryEntity( unsigned long number ) { value = number; normalize( ); }

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

    // Unary operations.
    virtual Entity *abs( ) const;
    virtual Entity *acos( ) const;
    virtual Entity *asin( ) const;
    virtual Entity *atan( ) const;
    virtual Entity *complex_conjugate( ) const;
    virtual Entity *cos( ) const;
    virtual Entity *exp( ) const;
    virtual Entity *exp10( ) const;
    virtual Entity *fractional_part( ) const;
    virtual Entity *imaginary_part( ) const;
    virtual Entity *integer_part( ) const;
    virtual Entity *inv( ) const;
    virtual Entity *ln( ) const;
    virtual Entity *log( ) const;
    virtual Entity *logical_not( ) const;
    virtual Entity *neg( ) const;

    // Binary operations.
    virtual Entity *divide( const Entity * ) const;
    virtual Entity *logical_and( const Entity * ) const;
    virtual Entity *logical_or( const Entity * ) const;
    virtual Entity *logical_xor( const Entity * ) const;
    virtual Entity *minus( const Entity * ) const;
    virtual Entity *multiply( const Entity * ) const;
    virtual Entity *plus( const Entity * ) const;

    // Conversions.
    virtual Entity *to_binary( ) const;
    virtual Entity *to_complex( ) const;
    virtual Entity *to_float( ) const;
    virtual Entity *to_integer( ) const;

private:
    unsigned long value;  // The bits themselves.
    void normalize( );    // Function to zero out most significant bits.

  };

#endif
