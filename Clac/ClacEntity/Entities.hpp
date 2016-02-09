/*! \file    Entities.cpp
 *  \brief   Master header that draws in all Entity headers.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * This header is useful because each of the *Entity.cpp files needs to include all Entity
 * headers. Why is that? It's because of the conversion methods in every Entity class (there are
 * methods that target each Entity type).
 *
 * It might be possible to put the conversion methods of all Entity types into a single file.
 * That would reduce the number of "alien" Entity headers that would need to be #included into
 * each cpp file. On the other hand there is already a need for some alien headers (often
 * FloatEntity and ComplexEntity) just to implement the required Entity operations. There may
 * also be an issue with g++'s handling of vtable generation. Look up information on "vague
 * linkage" in the g++/gcc documentation for more information.
 */

#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include "BinaryEntity.hpp"
#include "ComplexEntity.hpp"
#include "DirectoryEntity.hpp"
#include "FloatEntity.hpp"
#include "IntegerEntity.hpp"
#include "LabeledEntity.hpp"
#include "ListEntity.hpp"
#include "MatrixEntity.hpp"
#include "ProgramEntity.hpp"
#include "RationalEntity.hpp"
#include "StringEntity.hpp"
#include "VectorEntity.hpp"

#endif
