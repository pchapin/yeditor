/*! \file    IntegerEntity.hpp
    \brief   Interface to the clac numeric type IntegerEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef INTEGERENTITY_HPP
#define INTEGERENTITY_HPP

#include "Entity.hpp"
#include "VeryLong.hpp"

class IntegerEntity : public Entity {
public:

    // For bulding an integer entity from its primitive.
    IntegerEntity( const VeryLong &number );

    VeryLong get_value( )
      { return value; }

    // Functions for maintaining a member of the Entity family.
    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

    // Unary operations.
    virtual Entity *abs              ( ) const;
    virtual Entity *acos             ( ) const;
    virtual Entity *asin             ( ) const;
    virtual Entity *atan             ( ) const;
    virtual Entity *complex_conjugate( ) const;
    virtual Entity *cos              ( ) const;
    virtual Entity *exp              ( ) const;
    virtual Entity *exp10            ( ) const;
    virtual Entity *fractional_part  ( ) const;
    virtual Entity *imaginary_part   ( ) const;
    virtual Entity *integer_part     ( ) const;
    virtual Entity *inv              ( ) const;
    virtual Entity *ln               ( ) const;
    virtual Entity *log              ( ) const;
    virtual Entity *neg              ( ) const;
    virtual Entity *real_part        ( ) const;
    virtual Entity *sign             ( ) const;
    virtual Entity *sin              ( ) const;
    virtual Entity *sq               ( ) const;
    virtual Entity *sqrt             ( ) const;
    virtual Entity *tan              ( ) const;

    // Conversion operations.
    virtual Entity *to_float   ( ) const;
    virtual Entity *to_integer ( ) const;

    // Binary operations.
    virtual Entity *divide   ( const Entity * ) const;
    virtual Entity *minus    ( const Entity * ) const;
    virtual Entity *modulo   ( const Entity * ) const;
    virtual Entity *multiply ( const Entity * ) const;
    virtual Entity *plus     ( const Entity * ) const;
    virtual Entity *power    ( const Entity * ) const;

    // Relational operations.
    virtual Entity *is_equal          ( const Entity * ) const;
    virtual Entity *is_notequal       ( const Entity * ) const;
    virtual Entity *is_less           ( const Entity * ) const;
    virtual Entity *is_lessorequal    ( const Entity * ) const;
    virtual Entity *is_greater        ( const Entity * ) const;
    virtual Entity *is_greaterorequal ( const Entity * ) const;

private:
    VeryLong value;

};

#endif

