/*! \file    MatrixEntity.hpp
    \brief   Interface to the clac numeric type MatrixEntity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef MATRIXENTITY_HPP
#define MATRIXENTITY_HPP

#include <string>
#include <vector>
#include "Entity.hpp"

class MatrixEntity : public Entity {
public:
    ~MatrixEntity( );

    virtual EntityType my_type( ) const;
    virtual std::string display( ) const;
    virtual Entity *duplicate( ) const;

private:

    // It would probably make sense to create a separate matrix type with overloaded operators,
    // etc (as was done for rational and integer, and then just make the private part of a
    // matrix an instance of that type.
    // 
    typedef std::vector< Entity * > row_type;
    std::vector< row_type > value;
};

#endif
