/*! \file    special.cpp
 *  \brief   Implementation of specialized YEditFile classes.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cctype>
#include <cstring>

#include "EditBuffer.hpp"
#include "scr.hpp"
#include "special.hpp"
#include "support.hpp"

const char *asm_keys[] = {
    "MACRO",   "macro",
    "PROC",    "proc",
    "STRUCT",  "struct",
    NULL
};

const char *ada_keys[] = {
    "FUNCTION",  "function",  "Function",
    "PACKAGE",   "package",   "Package",
    "PROCEDURE", "procedure", "Procedure",
    "TASK",      "task",      "Task",
    "ACCEPT",    "accept",    "Accept",
    NULL
};

const char *pseudocode_keys[] = {
    "CLASS",
    "FUNCTION",
    "TYPE",
    NULL
};

static const char *check_keys( const std::string &line, const char **keys )
{
    const char *subline = NULL;
    
    while( *keys != NULL ) {
        if( ( subline = std::strstr( line.c_str( ), *keys ) ) != NULL ) return subline;
        keys++;
    }
    return subline;
}

static int brace_count( const std::string &line )
{
    int count = 0;

    for( char ch : line ) {
        if( ch == '{' ) count++;
        if( ch == '}' ) count--;
    }
    return count;
}

/*===================================================*/
/*           ADA_YEditFile Specialization           */
/*===================================================*/

