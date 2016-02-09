/*! \file    spica.hpp
    \brief   List template that preallocates memory.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

To simplify use of the Spica library this header is provided. It includes all other Spica
headers. Simply include spica.h in your project to access any spica related component.
Precompilation of this header is recommended.
*/

#ifndef SPICA_HPP
#define SPICA_HPP

/*! \namespace spica
 * \brief Name space enclosing the Spica library.
 *
 * This name space contains all components in the Spica library. No third party or external
 * library components are included in the spica name space.
 */

#include <BinomialHeap.hpp>
#include <BoundedList.hpp>
#include <Graph.hpp>
#include <sorters.hpp>

#endif

