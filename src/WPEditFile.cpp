/*! \file    WPEditFile.cpp
 *  \brief   Implementation of class WPEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * IMPLEMENTATION NOTES
 *
 * It would be better if the cursor moved when the paragraph was adjusted so that it stayed
 * attached to the same text.
 *
 * It may be desirable to redesign this code in a later version of Y; it needs it.
*/

#include <cctype>
#include <cstring>

#include "EditBuffer.hpp"
#include "support.hpp"
#include "WPEditFile.hpp"

/*=======================================*/
/*           Private Functions           */
/*=======================================*/

//! Return true if given character is a legitimate paragraph character.
/*!
 * If characters of this type are the first characters on a line, then the line is taken as part
 * of a paragraph.
 */
static bool paragraph_char( int ch )
{
    return static_cast< bool > (
        std::isalpha(ch)  ||  std::isdigit(ch)  ||
        ch == '('  ||
        ch == '$'  ||
        ch == '\"' ||
        ch == '\''
       );
}


//! Return true if the given line is blank; false otherwise.
static bool blank_line( EditBuffer *line )
{
    for( unsigned i = 0; i < line->length( ); i++ ) {
        if( (*line)[i] != ' ' ) return false;
    }
    return true;
}


/*!
 * This function finds the line number of the first line in the current paragraph. The "current"
 * paragraph is the one in which the list's current point can be found. This function assumes
 * that the list is set at a meaningful line. Do not pass it lists which are set at
 * List::Off_End.
 */
static long first_line( EditList &list )
{
    // Remember the old active line for later restoration.
    long old_line = list.current_index( );

    // Back up current point to first line not in paragraph, first line which starts with a
    // space, or line zero.
    //
    while( list.current_index( ) != 0  &&  paragraph_char( (*list.get( ))[0] ) ){
        list.previous( );
    }

    // Advance if we backed up too far (ie off paragraph).
    if( blank_line( list.get( ) ) ) {
        list.next( );
    }
    if( !paragraph_char( (*list.get( ))[0] )  &&  (*list.get( ))[0] != ' ' ) {
        list.next( );
    }

    // Record the line number and reset the object.
    long return_value = list.current_index( );
    list.jump_to( old_line );

    return return_value;
}


/*!
 * This function finds the line number of the last line + 1 in the current paragraph. The
 * "current" paragraph is the one in which the list's current point can be found. This function
 * assumes that the list is set at a resonable line. Do not pass it lists set at List::Off_End.
 */
static long last_line( EditList &list )
{
    long old_line = list.current_index( );

    // Step forward until we step off the end of the current paragraph.
    do {
        list.next( );
    } while( list.get( ) != NULL  &&  paragraph_char( (*list.get( ))[0] ) );

    // Record the line number just past the end and reset the object.
    long return_value = list.current_index( );
    list.jump_to( old_line );

    return return_value;
}


/*!
 * This function does the dirty work of reformatting a paragraph. It must be given the range of
 * lines to reformat (first <= range < last). It assumes that the lines exist in the given
 * EditList. This function returns false if it encounters memory problems during the operation.
 */
static bool process_paragraph( EditList &list, long first, long last )
{
    bool         abort = false;  // =true if we've run out of memory.
    EditList     temp;           // Holds text of paragraph temporarly.
    const char  *indentor = "";  // Used to indent the first line of the para.

    // Position the list to start of paragraph. Note if the paragraph is indented.
    list.jump_to( first );
    if( (*list.get( ))[0] == ' ' ) indentor = "     ";

    // Copy pointers to lines into temporary and delete free nodes from the list.
    long count = last - first;
    while( !abort && count-- ) {
        if( temp.insert( list.get( ) ) == NULL ) abort = true;
        else list.erase( );
    }

    // Tell user if it didn't work.
    if( abort ) {
        memory_message( "Can't create scratch copy of paragraph" );
        return false;
    }

    // Create a new line to hold the reformatted text.
    EditBuffer *new_line = new EditBuffer( indentor );
  
    // Loop over all the lines in the temporary list.
    for( temp.jump_to( 0 ); !abort && temp.get( ) != NULL; temp.next( ) ) {

        // TODO: Clean this mess up!
        std::string string_buffer = temp.get( )->to_string( );
        char *buffer = new char[string_buffer.length( ) + 1];
        std::strcpy( buffer, string_buffer.c_str( ) );
        char *word = std::strtok( buffer, " " );

        // Loop over all words in the line from the temporary list.
        while( word != NULL ) {

            // If this line will become too long, insert what we've got and start next line.
            if( new_line->length( ) + std::strlen( word ) > 96 ) {
                list.insert( new_line );
                new_line = new EditBuffer( word );
                new_line->append( ' ' );
            }

            // Otherwise put this word on the temporary line.
            else {
                new_line->append( word );
                new_line->append( ' ' );
            }

            // Try to extract the next word.
            word = std::strtok( NULL, " " );
        }

        delete buffer;
    }

    if( abort ) {
        memory_message( "Can't rebuild entire paragraph" );
        delete new_line;
        return false;
    }

    // If final line is partially filled. Insert it as well.
    if( !abort && new_line->length( ) > 0 ) {
        list.insert( new_line );
    }
    else {
        delete new_line;
    }

    return true;
}

/*=============================================*/
/*           Public Member Functions           */
/*=============================================*/


/*!
 * \bug This method marks the object as changed even if the reformatted paragraph is exactly the
 * same as the original.
 */
bool WPEditFile::reformat_paragraph( )
{
    bool result = true;

    // Synchronize the file data and extract a pointer to the current line.
    file_data.jump_to( current_point.cursor_line( ) );
    EditBuffer *current_line = file_data.get( );

    // Do interesting stuff only if the line exists and it starts with a paragraph character.
    if( current_line != NULL &&
        ( (*current_line)[0] == ' '  ||  paragraph_char( (*current_line)[0] ) ) ) {

        // Compute range of paragraph.
        long first = first_line( file_data );
        long last  = last_line( file_data );

        // Modify the object and mark it as changed.
        is_changed = true;
        result = process_paragraph( file_data, first, last );
    }
    return result;
}
