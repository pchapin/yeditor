/*! \file    ListEntity.hpp
    \brief   Interface to the clac numeric type ListEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef LISTENTITY_HPP
#define LISTENTITY_HPP

#include <list>
#include <string>
#include "Entity.hpp"

class ListEntity : public Entity {
public:
    ListEntity( ) { }
    ListEntity( const std::list< Entity * > &incoming ) : value( incoming ) { }
   ~ListEntity( );

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

    virtual Entity *plus( const Entity * ) const;

private:
    std::list< Entity * > value;
};

#endif
