/*! \file    string_utilities.hpp
    \brief   Declarations of supplementary (helper) string functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef STRING_UTILITIES_HPP
#define STRING_UTILITIES_HPP

#include <string>

namespace spica {

    void create_long_string(
              std::string           &result,
        const std::string           &search,
              std::string::size_type size,
              int                    range = 26,
              bool                   verbose = false
    );

    void display_partial_matches( const std::string &result, const std::string &search );

}

#endif
