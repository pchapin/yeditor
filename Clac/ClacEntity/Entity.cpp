/*! \file    Entity.cpp
    \brief   Implementation of the abstract base class Entity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

The functions in this file provide default behavior for the operations that an Entity classes
doesn't wish to provide. These functions mostly just produce errors by throwing an appropriate
exception.
*/

#include "Entity.hpp"

Entity::~Entity( )
{
    return;
}

//
// Unary operations.
//

Entity *Entity::abs( ) const
{
    throw Error( "Unable to take absolute value of object" );
    return NULL;
}

Entity *Entity::acos( ) const
{
    throw Error( "Unable to take arccosine of object" );
    return NULL;
}

Entity *Entity::asin( ) const
{
    throw Error( "Unable to take arcsine of object" );
    return NULL;
}

Entity *Entity::atan( ) const
{
    throw Error( "Unable to take arctangent of object" );
    return NULL;
}

Entity *Entity::complex_conjugate( ) const
{
    throw Error( "Unable to take complex conjugate of object" );
    return NULL;
}

Entity *Entity::cos( ) const
{
    throw Error( "Unable to take cosine of object" );
    return NULL;
}

Entity *Entity::exp( ) const
{
    throw Error( "Unable to exponentiate object" );
    return NULL;
}

Entity *Entity::exp10( ) const
{
    throw Error( "Unable to exponentiate object" );
    return NULL;
}

Entity *Entity::fractional_part( ) const
{
    throw Error( "Object has no fractional part" );
    return NULL;
}

Entity *Entity::imaginary_part( ) const
{
    throw Error( "Object has no imaginary part" );
    return NULL;
}

Entity *Entity::integer_part( ) const
{
    throw Error( "Object has no integer part" );
    return NULL;
}

Entity *Entity::inv( ) const
{
    throw Error( "Unable to invert object" );
    return NULL;
}

Entity *Entity::ln( ) const
{
    throw Error( "Unable to take natural logarithm of object" );
    return NULL;
}

Entity *Entity::log( ) const
{
    throw Error( "Unable to take logarithm of object" );
    return NULL;
}

Entity *Entity::logical_not( ) const
{
    throw Error( "Unable to logically negate object" );
    return NULL;
}

Entity *Entity::neg( ) const
{
    throw Error( "Unable to negate object" );
    return NULL;
}

Entity *Entity::real_part( ) const
{
    throw Error( "Object has no real part" );
    return NULL;
}

Entity *Entity::rotate_left( ) const
{
    throw Error( "Unable to rotate object to the left" );
    return NULL;
}

Entity *Entity::rotate_right( ) const
{
    throw Error( "Unable to rotate object to the right" );
    return NULL;
}

Entity *Entity::shift_left( ) const
{
    throw Error( "Unable to shift object to the left" );
    return NULL;
}

Entity *Entity::shift_right( ) const
{
    throw Error( "Unable to shift object to the right" );
    return NULL;
}

Entity *Entity::sign( ) const
{
    throw Error( "Unable to find sign of object" );
    return NULL;
}

Entity *Entity::sin( ) const
{
    throw Error( "Unable to take sine of object" );
    return NULL;
}

Entity *Entity::sq( ) const
{
    throw Error( "Unable to square object" );
    return NULL;
}

Entity *Entity::sqrt( ) const
{
    throw Error( "Unable to take square root of object" );
    return NULL;
}

Entity *Entity::tan( ) const
{
    throw Error( "Unable to take tangent of object" );
    return NULL;
}

Entity *Entity::transpose( ) const
{
    throw Error( "Unable to transpose object" );
    return NULL;
}

//
// Conversion Functions.
//

Entity *Entity::to_binary( ) const
{
    throw Error( "Unable to convert object to a binary" );
    return NULL;
}

Entity *Entity::to_complex( ) const
{
    throw Error( "Unable to convert object to a complex" );
    return NULL;
}

Entity *Entity::to_directory( ) const
{
    throw Error( "Unable to convert object to a directory" );
    return NULL;
}

Entity *Entity::to_float( ) const
{
    throw Error( "Unable to convert object to a float" );
    return NULL;
}

Entity *Entity::to_integer( ) const
{
    throw Error( "Unable to convert object to an integer" );
    return NULL;
}

Entity *Entity::to_labeled( ) const
{
    throw Error( "Unable to convert object to a labeled object" );
    return NULL;
}

Entity *Entity::to_list( ) const
{
    throw Error( "Unable to convert object to a list" );
    return NULL;
}

Entity *Entity::to_matrix( ) const
{
    throw Error( "Unable to convert object to a matrix" );
    return NULL;
}

Entity *Entity::to_program( ) const
{
    throw Error( "Unable to convert object to a program" );
    return NULL;
}

Entity *Entity::to_rational( ) const
{
    throw Error( "Unable to convert object to a rational number" );
    return NULL;
}

Entity *Entity::to_string( ) const
{
    throw Error( "Unable to convert object to a string" );
    return NULL;
}

Entity *Entity::to_vector( ) const
{
    throw Error( "Unable to convert object to a vector" );
    return NULL;
}

//
// Binary operations.
//

Entity *Entity::cross( const Entity * ) const
{
    throw Error( "Unable to take cross product of these objects" );
    return NULL;
}

Entity *Entity::divide( const Entity * ) const
{
    throw Error( "Unable to divide these objects" );
    return NULL;
}

Entity *Entity::dot( const Entity * ) const
{
    throw Error( "Unable to take dot product of these objects" );
    return NULL;
}

Entity *Entity::logical_and( const Entity * ) const
{
    throw Error( "Unable to logically AND these objects" );
    return NULL;
}

Entity *Entity::logical_or( const Entity * ) const
{
    throw Error( "Unable to logically OR these objects" );
    return NULL;
}

Entity *Entity::logical_xor( const Entity * ) const
{
    throw Error( "Unable to logically exclusive OR these objects" );
    return NULL;
}

Entity *Entity::minus( const Entity * ) const
{
    throw Error( "Unable to subtract these objects" );
    return NULL;
}

Entity *Entity::modulo( const Entity * ) const
{
    throw Error( "Unable to modulo these objects" );
    return NULL;
}

Entity *Entity::multiply( const Entity * ) const
{
    throw Error( "Unable to multiply these objects" );
    return NULL;
}

Entity *Entity::plus( const Entity * ) const
{
    throw Error( "Unable to add these objects" );
    return NULL;
}

Entity *Entity::power( const Entity * ) const
{
    throw Error( "Unable to exponentiate these objects" );
    return NULL;
}

//
// Relational operations.
//

Entity *Entity::is_equal( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

Entity *Entity::is_notequal( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

Entity *Entity::is_less( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

Entity *Entity::is_lessorequal( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

Entity *Entity::is_greater( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

Entity *Entity::is_greaterorequal( const Entity * ) const
{
    throw Error( "Unable to compare these objects" );
    return NULL;
}

//
// File operations.
//

// This should be made abstract eventually. Doing so now will cause problems for Entity classes
// that have not yet gotten around to implementing it.
//
long Entity::file_size( ) const
{
    return 0;
}

void Entity::write( std::ofstream & ) const
{
    throw Error( "Unable to write object to a file" );
}

// This function is static and will eventually have to be fleshed out (and probably moved to a
// different file in order to handle all the entity types.
//
Entity *Entity::read( std::ifstream & )
{
    throw Error( "Unable to read files" );
    return NULL;
}
