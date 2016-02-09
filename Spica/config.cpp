/*! \file    config.cpp
 *  \brief   Implementation of a simple configuration management module.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <fstream>
#include <map>
#include <regex>
#include <string>

#include "config.hpp"

using namespace std;

namespace spica {

    map<string, string> read_configuration( const string &file_name )
    {
        map<string, string> configuration;
        ifstream            input_file( file_name.c_str( ) );
        regex               pattern( R"(\s*(.*)\s*=\s*(.*)\s*)" );
        string              line;

        // If the file opened correctly...
        if( input_file ) {
            while( getline( input_file, line ) ) {
                smatch matches;
                if( regex_match( line, matches, pattern ) ) {
                    configuration[ matches[1] ] = matches[2];
                }
            }
        }

        return configuration;
    }

}
