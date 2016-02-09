/*! \file    DirectoryEntity.hpp
    \brief   Interface to the clac numeric type DirectoryEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef DIRECTORYENTITY_HPP
#define DIRECTORYENTITY_HPP

#include <string>
#include <map>
#include "Entity.hpp"

class DirectoryEntity : public Entity {
public:
    DirectoryEntity( ) { }

    virtual ~DirectoryEntity( );

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

private:
    DirectoryEntity( const std::map< std::string, Entity * > &existing );
    std::map< std::string, Entity * > value;
};

#endif
