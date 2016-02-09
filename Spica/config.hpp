/*! \file    config.hpp
 *  \brief   Interface to a simple configuration management module.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * This module simplifies the task of fetching and using configurable parameters.
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>

namespace spica {

    //! Return a map of (name, value) pairs.
    /*!
     * Reads the specified file looking for lines of the form "name = value." Returns a map
     * containing all (name, value) pairs found.
     */
    std::map<std::string, std::string> read_configuration( const std::string &file_name );

}

#endif
