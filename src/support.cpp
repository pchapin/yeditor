/*! \file    support.cpp
 *  \brief   Implementation of various supporting functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include "environ.hpp"

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "FileList.hpp"
#include "FileNameMatcher.hpp"
#include "global.hpp"
#include "Timer.hpp"
#include "MessageWindow.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "YEditFile.hpp"


//! Simple adjustments to the __DATE__ macro.
/*!
 * This function adjusts the format of __DATE__. It puts a comma after the day of the month and
 * purges leading zeros or spaces from the day of the month.
 *
 * \param raw_date A pointer to a null terminated string in the format of the standard
 * __DATE__ macro.
 * \return A pointer to the adjusted date string. The returned pointer is in statically
 * allocated storage. This function is not thread-safe!
 */
char *adjust_date( const char * const raw_date )
{
    static char  buffer[13];
           char *buffer_ptr;

    // Make a working copy of the date as from the ANSI __DATE__ macro.
    std::strcpy( buffer, raw_date );

    // Open up space for the comma.
    for( buffer_ptr = std::strchr( buffer,'\0' ); buffer_ptr >= &buffer[6]; buffer_ptr-- ) {
        *( buffer_ptr + 1 ) = *buffer_ptr;
    }

    // Put the comma in.
    buffer[6] = ',';

    // If this is a date from 1 to 9, close up the extra space.
    if( buffer[4] == '0' || buffer[4] == ' ' ) {
        for( buffer_ptr = &buffer[4]; *buffer_ptr != '\0'; buffer_ptr++ ) {
            *buffer_ptr = *( buffer_ptr + 1 );
        }
    }

    // Return the result.
    return buffer;
}


unsigned word_right( const EditBuffer &line, unsigned column )
{
    bool flag = std::isalnum( line[column] ) ? true : false;

    // If cursor on a character, skip all of them.
    while( flag == true ) {
        column++;
        if( !std::isalnum( line[column] ) ) flag = false;
    }

    // Skip all non-alphanumeric characters.
    while( flag == false  &&  line[column] ) {
        column++;
        if( std::isalnum( line[column] ) ) flag = true;
    }
    return column;
}


unsigned word_left( const EditBuffer &line, unsigned column )
{
    // Are we on an alphanumeric character?
    bool flag = std::isalnum( line[column] ) ? true : false;

    // If so, back the cursor up (if there's space).
    if( flag == true  && column != 0 ) {
        column--;
        flag = std::isalnum( line[column] ) ? true : false;
    }

    // Skip over any non-alphanumeric characters.
    while( flag == false  &&  column != 0 ) {
        column--;
        if( std::isalnum( line[column] ) ) flag = true;
    }

    // Skip over alphanumeric characters (to just past beginning of word).
    while( flag == true  &&  column != 0 ) {
        column--;
        if( !std::isalnum( line[column] ) ) flag = false;
    }

    // Adjust back to beginning of word (word at start of line special case).
    if( column != 0 ) column++;

    return column;
}


//! Loads the named file.
/*!
 * This function does the grunt work of forcing Y to load the named file (no wildcards), and
 * position the cursor at the desired location. This function is used by load_files().
 *
 * \param name The name of the file to load.
 * \param line_number The initial line number of the current point after loading.
 * \param column_number The initial column number of the current point after loading.
 */
bool load_file( const char *name, long line_number, int column_number )
{
    bool return_value;

    if( ( return_value = FileList::lookup( name ) ) == false ) {
        return_value = FileList::new_file( name );
    }

    if( return_value == true ) {
        if( line_number != -1L )
            FileList::active_file( ).CP( ).jump_to_line( line_number );
        if( column_number != -1L )
            FileList::active_file( ).CP( ).jump_to_column( column_number );
    }

    return return_value;
}


