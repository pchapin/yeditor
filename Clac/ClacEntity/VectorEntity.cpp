/*! \file    VectorEntity.cpp
 *  \brief   Implementation of the clac numeric type VectorEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Entities.hpp"

VectorEntity::~VectorEntity( )
{
    std::vector< Entity * >::iterator p;

    for( p = value.begin( ); p != value.end( ); ++p ) {
        delete *p;
    }
}

EntityType VectorEntity::my_type( ) const
{
    return VECTOR;
}

std::string VectorEntity::display( ) const
{
    return "VectorEntity::display( ) not implemented!";
}

Entity *VectorEntity::duplicate( ) const
{
    throw Error("VectorEntity::duplicate( ) not implemented!");
}

