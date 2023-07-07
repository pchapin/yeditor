/*! \file    WordSource.hpp
 *  \brief   Interface to the WordSource classes.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef WORDSOURCE_HPP
#define WORDSOURCE_HPP

#include <cstddef>
#include <cstdio>

#include "EditBuffer.hpp"

/*!
 * An abstract base class from which the various types that can provide macro words are defined.
 * The constructor of the derived class sets up the source. The method get_word() extracts the
 * next word from the source.
 */
class WordSource {
public:
             WordSource( ) : current_state( NORMAL ) { }
    virtual ~WordSource( ) { return; }

    /*!
     * Returns the next word from the word source by way of its parameter. Returns false if
     * there are no more words left (in which case the parameter is unchanged); otherwise it
     * returns true. This function makes multiple calls on the virtual functions get() and
     * unget() to read the actual word source.
     */
    virtual bool get_word( EditBuffer &word );

private:
    // These states are used by the finite state machine in get_word() used to extract words.
    enum State {
        NORMAL,     COMMENT,   COLLECT_NAME, STRING,    ESC,
        BIG_STRING, BIG_WHITE, BIG_COMMENT,  BIG_QUOTE, BIG_ESC
    };

    // Initially NORMAL.
    State current_state;

    /*!
     * Returns the next character from the word source or EOF if source is empty. Repeated calls
     * against an empty source will continue to return EOF.
     */
    virtual int get( ) = 0;

    /*!
     * Pushs the given character back into the word source so that the next call to get() will
     * return that character. It is undefined to unget() to a word source for which has not yet
     * had a call to get(). Only one level of push back is supported.
     */
    virtual void unget( int ch ) = 0;
  };


//! The following class encapsulates a source of words that are stored in a string.
class StringWord : public WordSource {
public:
    explicit StringWord( const char *const source_string ) :
        WordSource( ),
        string_buffer( source_string ),
        length( string_buffer.length( ) ),
        offset( 0 )
    { }

private:
    EditBuffer string_buffer; //!< The text in question.
    std::size_t     length;        //!< The number of characters in string_buffer.
    std::size_t     offset;        //!< Current get() location.

    virtual int  get( );
    virtual void unget( int ch );
};


//! Objects of this class take words from the keyboard.
class KeyboardWord : public WordSource {
public:
    virtual bool get_word( EditBuffer &word );

private:
    virtual int  get( );
    virtual void unget( int ch );
};


//! The following class encapsulates a source of words that are stored in a text file.
class FileWord : public WordSource {
public:
    explicit FileWord( const char *file_name );
   ~FileWord( );

private:
    std::FILE *input_file;

    virtual int  get( );
    virtual void unget( int ch );
};


/*!
 * Allows the caller to install a line of macro text into the key map at the key with the
 * specified name. This modifies the stream of macro words returned by a KeyboardWord object.
 * This function is used to remap the keyboard.
 */
void modify_key_association( const char *key_name, const char *new_macro_text );

#endif
