/*! \file    ProgramEntity.cpp
 *  \brief   Implementation of the clac numeric type ProgramEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Entities.hpp"

EntityType ProgramEntity::my_type( ) const
{
    return PROGRAM;
}

std::string ProgramEntity::display( ) const
{
    return "ProgramEntity::display( ) not implemented!";
}

Entity *ProgramEntity::duplicate( ) const
{
    return new ProgramEntity( value );
}

