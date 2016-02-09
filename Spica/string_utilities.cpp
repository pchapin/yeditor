/*! \file    string_utilities.cpp
    \brief   Implementation of supplementary (helper) string functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "string_utilities.hpp"

namespace spica {

    //! Generate a string that contains no matching substring to the given string.
    /*!
     * This function is useful for creating long strings with no match to test the performance
     * of various string matching algorithms.
     *
     * The range of characters used start with 'a' and contain as many characters as specified
     * (not more than 26). The search string should only contain characters in the specified
     * range.
     *
     * This function makes use of the standard random number generator, but it does not seed it.
     * If the caller wants to seed the generator first, that is up to him/her.
     *
     * \param result The string to receive the long string with no matches.
     * \param search The string that will be used to search result.
     * \param size The number of characters desired in result.
     * \param range The number of different characters to use in result.
     * \param verbose If true, this function displays progress info  as it works.
     */
    void create_long_string(
                std::string           &result,
          const std::string           &search,
                std::string::size_type size,
                int                    range,
                bool                   verbose
    )
    {
        // Blow away anything already in the result string. Then allocate the requested space
        // for it. This might throw an exception if there isn't enough memory.
        //
        result.erase();
        result.resize( size );

        // Fill the string with random characters in the specified range.
        if( verbose ) std::cout << "Building string...\n";
        for( std::string::size_type index = 0; index < size; ++index ) {
            result[index] = static_cast< char >( ( std::rand( ) % range ) + 'a' );
        }

        // Put a null character in the last position so std::strstr() will terminate properly.
        result[size - 1] = '\0';

        // Now keep looping until std::string::find() returns failure. This might take a while
        // depending on the search string and the range of characters being used. Also
        // std::string::find() is probably O(N*M).
        //
        // Can you prove that this loop will always terminate eventually?
        //
        std::string::size_type hit;
        if( verbose ) std::cout << "Checking string...\n";
        while( ( hit = result.find( search.c_str( ) ) ) != std::string::npos ) {
            std::string::size_type last_match = hit + search.size( ) - 1;
            if( verbose ) std::cout << "Modifying index: " << last_match << "\n";
            result[last_match] =
                ( result[last_match] == 'a' + range - 1 ) ? 'a' : result[last_match] + 1;
        }
        if( verbose ) std::cout << "\nDone!\n";
    }


    //
    // The following function searches for search in result and reports
    // information about all the partial matches that occur.
    //
    void display_partial_matches( const std::string &result, const std::string &search )
    {
        std::string my_search( search );

        // Output summary information.
        std::cout << "Size of string being searched: " << result.size( ) << "\n";
        std::cout << "Size of search string: " << search.size( ) << "\n";

        // Keep looping while the search string is non-zero sized.
        while( my_search.size( ) != 0 ) {
            std::string::size_type match_count = 0;
            std::string::size_type index = 0;

            // Keep looping as long as there is a match.
            std::string::size_type hit;
            while( ( hit = result.find( my_search, index ) ) != std::string::npos ) {
                match_count++;
                index = hit + 1;
            }

            // Output results.
            std::cout << "Match depth: " << std::setw( 2 ) << my_search.size( )
                      << ", Search = " << my_search
                      << ", Hits = "   << match_count << "\n";

            my_search.erase( my_search.size( ) - 1, 1 );
        }
    }

}
