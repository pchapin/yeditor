/*! \file    ProgramEntity.hpp
    \brief   Interface to the clac numeric type ProgramEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef PROGRAMENTITY_HPP
#define PROGRAMENTITY_HPP

#include <string>
#include "Entity.hpp"

class ProgramEntity : public Entity {
public:
    ProgramEntity( const std::string &program ) : value( program )
      { }

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

private:
    std::string value;
};

#endif
