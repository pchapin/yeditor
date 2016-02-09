/*! \file    StringEntity.cpp
 *  \brief   Implementation of the clac numeric type StringEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Entities.hpp"

EntityType StringEntity::my_type( ) const
{
    return STRING;
}

std::string StringEntity::display( ) const
{
    return value;
}

Entity *StringEntity::duplicate( ) const
{
    return new StringEntity( value );
}

Entity *StringEntity::plus( const Entity *R ) const
{
    const StringEntity *right = dynamic_cast< const StringEntity * >( R );
    return new StringEntity( value + right->value );
}


//
// Conversions from StringEntity
//

Entity *StringEntity::to_string( ) const
{
    return duplicate( );
}