bool load_files( const char **argv )
{
    std::string base_name;     // Contains the base part of a saved name.
    std::string workspace;     // Used to hold expanded abbreviations.
    bool base_valid = false;   // =true if base_name contains valid material.
    int  file_count = 0;       // Number of files loaded.
    bool First_File = true;    // =false when at least one file has been loaded.
    YEditFile *leading_file;   // Reference to the first file loaded.

    // Tell user if there's a problem.
    if( argv[1] == NULL ) {
        error_message( "You must specify at least one filename" );
        return false;
    }

    // Default value for initial position = no change.
    long line_number   = -1;
    int  column_number = -1;

    // For each parameter.
    for( argv++; *argv; argv++ ) {

        // If we're looking at a switch (accept either '-' or '/')...
        if( **argv == '-'  ||  **argv == '/' ) {
            switch( *++*argv ) {

                // Set initial line number. Note that user's line number 1 based.
            case 'l':
            case 'L':
                line_number = std::atol( ++*argv ) - 1L;
                break;

                // Set initial column number. Note that user's column number 1 based.
            case 'c':
            case 'C':
                column_number = std::atoi( ++*argv ) - 1;
                break;

            // If the user sets restricted mode anywhere on the command line, set it globally.
            // The editor is either restricted or not!
            case 'r':
            case 'R':
                restricted_mode = true;
                break;

                // Print message and wait for acknowledgment.
            default:
                warning_message( "Unrecognized switch (%c) ignored", **argv );
                break;

            } // End of switch statement which handles each switch type.
        }   // End of switch handling if...

        // Otherwise it was no switch, it must be a file name (or wildcard spec).
        else {
            FileNameMatcher wild_match;
            char *file_name;

            // If the current argument is an abbreviation, try expanding it.
            if( **argv == '.' && base_valid ) {
                workspace = base_name;
                workspace.append( *argv );
                *argv = workspace.c_str( );
            }

            // Otherwise, save the base part of the current name.
            else {
                const char *end_pointer = std::strrchr( *argv, '.' );
                if( end_pointer != NULL ) {
                    base_name.assign( *argv, end_pointer - *argv );
                    base_valid   = true;
                }
            }

            wild_match.set_name( *argv );

            // If no files match the spec, assume the spec is the name of a new file.
            if( ( file_name = wild_match.next( ) ) == NULL ) {
                if( std::strchr( *argv, '*' ) != NULL ||
                    std::strchr( *argv, '?' ) != NULL ) {
                    warning_message( "No files match %s", *argv );
                }
                else {
                    load_file( *argv, line_number, column_number );
                    file_count++;
                    if( First_File ) {
                        First_File = false;
                        leading_file = &FileList::active_file( );
                    }
                }
            }

            else {

                // Loop over all names which match the wildcard spec and load them.
                while( file_name != NULL ) {
                    load_file( file_name, line_number, column_number );
                    file_count++;
                    if( First_File ) {
                        First_File = false;
                        leading_file = &FileList::active_file( );
                    }
                    file_name = wild_match.next( );
                }
            }

        } // End of if...else... that determines if we are looking at a switch.
    }   // End of loop that passes over all command line arguments.

    if( file_count == 0 ) {
        error_message( "No files loaded" );
        return false;
    }

    FileList::lookup( leading_file->name( ) );
    return true;
}


void insert_AWK( std::string &working_buffer )
{
    std::string::size_type found;
    std::string::size_type search_start = 0;

    // Handle all occurrences of .awk in the working buffer.
    while( ( found = working_buffer.find( ".awk", search_start ) ) != std::string::npos ||
           ( found = working_buffer.find( ".AWK", search_start ) ) != std::string::npos ) {

        // Remember how far we are into the string for next search.
        search_start = found + 4;

        // Back up pointer to beginning of word before .awk (skip letters, digits, and '.'
        while( found > 0 &&
               ( std::isalnum( working_buffer[found] ) || working_buffer[found] == '.' ) )
            found--;

        // Bump pointer back to letter unless loop broke because we came to start of buffer.
        if( !( std::isalnum( working_buffer[found] ) || working_buffer[found] == '.' ) ) found++;

        // Insert the text into the string.
        working_buffer.insert( found, "awk -f " );

        // Advance offset for next search past the inserted text.
        search_start += 7;
    }
}


//! Compare strings in a case insensitive manner.
/*!
 * The function _stricmp is available in the Open Watcom library, but not in other libraries. To
 * simplify cross compiler portability I am implementing a version here to be used with all
 * compilers.
 */
