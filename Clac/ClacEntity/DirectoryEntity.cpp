/*! \file    DirectoryEntity.cpp
 *  \brief   Implementation of the clac numeric type DirectoryEntity.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include "Entities.hpp"

DirectoryEntity::DirectoryEntity( const std::map< std::string, Entity * > &existing )
{
    std::map< std::string, Entity * >::const_iterator p;

    try {
        for( p = existing.begin( ); p != existing.end( ); ++p ) {
            Entity *dup = p->second->duplicate( );
            value[p->first] = dup;
        }
    }
    catch( ... ) {
        for( p = value.begin( ); p != value.end( ); ++p ) {
            delete p->second;
        }
        throw;
    }
}

DirectoryEntity::~DirectoryEntity( )
{
    std::map< std::string, Entity * >::iterator p;
    
    for( p = value.begin( ); p != value.end( ); ++p ) {
        delete p->second;
    }
}

EntityType DirectoryEntity::my_type( ) const
{
    return DIRECTORY;
}

std::string DirectoryEntity::display( ) const
{
    return "DirectoryEntity::display( ) not implemented!";
}

Entity *DirectoryEntity::duplicate( ) const
{
    return new DirectoryEntity( value );
}
