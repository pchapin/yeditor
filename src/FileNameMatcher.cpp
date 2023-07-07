/*! \file    FileNameMatcher.cpp
 *  \brief   Implementatin of file name matching functions
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the implementation of a class which assists in the problem of matching
 * filenames.
*/

#include "environ.hpp"

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#if eOPSYS == ePOSIX
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "FileNameMatcher.hpp"

// The Unix version is very different than the other versions.
#if eOPSYS == ePOSIX

//! Checks a string for a wildcard characters.
/*!
 * Returns true if wild card characters are in the string, otherwise returns false. This class
 * handles strings without wildcard characters using stat(). That is faster.
 */
static bool wild( const char *string )
{
    while( *string ) {
        if( *string == '*' || *string == '?' || *string == '[' ) return true;
        string++;
    }
    return false;
}


FileNameMatcher::FileNameMatcher( )
{
    full_name        = "*";
    search_attribute = NORMAL;
    first_match      = true;
    done             = false;
}


FileNameMatcher::~FileNameMatcher( )
{
    // If we were in the middle of a scan but didn't finish, then clean up.
    if( !done ) globfree( &glob_data );
}


bool FileNameMatcher::set_name( const char *wild_name, AttribFlags file_attribute )
{
    search_attribute = file_attribute;
    full_name        = wild_name;
    first_match      = true;
    done             = false;

    return true;
}


char *FileNameMatcher::next()
{
    char *return_value = buffer;
    
    // If we've used up all matches already, return NULL.
    if( done ) return 0;

    // The first attempt to match a name is special.
    if( first_match ) {

        // If there are wildcard characters in the name, then use glob(). Note that this use of
        // glob() will probably have problems if we encounter a device file, a socket, or a pipe
        // (or some other odd thing in the directory. This should probably be handled below
        // somehow.
        //
        if( wild( full_name ) ) {
            glob( full_name, GLOB_MARK, 0, &glob_data );
            match_index = 0;
            first_match = false;
        }

        // Otherwise, use stat().
        else {
            struct stat file_info;

            // If the stat() fails, the file's not there.
            if( stat( full_name, &file_info ) != 0 ) {
                done = true;
                return 0;
            }

            // If the file's not a regular file, it's "not there."
            if( !S_ISREG( file_info.st_dev ) ) {
                done = true;
                return 0;
            }

            // Otherwise, we like the file. Copy it's name into the buffer in case the user
            // wants to find out something else about it.
            //
            strcpy( buffer, full_name );
            done = true;
            return buffer;
        }
    }

    // Try to return the next name in the set of matched names.
    while( match_index < static_cast< int >( glob_data.gl_pathc ) ) {

        strcpy( buffer, glob_data.gl_pathv[match_index++] );

        // Directories will have a trailing slash. Forget about them.
        char *end = strchr( buffer, '\0' ); end--;
        if( *end != '/' ) break;
    }

    // If we ran out of globbed data, then we are done.
    if( match_index >= static_cast< int >( glob_data.gl_pathc ) ) {
        done         = true;
        return_value = 0;
        globfree( &glob_data );
    }

    return return_value;
}


time_t FileNameMatcher::modify_time()
{
    // Assume that buffer contains a file name.

    struct stat file_info;

    // If the stat() fails, that means the file got deleted between the time buffer was filled
    // and this call to modify_time(). In effect, the file has been modified. However, I'm not
    // sure how best to handle this case here.
    //
    if( stat( buffer, &file_info ) != 0 ) {
        return 0;
    }

    return file_info.st_mtime;
}


#elif eOPSYS == eWIN32

FileNameMatcher::FileNameMatcher( )
{
    search_handle    = INVALID_HANDLE_VALUE;
    full_name        = "*.*";
    search_attribute = NORMAL;
    start_of_name    = buffer;
    first_match      = true;
    done             = false;
}


