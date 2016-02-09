/*! \file    LabeledEntity.cpp
 *  \brief   Implementation of the clac numeric type LabeledEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Entities.hpp"

EntityType LabeledEntity::my_type( ) const
{
    return LABELED;
}

std::string LabeledEntity::display( ) const
{
    std::string result( label );
    result.append( ": " );
    result.append( object->display( ) );
    return result;
}

Entity *LabeledEntity::duplicate( ) const
{
    Entity *new_object = object->duplicate( );
    return new LabeledEntity( new_object, label );
}
