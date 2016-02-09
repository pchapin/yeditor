/*! \file    WordStream.cpp
    \brief   This file contains the implementation of various word stream classes.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstring>
#include "WordStream.hpp"

// -------------------
// Word_Stream Members
// -------------------

const char *WordStream::delimiters = " \t";
char  WordStream::comment    = ';';

void WordStream::set_delimiters( char *D ) { delimiters = D; }
void WordStream::set_comment( char C )     { comment    = C; }

WordStream::~WordStream( ) { }

/*!
 * The following function accepts a pointer which points into a string. It returns a pointer
 * into that same string that marks the beginning of the next word. If there are no more words,
 * the function returns a pointer to the null character.
 *
 * If a comment character is encountered, the function returns a pointer to the null character.
 */
const char *WordStream::find_next_word( const char *string )
{
    while( *string ) {
        if( std::strchr( delimiters, *string ) != 0 ) string++;
        else if( *string == comment ) string = std::strchr( string, '\0' );
        else break;
    }
    return string;
}


/*!
 * The following function takes a pointer to a string which is pointing at a word. It returns a
 * pointer to the first non word character after the word. If the initial word is a quoted
 * string, this function correctly seeks to the spot just after the close quotes (nested quotes
 * not supported). If there is no close quote, the end of the string is assumed to be the end of
 * the word.
 *
 * If a comment character is encountered in the word, it is taken to be the end of the word. If
 * a quote is found in the word, it is taken to be the end of the word. This handles cases like:
 *
 *   word;comment
 *   word1"This is another word"word2
 */
const char *WordStream::find_next_space( const char *string )
{
    // If this word is a quoted string, search for the close quote mark.
    if( *string == '\"' ) {
        const char *end_quote = std::strchr( string + 1, '\"' );

        if( end_quote != 0 ) end_quote++;
        else end_quote = std::strchr( string + 1, '\0' );
        return end_quote;
    }

    // Otherwise, scan until we get to a delimiter, comment mark, or quote.
    while( *string ) {
        if( *string == '\"' ) break;
        if( *string == comment ) break;
        if( std::strchr( delimiters, *string ) == 0 ) string++;
        else break;
    }

    return string;
}

// ---------------------
// String_Stream Members
// ---------------------

/*!
 * Find the next word in the encapsulated string. The value of current_point marks where to
 * start the search. Note that current_point is initialized by the constructor.
 */
std::string StringStream::next_word( )
{
    std::string word;

    // Find the start of the next word.
    current_point = find_next_word( current_point );

    // If there are no more words, return an empty word.
    if( *current_point == '\0' ) return word;

    // Find the point just past the end of this word.
    const char *end_word = find_next_space( current_point );

    // Copy this word into the holding area.
    word.assign( current_point, static_cast<std::string::size_type>( end_word - current_point) );

    current_point = end_word;
    return word;
  }


// -------------------
// File_Stream Members
// -------------------

/*!
 * The following function gets another word from the word file. Notice that it will read lines
 * as necessary until it either gets a word or exhausts the file. Notice also that, due to the
 * behavior of find_next_space(), a quoted string that is not terminated on a line will have the
 * end of line be taken as a terminator. For example:
 *
 *   This is an "unmatched quoted string
 *   These words are each separate" This is inside another string!
 */
std::string FileStream::next_word( )
{
    std::string word;

    // Keep looping until we can get a word.
    while( 1 ) {

        // If the line_buffer is empty, read the file.
        if( current_point == 0 ) {
            if( !word_source ) return word;
            getline( word_source, line_buffer );
            current_point = line_buffer.c_str( );
        }

        // Try to get a word out of the current line. Break out if we get one.
        current_point = find_next_word( current_point );
        if( *current_point != '\0' ) break;
        current_point = 0;
    }

    // Locate the end of the word, etc.
    const char *end_word = find_next_space( current_point );

    // Copy this word into the holding area.
    word.assign(
        current_point,
        static_cast<std::string::size_type>( end_word - current_point )
    );

    current_point = end_word;
    return word;
}

// --------------------
// MasterStream Members
// --------------------

/*!
 * The destructor deletes all the word stream objects on the stack.
 */
MasterStream::~MasterStream( )
{
    // Toss out all the active word streams.
    while( !stream_stack.empty( ) ) {
        delete stream_stack.top( );
        stream_stack.pop( );
    }
}


/*!
 * The following function extracts the next word from a master stream. This entails getting the
 * next word from the word stream currently on top of the stack (if any). If that word stream is
 * exhausted, the stack is popped and another attempt to get a word is made. This function only
 * returns an empty word if there are no more word streams to try.
 */
std::string MasterStream::next_word( )
{
    std::string word;

    // Loop until we get a word (or bust).
    while( 1 ) {

        // If there are no word streams left, then we are done.
        if( stream_stack.empty( ) ) break;

        // Try to get the next word out of the current word stream.
        word = stream_stack.top( )->next_word( );

        // If it worked, return it.
        if( word.length( ) != 0 ) break;

        // Otherwise, blow this word stream object away.
        delete stream_stack.top( );
        stream_stack.pop( );
    }

    return word;
}


//! The following function puts a new word stream onto the stack.
void MasterStream::push( WordStream *new_stream )
{
    stream_stack.push( new_stream );
}
