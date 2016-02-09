/*! \file    StringEntity.hpp
    \brief   Interface to the clac numeric type StringEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef STRINGENTITY_HPP
#define STRINGENTITY_HPP

#include <string>
#include "Entity.hpp"

class StringEntity : public Entity {
public:
    StringEntity( ) { }
    StringEntity( const std::string &incoming ) : value( incoming ) { }

    // It might be nice to do without this at some point.
    std::string get_value( )
      { return value; }

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

    // Conversion functions.
    virtual Entity *to_string( ) const;

    // Binary operations.
    virtual Entity *plus( const Entity * ) const;

private:
    std::string value;
};

#endif