bool ADA_YEditFile::next_procedure( )
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );
    file_data.next( );

    while( !found && ( ( line = file_data.next( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, ada_keys );
        const char *comment;
        if( substring ) {

            // Ignore keywords inside of comments.
            if( ( comment = std::strstr( temp.c_str( ), "--" ) ) == NULL ) found = true;
            else if( substring < comment ) found = true;
        }
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) - 1 );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

bool ADA_YEditFile::previous_procedure( )
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );

    while( !found && ( ( line = file_data.previous( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, ada_keys );
        const char *comment;
        if( substring ) {

            // Ignore keywords inside of comments.
            if( ( comment = std::strstr( temp.c_str( ), "--" ) ) == NULL ) found = true;
            else if( substring < comment ) found = true;
        }
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

/*===================================================*/
/*           ASM_YEditFile Specialization           */
/*===================================================*/

bool ASM_YEditFile::next_procedure()
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );
    file_data.next( );

    while( !found && ( ( line = file_data.next( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, asm_keys );
        if( substring ) {

            // Ignore "procedure", "process", etc.
            if( ( *substring != 'p' && *substring != 'P' ) || !std::isalpha( substring[4] ) )
                found = true;
        }
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) - 1 );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

bool ASM_YEditFile::previous_procedure()
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );
    
    while( !found && ( ( line = file_data.previous( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, asm_keys );
        if( substring ) {

            // Ignore "procedure", "process", etc.
            if( ( *substring != 'p' && *substring != 'P' ) || !std::isalpha( substring[4] ) )
                found = true;
        }
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

/*=================================================*/
/*           C_YEditFile Specialization           */
/*=================================================*/

static long find_head(EditList &data, long brace_line)
{
    long head_line = brace_line;

    // Position the EditList at the line which contains the function's first {
    data.jump_to( brace_line );

    // Back the list up until we find the first unindented line.
    while( head_line > 0                &&
           brace_line - head_line < 10  &&
           (*data.get( ))[0] == ' ' ) {
        data.previous( );
        head_line--;
    }

    if ( (*data.get( ))[0] == ' ' ) return brace_line;
    else return head_line;
}

bool C_YEditFile::next_procedure( )
{
    bool found       = false;
    int  total_count = 0;
    int  line_count;
    EditBuffer *line;

    if( marks_valid && current_point.cursor_line( ) == function_head )
        current_point.jump_to_line( function_brace );

    long line_number = current_point.cursor_line( );

    file_data.jump_to( 0 );

    while( ( line = file_data.next( ) ) != NULL ) {
        std::string temp = line->to_string( );
        line_count = brace_count( temp );
        if( total_count == 0 &&
            line_count   > 0 &&
            file_data.current_index( ) - 1 > line_number ) {

            found = true;
            break;
        }
        total_count += line_count;
    }

    if( found ) {
        marks_valid    = true;
        function_brace = file_data.current_index( ) - 1;
        function_head  = find_head( file_data, function_brace );
        current_point.jump_to_line( function_head );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );
    
    return true;
}

bool C_YEditFile::previous_procedure()
{
    bool found       = false;
    int  total_count = 0;
    int  line_count;
    long line_number = current_point.cursor_line( );
    long last_found;
    EditBuffer *line;

    file_data.jump_to( 0 );

    while( ( line = file_data.next( ) ) != NULL ) {
        if( file_data.current_index( ) - 1 == line_number ) break;
        std::string temp = line->to_string( );
        line_count = brace_count( temp );
        if( total_count == 0  &&  line_count > 0 ) {
            last_found = file_data.current_index( ) - 1;
            found = true;
        }
        total_count += line_count;
    }
    
    if( found ) {
        marks_valid    = true;
        function_brace = last_found;
        function_head  = find_head( file_data, last_found );
        current_point.jump_to_line( function_head );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

bool C_YEditFile::extra_indent( )
{
    long line_number = current_point.cursor_line( );
    
    if( line_number == 0L ) return false;

    line_number--;
    file_data.jump_to( line_number );

    std::string previous_line = file_data.get( )->to_string( );
    std::string::size_type last = previous_line.find_last_not_of( " " );
    if( last != std::string::npos && previous_line[last] == '{' ) return true;
    return false;
}

bool C_YEditFile::insert_char( char letter )
{
    // Insert the character in the usual way.
    YEditFile::insert_char( letter );

    // If this is a close brace, handle it in a special way.
    if( letter == '}' ) {

        bool spaces_only = true;                           // Assume everything is a space.
        file_data.jump_to( current_point.cursor_line( ) ); // Position list at current line.

        // Loop over all characters on this row up to the '}'.
        unsigned space_count;
        for( space_count = 0; space_count < current_point.cursor_column( ); space_count++ ) {

            // If this is a non-space, break out at once.
            if( (*file_data.get( ))[space_count] != ' ' ) {
                spaces_only = false;
                break;
            }
        }

        // If we found only spaces, backspace to the previous tab stop.
        if( spaces_only  &&  space_count != 0 ) {
            do {
                backspace( );
                current_point.cursor_left( );
            } while( --space_count % tab_distance( ) != 0 );
        }
        
    }
    return true;
}

/*===================================================*/
/*           PCD_YEditFile Specialization           */
/*===================================================*/

bool PCD_YEditFile::next_procedure( )
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );
    file_data.next( );

    while( !found && ( ( line = file_data.next( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, pseudocode_keys );
        if( substring ) found = true;
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) - 1 );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

bool PCD_YEditFile::previous_procedure()
{
    bool found = false;
    EditBuffer *line;

    file_data.jump_to( current_point.cursor_line( ) );

    while( !found && ( ( line = file_data.previous( ) ) != NULL ) ) {
        std::string temp = line->to_string( );
        const char *substring = check_keys( temp, pseudocode_keys );
        if( substring ) found = true;
    }

    if( found ) {
        current_point.jump_to_line( file_data.current_index( ) );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

/*====================================================*/
/*           SCALA_YEditFile Specialization           */
/*====================================================*/

// Right now the SCALA_YEditFile is a replica of the C_YEditFile except that the tab stop
// setting is different. Eventually the logic should be improved here to account for Scala's
// more complex syntactic structure (in particular, its ability to nest anything inside of
// anything else). At some point a D_YEditFile should also be defined with similar features.

bool SCALA_YEditFile::next_procedure( )
{
    bool found       = false;
    int  total_count = 0;
    int  line_count;
    EditBuffer *line;

    if( marks_valid && current_point.cursor_line( ) == function_head )
        current_point.jump_to_line( function_brace );

    long line_number = current_point.cursor_line( );

    file_data.jump_to( 0 );

    while( ( line = file_data.next( ) ) != NULL ) {
        std::string temp = line->to_string( );
        line_count = brace_count( temp );
        if( total_count == 0 &&
            line_count   > 0 &&
            file_data.current_index( ) - 1 > line_number ) {

            found = true;
            break;
        }
        total_count += line_count;
    }

    if( found ) {
        marks_valid    = true;
        function_brace = file_data.current_index( ) - 1;
        function_head  = find_head( file_data, function_brace );
        current_point.jump_to_line( function_head );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );
    
    return true;
}

bool SCALA_YEditFile::previous_procedure()
{
    bool found       = false;
    int  total_count = 0;
    int  line_count;
    long line_number = current_point.cursor_line( );
    long last_found;
    EditBuffer *line;

    file_data.jump_to( 0 );

    while( ( line = file_data.next( ) ) != NULL ) {
        if( file_data.current_index( ) - 1 == line_number ) break;
        std::string temp = line->to_string( );
        line_count = brace_count( temp );
        if( total_count == 0  &&  line_count > 0 ) {
            last_found = file_data.current_index( ) - 1;
            found = true;
        }
        total_count += line_count;
    }
    
    if( found ) {
        marks_valid    = true;
        function_brace = last_found;
        function_head  = find_head( file_data, last_found );
        current_point.jump_to_line( function_head );
        current_point.adjust_window_line( 1 );
    }
    else info_message( "Not found" );

    return true;
}

bool SCALA_YEditFile::extra_indent( )
{
    long line_number = current_point.cursor_line( );
    
    if( line_number == 0L ) return false;

    line_number--;
    file_data.jump_to( line_number );

    std::string previous_line = file_data.get( )->to_string( );
    std::string::size_type last = previous_line.find_last_not_of( " " );
    if( last != std::string::npos && previous_line[last] == '{' ) return true;
    return false;
}

bool SCALA_YEditFile::insert_char( char letter )
{
    // Insert the character in the usual way.
    YEditFile::insert_char( letter );

    // If this is a close brace, handle it in a special way.
    if( letter == '}' ) {

        bool spaces_only = true;                           // Assume everything is a space.
        file_data.jump_to( current_point.cursor_line( ) ); // Position list at current line.

        // Loop over all characters on this row up to the '}'.
        unsigned space_count;
        for( space_count = 0; space_count < current_point.cursor_column( ); space_count++ ) {

            // If this is a non-space, break out at once.
            if( (*file_data.get( ))[space_count] != ' ' ) {
                spaces_only = false;
                break;
            }
        }

        // If we found only spaces, backspace to the previous tab stop.
        if( spaces_only  &&  space_count != 0 ) {
            do {
                backspace( );
                current_point.cursor_left( );
            } while( --space_count % tab_distance( ) != 0 );
        }
        
    }
    return true;
}
