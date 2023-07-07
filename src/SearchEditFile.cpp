/*! \file    SearchEditFile.cpp
 *  \brief   Implementation of class SearchEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstring>

#include "EditBuffer.hpp"
#include "SearchEditFile.hpp"


/*!
 * Search from the current point forward in the file's data looking for the first occurrence of
 * search_string. If the current point is already on the start of a valid copy of the search
 * string, the search stops at once and the current point is not moved.
 *
 *
 * \param search_string The string being searched for. The string must be contained entirely on
 * a single line to be considered found on that line.
 * \return True if an occurrence of the search string is found, otherwise return false. If an
 * occurrence is found the current point is moved to the start of that occurrence.
 */
bool SearchEditFile::simple_search( const char *search_string )
{
    const char *found_string;    // Points at copy of string inside a line.
    bool        found = false;   // =true if the string is found.

    // Check the current line (if there is one).
    file_data.jump_to( current_point.cursor_line( ) );
    if( file_data.get( ) != NULL ) {

        // If the current point on the text of a line, check the partial line.
        if( current_point.cursor_column( ) < file_data.get( )->length( ) ) {
            std::string line = file_data.get( )->to_string( );

            // If we've found it already, jump to it and set found.
            if( ( found_string = std::strstr( line.c_str( ) + current_point.cursor_column( ),
                    search_string ) ) != NULL ) {
                found = true;
                current_point.jump_to_column( found_string - line.c_str( ) );
            }
        }
    }

    // Check all other lines in the object if we haven't already found it.
    if( found == false ) {

        for( file_data.next( ); file_data.get( ) != NULL; file_data.next( ) ) {
            std::string line = file_data.get( )->to_string( );
            if( ( found_string = std::strstr( line.c_str( ), search_string ) ) != NULL) {
                found = true;
                current_point.jump_to_line( file_data.current_index( ) );
                current_point.jump_to_column( found_string - line.c_str( ) );
                break;
            }
        }
    }
    
    return found;
}
