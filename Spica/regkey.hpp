/*! \file    regkey.hpp
    \brief   Interface to a simple registry key object.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This class provides a rather simple-minded interface to the registry. Construction opens the key
specified to the constructor while destruction closes the key. When a key is open, new values
can be added and existing values can be queried. This class currently only supports string and
DWORD values.

The constructor takes two parameters. The first is intended to be one of the special symbols
HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. The second parameter is the path to the desired key
*without* a leading backslash. The constructor will create the key (and required parent keys) if
it does not exist.

Error reporting is minimal (non-existant). This is an important area where this class could be
improved.
*/

#ifndef REGKEY_HPP
#define REGKEY_HPP

#include <string>
#include <windows.h>

namespace common {
    namespace Win32 {

        class RegistryKey {
        private:
            HKEY  key_handle; // Holds the handle to the key.
            bool  error;      // =true if we couldn't create (or open) the key.

        public:
            RegistryKey(HKEY topLevel_key, const char *key_name);
           ~RegistryKey();

            // This function adds the (name, value) pair to the key. Notice that this function
            // only allows string values.
            //
            void set_value( const char *name, const std::string &value );
            
            // This function retrieves the value associated with the given name. It returns true
            // if the name was found and false otherwise.
            //
            bool get_value( const char *name, std::string &value );

            // Same as above except for DWORD type entries.
            void set_value( const char *name, DWORD  value );
            bool get_value( const char *name, DWORD &value );
        };
    }
}

#endif
