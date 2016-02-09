/*! \file    ComplexEntity.hpp
    \brief   Interface to the clac numeric type ComplexEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef COMPLEXENTITY_HPP
#define COMPLEXENTITY_HPP

#include <complex>
#include "Entity.hpp"

class ComplexEntity : public Entity {
public:

    // For building a ComplexEntity from its primitives.
    ComplexEntity( const std::complex< double > &number ) : value( number ) { }
    ComplexEntity( double real_part, double imaginary_part = 0.0) :
      value( real_part, imaginary_part ) { }

    // Functions for maintaining a member of the Entity family.
    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

    // Unary operations.
    virtual Entity *abs( )               const;
    virtual Entity *complex_conjugate( ) const;
    virtual Entity *cos( )               const;
    virtual Entity *exp( )               const;
    virtual Entity *exp10( )             const;
    virtual Entity *imaginary_part( )    const;
    virtual Entity *inv( )               const;
    virtual Entity *ln( )                const;
    virtual Entity *log( )               const;
    virtual Entity *neg( )               const;
    virtual Entity *real_part( )         const;
    virtual Entity *sign( )              const;
    virtual Entity *sin( )               const;
    virtual Entity *sqrt( )              const;
    virtual Entity *tan( )               const;

    // Conversion functions.
    virtual Entity *to_complex( ) const;

    // Binary operations.
    virtual Entity *divide   ( const Entity * ) const;
    virtual Entity *minus    ( const Entity * ) const;
    virtual Entity *multiply ( const Entity * ) const;
    virtual Entity *plus     ( const Entity * ) const;
    virtual Entity *power    ( const Entity * ) const;

private:
    std::complex< double > value;
};

#endif