int my_stricmp( const char *s1, const char *s2 )
{
    while( *s1 && *s2 ) {
        if( std::toupper( *s1 ) > std::toupper( *s2 ) ) return  1;
        if( std::toupper( *s1 ) < std::toupper( *s2 ) ) return -1;
        s1++; s2++;
    }

    // We've encounted the end of one of the strings. Up until now, all characters have been
    // equal.
    if( std::toupper( *s1 ) > std::toupper( *s2 ) ) return  1;
    if( std::toupper( *s1 ) < std::toupper( *s2 ) ) return -1;
    return 0;
}


//! Compare strings in a case insensitive manner up to a specific number of characters.
/*!
 * The function _strnicmp is available in the Open Watcom library, but not in other libraries.
 * To simplify cross compiler portability I am implementing a version here to be used with all
 * compilers.
 */
int my_strnicmp( const char *s1, const char *s2, int n )
{
    while( *s1 && *s2 && n-- ) {
        if( std::toupper( *s1 ) > std::toupper( *s2 ) ) return  1;
        if( std::toupper( *s1 ) < std::toupper( *s2 ) ) return -1;
        s1++; s2++;
    }

    // We've encounted the end of one of the strings. Up until now, all characters have been
    // equal.
    if( std::toupper( *s1 ) > std::toupper( *s2 ) ) return  1;
    if( std::toupper( *s1 ) < std::toupper( *s2 ) ) return -1;
    return 0;
}


//! Displays a message for one second.
/*!
 * This function displays the string specified by 'format' for one second. The string is
 * displayed in a window in the middle of the screen which has no "header". The format string
 * can contain printf format specifiers.
 */
void info_message( const char *format, ... )
{
    char    buffer[128+1]; // Largest possible message without program failure.
    std::va_list arg_pointer;

    // TODO: Fix problems if string becomes too long!
    va_start( arg_pointer, format );
    std::vsprintf( buffer, format, arg_pointer );
    va_end( arg_pointer );

    scr::MessageWindow message( buffer, scr::MESSAGE_WINDOW_MESSAGE );

    // Refresh the screen so the user can see the window!
    scr::refresh( );

    // Wait the required time.
    spica::Timer wait;
    wait.start( );
    while( wait.time( ) < 1000L ) /* Null */ ;
    message.close( );
}


//! Displays a message and waits for user confirmation.
/*!
 * This function displays a message and waits for the user to strike a key.
 *
 * \param string The message to display.
 * \param non_default If the user enters this character, the default action is avoided.
 * \param ESC_default If 'NO' then the ESC key can also be used to avoid the default action.
 *
 * \return true if the user wants the default action; false otherwise.
 */
bool confirm_message( const char *string, char non_default, bool ESC_default )
{
    bool took_default = true;

    scr::MessageWindow confirm;
    confirm.set( string, 1 );

    int result  = confirm.open( );
    result      = std::toupper( result );
    non_default = static_cast< char >( std::toupper( non_default ) );

    if( result == non_default ) took_default = false;
    if( result == scr::K_ESC && ESC_default == false ) took_default = false;
    return took_default;
}


//! Displays a warning message.
/*!
 * This function displays the specified string (which can have printf format specifiers in it)
 * in a window with the warning header.
 */
void warning_message( const char *format, ... )
{
    char    buffer[128+1];
    std::va_list arg_pointer;

    // TODO: Fix problems if string becomes too long!
    va_start( arg_pointer, format );
    std::vsprintf( buffer, format, arg_pointer );
    va_end( arg_pointer );

    scr::MessageWindow message( buffer, scr::MESSAGE_WINDOW_WARNING );
}


// Displays an error message.
/*!
 * This function displays the specified string (which can have printf format specifiers in it)
 * in a window with the error header.
 */
void error_message( const char *format, ... )
{
    char    buffer[128+1];
    std::va_list arg_pointer;

    // TODO: Fix problems if string becomes too long!
    va_start( arg_pointer, format );
    std::vsprintf( buffer, format, arg_pointer );
    va_end( arg_pointer );

    scr::MessageWindow message( buffer, scr::MESSAGE_WINDOW_ERROR );
}


//! Displays an out of memory error message.
/*!
 * \param string A message that explains what caused the out of memory problem.
 */
void memory_message( const char *string )
{
    error_message( "OUT OF MEMORY: %s", string );
}
