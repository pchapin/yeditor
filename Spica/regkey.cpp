/*! \file    regkey.cpp
    \brief   Implementation of a simple registry key object.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <string>
#include <windows.h>
#include "regkey.hpp"

namespace common {
    namespace Win32 {

        //
        // RegistryKey::RegistryKey(HKEY, const char *)
        //
        // The following function opens (or creates) the specified registry key. It trys to set
        // as many things as possible to default values in order to make life simple for the
        // caller.
        //
        RegistryKey::RegistryKey(HKEY topLevel_key, const char *key_name)
        {
            error = true;

            DWORD disposition;
            LONG  open_result =
                RegCreateKeyEx(
                    topLevel_key,            // One of the special top level key names.
                    key_name,                // Name of subkey to open or create.
                    0,                       // Reserved. Must be zero.
                    "",                      // Name of key class. Ignored if key exists.
                    REG_OPTION_NON_VOLATILE, // Save this key between reboots.
                    KEY_ALL_ACCESS,          // Want full access.
                    0,                       // Security attributes.
                   &key_handle,              // Resulting key handle.
                   &disposition              // Resulting flag (open or create).
                );

            if (open_result == ERROR_SUCCESS) error = false;
            // If an error occurs the other calls will silently not work. This should be fixed
            // eventually. It would be better to have some sort of error reporting. Notice that
            // the registry functions don't seem to work with GetLastError().
        }


        //
        // RegistryKey::~RegistryKey()
        //
        // The destructor closes the key if it was opened okay. This is important for things to
        // work right with exceptions around.
        //
        RegistryKey::~RegistryKey()
        {
            if (!error) RegCloseKey(key_handle);
        }


        //
        // RegistryKey::set_value(const char *, const std::string &)
        //
        // This function adds a value to the key. The value has a name and an associated value.
        // Yes... that is confusing. Send your complaints to Microsoft. This version only
        // supports string values.
        //
        void RegistryKey::set_value( const char *name, const std::string &value )
        {
            if (!error) {
                RegSetValueEx(
                    key_handle,               // Handle of the key.
                    name,                     // Name of the value.
                    0,                        // Reserved. Must be zero.
                    REG_SZ,                   // This value has type "string."
                    reinterpret_cast<CONST BYTE *>(value.c_str()),
                    value.size()+1            // Number of bytes.
                ); 
            }
        }


        //
        // RegistryKey::get_value
        //
        // This function retrieves the value associated with the given name. It returns true if
        // the name was found and false otherwise.
        //
        bool RegistryKey::get_value(const char *name, std::string &value)
        {
            // First let's be sure the key was opened okay. If not, we can't get a value.
            // 
            if (error) return false;

            char  buffer[128+1];
            DWORD type;
            DWORD buffer_size(128+1);

            // Call the API function. Notice that we don't care about the type. We assume that
            // it is a string. This is probably a bad idea, but it's quick and dirty so we'll do
            // it!
            //
            if (RegQueryValueEx(
                     key_handle,            // Handle of key to query.
                     name,                  // Name of the value.
                     0,                     // Reserved. Must be NULL.
                    &type,                  // The value's type.
                     reinterpret_cast<LPBYTE>(buffer),
                    &buffer_size            // Size of buffer. Modified by call.
                ) == ERROR_SUCCESS) {
                value = buffer;
                return true;
            }

            return false;
        }

        //
        // RegistryKey::set_value
        //
        // This function adds a value to the key. This version only supports DWORD values.
        //
        void RegistryKey::set_value(const char *name, DWORD value)
        {
            if (!error) {
                RegSetValueEx(
                    key_handle,               // Handle of the key.
                    name,                     // Name of the value.
                    0,                        // Reserved. Must be zero.
                    REG_DWORD,                // This value has type "DWORD."
                    reinterpret_cast<CONST BYTE *>(&value),
                    sizeof(DWORD)             // Number of bytes.
                );
            }
        }

        //
        // RegistryKey::get_value
        //
        // This function retrieves the value associated with the given name. It returns true if
        // the name was found and false otherwise.
        //
        bool RegistryKey::get_value(const char *name, DWORD &value)
        {
            // First let's be sure the key was opened okay. If not, we didn't
            // get a value.
            // 
            if (error) return false;

            DWORD buffer;
            DWORD type;
            DWORD buffer_size(sizeof(DWORD));

            // Call the API function. Notice that we don't care about the type. We assume that
            // it is a DWORD. This is probably a bad idea, but it's quick and dirty so we'll do
            // it!
            //
            if (RegQueryValueEx(
                    key_handle,            // Handle of key to query.
                    name,                  // Name of value.
                    0,                     // Reserved. Must be NULL.
                   &type,                  // Value's type.
                    reinterpret_cast<LPBYTE>(&buffer),
                   &buffer_size            // Size of buffer.
                ) == ERROR_SUCCESS) {
                value = buffer;
                return true;
            }
            return false;
        }
    }
}
