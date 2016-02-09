/*! \file    LabeledEntity.hpp
    \brief   Interface to the clac numeric type LabeledEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef LABELEDENTITY_HPP
#define LABELEDENTITY_HPP

#include <string>
#include "Entity.hpp"

class LabeledEntity : public Entity {
public:
    LabeledEntity( Entity *thing, const std::string &name ) :
      object( thing ), label( name ) {}

    virtual ~LabeledEntity( ) { delete object; }

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

private:
    Entity     *object;
    std::string label;
};

#endif