bool FileNameMatcher::set_name( const char *wild_name, AttribFlags file_attribute )
{
    const char *past_path;         // Ends up pointing just past pathname part.
    const char *path_pntr;         // Scans down pathname part; copying to buf.
          char *buffer_pntr;       // Points into local buffer.
    ptrdiff_t   path_length;       // Number of bytes in pathname (could be 0).
    bool        return_value=true; // =false if pathname is too long.

    search_attribute = file_attribute;
    
    // Adjust past_path to point at first character of name portion.
    past_path = wild_name;
    while( *past_path++ ) /* null */ ;
    for( past_path--;
         past_path >= wild_name  &&  *past_path != ':'  &&  *past_path != '\\';
         past_path-- ) /* null */ ;
    past_path++;

    // Calculate the length of the path name (could be zero).
    path_length = past_path - wild_name;

    // Return error code if path is too long. Act as if there will be no matches.
    if( path_length + 12 > FNMATCHER_BUF_SIZE ) {
        return_value = false;
        done = true;
    }
    else {
        // Save pointer to full name for use by findfirst().
        full_name = wild_name;

        // Copy just the path part of the wild name into the local buffer.
        for( path_pntr = wild_name, buffer_pntr = buffer; path_pntr < past_path; /* Null */ )
            *buffer_pntr++ = *path_pntr++;

        // Mark place in local buffer where simple names are to go.
        start_of_name = buffer_pntr;

        // Indicate that findfirst() will need to be used.
        first_match = true;
        done        = false;
    }
    return return_value;
}


char *FileNameMatcher::next( )
{
    char *return_value = buffer;

    // If we've used up all matches already, return NULL.
    if( done ) return NULL;

    // Use FindFirstFile() if this is the first attempt.
    if( first_match ) {

        // If FindFirstFile() fails, there were no matches.
        if( ( search_handle = FindFirstFile( full_name, &file_data ) ) == INVALID_HANDLE_VALUE ) {
            done          = true;
            return_value = NULL;
        }
        else {
            // Otherwise there was a match. Copy the name into the local buffer.
            strcpy( start_of_name, file_data.cFileName );
            first_match = false;
        }
    }

    // Use FindNextFile() if this is not the first attempt.
    else {
        // If FindNextFile() fails, there are no more matches.
        if( FindNextFile( search_handle, &file_data ) == FALSE ) {
            done         = true;
            return_value = NULL;
        }
        else {
            // Otherwise, there was a match. Copy the name into the local buffer.
            strcpy( start_of_name, file_data.cFileName );
        }
    }

    return return_value;
}

#else
  // We have DOS, OS/2.

FileNameMatcher::FileNameMatcher( )
{
    full_name        = "*.*";
    search_attribute = NORMAL;
    start_of_name    = buffer;
    first_match      = true;
    done             = false;
}


bool FileNameMatcher::set_name( const char *wild_name, AttribFlags file_attribute )
{
    const char *past_path;         // Ends up pointing just past pathname part.
    const char *path_pntr;         // Scans down pathname part; copying to buf.
          char *buffer_pntr;       // Points into local buffer.
    ptrdiff_t   path_length;       // Number of bytes in pathname (could be 0).
    bool        return_value=true; // =false if pathname is too long.

    search_attribute = file_attribute;

    // Adjust past_path to point at first character of name portion.
    past_path = wild_name;
    while( *past_path++ ) /* null */ ;
    for( past_path--;
         past_path >= wild_name  &&  *past_path != ':'  &&  *past_path != '\\';
         past_path-- ) /* null */ ;
    past_path++;

    // Calculate the length of the path name (could be zero).
    path_length = past_path - wild_name;

    // Return error code if path is too long. Act as if there will be no matches.
    if( path_length + 12 > FNMATCHER_BUF_SIZE ) {
        return_value = false;
        done = true;
    }

    else {

      // Save pointer to full name for use by findfirst().
      full_name = wild_name;

      // Copy just the path part of the wild name into the local buffer.
      for( path_pntr = wild_name, buffer_pntr = buffer; path_pntr < past_path; /* Null */ )
          *buffer_pntr++ = *path_pntr++;

      // Mark place in local buffer where simple names are to go.
      start_of_name = buffer_pntr;

      // Indicate that findfirst() will need to be used.
      first_match = true;
      done        = false;
    }

    return return_value;
}


char *FileNameMatcher::next( )
{
    char *return_value = buffer;

    // If we've used up all matches already, return NULL.
    if( done ) return NULL;

    // Use _dos_findfirst() if this is the first attempt.
    if( first_match ) {
        // If _dos_findfirst() fails, there were no matches.
        if( _dos_findfirst( full_name, (int)search_attribute, &file_data ) != 0 ) {
            done          = true;
            return_value = NULL;
        }
        else {
            // Otherwise there was a match. Copy the name into the local buffer.
            strcpy( start_of_name, file_data.name );
            first_match = false;
        }
    }

    // Use _dos_findnext() if this is not the first attempt.
    else {
        // If _dos_findnext() fails, there are no more matches.
        if( _dos_findnext( &file_data ) != 0 ) {
            done         = true;
            return_value = NULL;
        }
        else {
            // Otherwise, there was a match. Copy the name into the local buffer.
            strcpy( start_of_name, file_data.name );
        }
    }

    return return_value;
}

#endif
