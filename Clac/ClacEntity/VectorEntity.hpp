/*! \file    VectorEntity.hpp
    \brief   Interface to the clac numeric type VectorEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef VECTORENTITY_HPP
#define VECTORENTITY_HPP

#include <string>
#include <vector>
#include "Entity.hpp"

class VectorEntity : public Entity {
public:
    virtual ~VectorEntity( );
    
    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

private:
    std::vector< Entity * > value;
};

#endif
