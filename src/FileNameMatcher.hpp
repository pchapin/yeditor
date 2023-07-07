/*! \file    FileNameMatcher.hpp
 *  \brief   Interface to file name matching functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef FILENAMEMATCHER_HPP
#define FILENAMEMATCHER_HPP

#include "environ.hpp"
#include <ctime>

// There are different definitions of this class depending on the OS.

#if   eOPSYS == ePOSIX
#include <glob.h>

#elif eOPSYS == eWIN32
#include <windows.h>
#undef min
#undef max

#else // DOS, OS/2
#include <dos.h>
#endif

#if eOPSYS == ePOSIX

// FNMATCHER_BUF_SIZE is the longest possible pathname.
#define FNMATCHER_BUF_SIZE  1024

class FileNameMatcher {

public:
    enum AttribFlags {
        NORMAL   // For now, Unix has only normal files. :-)
    };

private:
    glob_t glob_data;               // Results of glob() function.

    AttribFlags search_attribute;   // Desired file attribute.

    const char *full_name;          // Points at path provided by user.
          char  buffer[FNMATCHER_BUF_SIZE+1];// Holds matched names.
          int   match_index;        // Index into the array of matches.
    bool        first_match;        // =true when glob needs to be used.
    bool        done;               // =true when no more matches.

public:
    FileNameMatcher( );
   ~FileNameMatcher( );

    //! Sets new path and checks path length.
    bool set_name( const char *wild_name, AttribFlags some_attribute = NORMAL );

    //! Returns pointer to next matching name. Full path required.
    char *next( );

    // Return information on file after successful call to next().
    int           actual_attribute( )  { return (int)NORMAL; }
    time_t        modify_time( );
    unsigned long size( );
    char         *plain_name( );
};

#elif eOPSYS == eWIN32

// FNMATCHER_BUF_SIZE is the longest possible pathname.
#define FNMATCHER_BUF_SIZE  1024

class FileNameMatcher {
    
public:
    enum AttribFlags {
        NORMAL = 0x00, READ_ONLY = 0x01, HIDDEN  = 0x02, SYSTEM    = 0x04,
        LABEL  = 0x08, DIR       = 0x10, ARCHIVE = 0x20, THE_WORKS = 0x16
    };

private:
    WIN32_FIND_DATA file_data;        // Specifics on most recent file.
    HANDLE          search_handle;

    AttribFlags search_attribute;     // Desired file attribute.

    const char *full_name;            // Points at path provided by user.
          char  buffer[FNMATCHER_BUF_SIZE+1];// Holds matched names.
          char *start_of_name;        // Start of plain name part in buffer.
    bool        first_match;          // =true when findfirst needs to be used.
    bool        done;                 // =true when no more matches.

public:
    FileNameMatcher( );              // Initial path = "*.*".
   ~FileNameMatcher( )
        { if (search_handle != INVALID_HANDLE_VALUE) FindClose(search_handle); }

    //! Sets new path and checks path length.
    bool set_name( const char *wild_name, AttribFlags some_attribute = NORMAL );

    //! Returns pointer to next matching name. Full path required.
    char *next( );

    //! Return information on file after successful call to next().
    int           actual_attribute( ) { return (int)           file_data.dwFileAttributes; }
    unsigned      modify_time( )      { return                 file_data.ftLastWriteTime.dwHighDateTime;}
    unsigned long size( )             { return (unsigned long) file_data.nFileSizeLow;   }
    char         *plain_name( )       { return                 file_data.cFileName;   }
};

#else // DOS, OS/2

// FNMATCHER_BUF_SIZE is the longest possible pathname.
#define FNMATCHER_BUF_SIZE  1024

class FileNameMatcher {
    
public:
    enum AttribFlags {
        NORMAL = 0x00, READ_ONLY = 0x01, HIDDEN  = 0x02, SYSTEM    = 0x04,
        LABEL  = 0x08, DIR       = 0x10, ARCHIVE = 0x20, THE_WORKS = 0x16
    };

private:
    struct find_t file_data;        // Specifics on most recent file.

    AttribFlags search_attribute;   // Desired file attribute.

    const char *full_name;          // Points at path provided by user.
          char  buffer[FNMATCHER_BUF_SIZE+1];// Holds matched names.
          char *start_of_name;      // Start of plain name part in buffer.
    bool        first_match;        // =true when findfirst needs to be used.
    bool        done;               // =true when no more matches.
    
public:
    FileNameMatcher( );                // Initial path = "*.*".
    
    //! Sets new path and checks path length.
    bool set_name( const char *wild_name, AttribFlags some_attribute = NORMAL );

    //! Returns pointer to next matching name. Full path required.
    char *next( );

    //! Return information on file after successful call to next().
    int           actual_attribute( ) { return (int)           file_data.attrib; }
    unsigned      modify_time( )      { return                 file_data.wr_time;}
    unsigned long size( )             { return (unsigned long) file_data.size;   }
    char         *plain_name( )       { return                 file_data.name;   }
};

#endif

#endif
