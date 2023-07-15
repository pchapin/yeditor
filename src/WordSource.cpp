/*! \file    WordSource.cpp
 *  \brief   Implementation of the WordSource classes.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cctype>
#include <cstring>
#include <sstream>

#include "EditBuffer.hpp"
#include "keyboard.hpp"
#include "macro_stack.hpp"
#include "parameter_stack.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "WordSource.hpp"

//! Returns true if ch is "whitespace".
static bool is_white( const int ch )
{
    return std::isspace( ch ) != 0;
}


//! Returns true if ch is a character that could be part of an identifier name.
static bool is_name( const int ch )
{
    if( !is_white( ch ) && ch != '{' && ch != '#' && ch != '"' ) return true;
    return false;
}


bool WordSource::get_word( EditBuffer &word )
{
    int ch;
    int nested_count;
    EditBuffer string_contents;

    // Try to loop until the word source is exhausted.
    while( ( ch = get( ) ) != EOF ) {

        // REFER TO THE FINITE STATE MACHINE DIAGRAM FOR DOCUMENTATION!!
        switch( current_state ) {
        case NORMAL:
            if( is_white( ch ) ) break;
            if( is_name ( ch ) ) {
                word.erase( );
                word.append( static_cast<char>( ch ) );
                current_state = COLLECT_NAME;
                break;
            }
            switch( ch ) {
            case '#': current_state = COMMENT; break;
            case '"':
                string_contents.erase( );
                current_state = STRING;
                break;
            case '{':
                string_contents.erase( );
                nested_count = 1;
                current_state = BIG_STRING;
                break;
            default:
                // For now, just ignore all other special characters.
                // TODO: Produce an error or warning message for unexpected characters here?
                break;
            }
            break;

        case COMMENT:
            if( ch == '\n' ) current_state = NORMAL;
            break;

        case COLLECT_NAME:
            if( is_name( ch ) ) word.append( static_cast<char>( ch ) );
            else {
                unget( ch );
                current_state = NORMAL;
                return true;               // <== NOTE RETURN HERE!!
            }
            break;

        case STRING:
            switch( ch ) {
            case '\\': current_state = ESC; break;
            case '"' :
                parameter_stack.push( string_contents );
                current_state = NORMAL;
                break;
            default:
                string_contents.append( static_cast<char>( ch ) );
                break;
            }
            break;

        case ESC:
            string_contents.append( static_cast<char>( ch ) );
            current_state = STRING;
            break;

        case BIG_STRING:
            if( is_white( ch ) ) {
                string_contents.append( ' ' );
                current_state = BIG_WHITE;
                break;
            }
            switch( ch ) {
            case '#':
                string_contents.append( ' ' );
                current_state = BIG_COMMENT;
                break;
            case '"':
                string_contents.append( static_cast<char>( ch ) );
                current_state = BIG_QUOTE;
                break;
            case '{':
                string_contents.append( static_cast<char>( ch ) );
                // The variable nested_count has been initialized.
                //lint -e{644}
                nested_count++;
                break;
            case '}':
                nested_count--;
                if( nested_count == 0 ) {
                    parameter_stack.push( string_contents );
                    current_state = NORMAL;
                }
                else {
                    string_contents.append( static_cast<char>( ch ) );
                }
                break;
            default:
                string_contents.append( static_cast<char>( ch ) );
                break;
            }
            break;

        case BIG_WHITE:
            if( !is_white( ch ) ) {
                unget( ch );
                current_state = BIG_STRING;
            }
            break;

        case BIG_COMMENT:
            if( ch == '\n' ) current_state = BIG_STRING;
            break;

        case BIG_QUOTE:
            if( ch == '\\' ) {
                string_contents.append( static_cast<char>( ch ) );
                current_state = BIG_ESC;
            }
            else if( ch == '"' ) {
                string_contents.append( static_cast<char>( ch ) );
                current_state = BIG_STRING;
            }
            else {
                string_contents.append( static_cast<char>( ch ) );
            }
            break;

        case BIG_ESC:
            string_contents.append( static_cast<char>( ch ) );
            current_state = BIG_QUOTE;
            break;
        }
    }

    // ch == EOF. Let's see what state we're in now. Return true if we are at the end of a valid
    // word, otherwise return false.

    switch( current_state ) {
    case NORMAL:
        return false;

    case COMMENT:
        return false;

    case COLLECT_NAME:
        current_state = NORMAL;   // So we work on the next call to get_word( ).
        return true;

    case STRING:
        parameter_stack.push( string_contents );
        return false;

    case ESC:
        parameter_stack.push( string_contents );
        return false;

    case BIG_ESC:
        string_contents.append( '"' );
        //lint -fallthrough
    case BIG_QUOTE:
        string_contents.append( '"' );
        //lint -fallthrough
    case BIG_STRING:
    case BIG_WHITE:
    case BIG_COMMENT:
        for( int i = nested_count; i > 1; --i )
            string_contents.append( '}' );
        return false;
    }

    // Should never get here. This line is to keep the compiler quiet.
    return false;
}


//= String_Word ===========================================================

int StringWord::get( )
{
    if( offset >= length ) return EOF;
    return string_buffer[offset++];
}


void StringWord::unget( const int ch )
{
    string_buffer.replace(static_cast<char>( ch ), --offset);
}


//= Keyboard_Word =========================================================

int KeyboardWord::get( )
{
    // This should never happen.
    error_message( "!!! Inside Keyboard_Word::get( ) !!!" );
    return EOF;
}


void KeyboardWord::unget( int )
{
    // This should never happen.
    error_message( "!!! Inside Keyboard_Word::unget( int ) !!!" );
}

// Also see below for the definition of Keyboard_Word::get_word( )


//= File_Word =============================================================

FileWord::FileWord( const char *const file_name) :
    WordSource( ),
    input_file( std::fopen( file_name, "r" ) )
{
    if( input_file == nullptr )
        error_message("Can't open macro file %s for reading", file_name);
}


FileWord::~FileWord( )
{
    if( input_file != nullptr ) std::fclose( input_file );
}


int FileWord::get( )
{
    if( input_file == nullptr ) return EOF;
    return std::getc( input_file );
}


void FileWord::unget( const int ch )
{
    if( input_file != nullptr ) std::ungetc( ch, input_file );
}


//=========================================================================

struct KeyboardAssociation {
    int key_code;
    EditBuffer macro_text;

    KeyboardAssociation( const int code, const char *const text ) :
        key_code( code ), macro_text( text ) { }
};


// The following table forms the association between keystrokes and macro text. For each
// keystroke that might be returned from get_key(), there is some default macro text. When a
// KeyboardWord object gets a keystroke, it searches this table for the associated macro and
// starts the execution of that macro.
//
static KeyboardAssociation keyboard_map[] = {

    // Control keys...
    KeyboardAssociation( 0x00, "\"Can't add null characters\" error_message" ),
    KeyboardAssociation( 0x01, "\"\x01\" add_text" ),
    KeyboardAssociation( 0x02, "\"\x02\" add_text" ),
    KeyboardAssociation( 0x03, "\"\x03\" add_text" ),
    KeyboardAssociation( 0x04, "\"\x04\" add_text" ),
    KeyboardAssociation( 0x05, "\"\x05\" add_text" ),
    KeyboardAssociation( 0x06, "\"\x06\" add_text" ),
    KeyboardAssociation( 0x07, "\"\x07\" add_text" ),
    KeyboardAssociation( 0x08, "backspace" ),
    KeyboardAssociation( 0x09, "tab" ),
    KeyboardAssociation( 0x0A, "delete_to_eol" ),
    KeyboardAssociation( 0x0B, "\"\x0B\" add_text" ),
    KeyboardAssociation( 0x0C, "\"\x0C\" add_text" ),
    KeyboardAssociation( 0x0D, "new_line" ),
    KeyboardAssociation( 0x0E, "\"\x0E\" add_text" ),
    KeyboardAssociation( 0x0F, "\"\x0F\" add_text" ),
    KeyboardAssociation( 0x10, "\"\x10\" add_text" ),
    KeyboardAssociation( 0x11, "\"\x11\" add_text" ),
    KeyboardAssociation( 0x12, "\"\x12\" add_text" ),
    KeyboardAssociation( 0x13, "\"\x13\" add_text" ),
    KeyboardAssociation( 0x14, "\"\x14\" add_text" ),
    KeyboardAssociation( 0x15, "\"\x15\" add_text" ),
    KeyboardAssociation( 0x16, "\"\x16\" add_text" ),
    KeyboardAssociation( 0x17, "\"\x17\" add_text" ),
    KeyboardAssociation( 0x18, "\"\x18\" add_text" ),
    KeyboardAssociation( 0x19, "\"\x19\" add_text" ),
    KeyboardAssociation( 0x1A, "\"\x1A\" add_text" ),
    KeyboardAssociation( 0x1B, "block_off" ),
    KeyboardAssociation( 0x1C, "\"\x1C\" add_text" ),
    KeyboardAssociation( 0x1D, "\"\x1D\" add_text" ),
    KeyboardAssociation( 0x1E, "\"\x1E\" add_text" ),
    KeyboardAssociation( 0x1F, "\"\x1F\" add_text" ),

    // Normal keys...
    KeyboardAssociation( 0x20, "\"\x20\" add_text" ),
    KeyboardAssociation( 0x21, "\"\x21\" add_text" ),
    KeyboardAssociation( 0x22, "\"\\\x22\" add_text" ),  // Quote the double quote.
    KeyboardAssociation( 0x23, "\"\x23\" add_text" ),
    KeyboardAssociation( 0x24, "\"\x24\" add_text" ),
    KeyboardAssociation( 0x25, "\"\x25\" add_text" ),
    KeyboardAssociation( 0x26, "\"\x26\" add_text" ),
    KeyboardAssociation( 0x27, "\"\x27\" add_text" ),
    KeyboardAssociation( 0x28, "\"\x28\" add_text" ),
    KeyboardAssociation( 0x29, "\"\x29\" add_text" ),
    KeyboardAssociation( 0x2A, "\"\x2A\" add_text" ),
    KeyboardAssociation( 0x2B, "\"\x2B\" add_text" ),
    KeyboardAssociation( 0x2C, "\"\x2C\" add_text" ),
    KeyboardAssociation( 0x2D, "\"\x2D\" add_text" ),
    KeyboardAssociation( 0x2E, "\"\x2E\" add_text" ),
    KeyboardAssociation( 0x2F, "\"\x2F\" add_text" ),
    KeyboardAssociation( 0x30, "\"\x30\" add_text" ),
    KeyboardAssociation( 0x31, "\"\x31\" add_text" ),
    KeyboardAssociation( 0x32, "\"\x32\" add_text" ),
    KeyboardAssociation( 0x33, "\"\x33\" add_text" ),
    KeyboardAssociation( 0x34, "\"\x34\" add_text" ),
    KeyboardAssociation( 0x35, "\"\x35\" add_text" ),
    KeyboardAssociation( 0x36, "\"\x36\" add_text" ),
    KeyboardAssociation( 0x37, "\"\x37\" add_text" ),
    KeyboardAssociation( 0x38, "\"\x38\" add_text" ),
    KeyboardAssociation( 0x39, "\"\x39\" add_text" ),
    KeyboardAssociation( 0x3A, "\"\x3A\" add_text" ),
    KeyboardAssociation( 0x3B, "\"\x3B\" add_text" ),
    KeyboardAssociation( 0x3C, "\"\x3C\" add_text" ),
    KeyboardAssociation( 0x3D, "\"\x3D\" add_text" ),
    KeyboardAssociation( 0x3E, "\"\x3E\" add_text" ),
    KeyboardAssociation( 0x3F, "\"\x3F\" add_text" ),
    KeyboardAssociation( 0x40, "\"\x40\" add_text" ),
    KeyboardAssociation( 0x41, "\"\x41\" add_text" ),
    KeyboardAssociation( 0x42, "\"\x42\" add_text" ),
    KeyboardAssociation( 0x43, "\"\x43\" add_text" ),
    KeyboardAssociation( 0x44, "\"\x44\" add_text" ),
    KeyboardAssociation( 0x45, "\"\x45\" add_text" ),
    KeyboardAssociation( 0x46, "\"\x46\" add_text" ),
    KeyboardAssociation( 0x47, "\"\x47\" add_text" ),
    KeyboardAssociation( 0x48, "\"\x48\" add_text" ),
    KeyboardAssociation( 0x49, "\"\x49\" add_text" ),
    KeyboardAssociation( 0x4A, "\"\x4A\" add_text" ),
    KeyboardAssociation( 0x4B, "\"\x4B\" add_text" ),
    KeyboardAssociation( 0x4C, "\"\x4C\" add_text" ),
    KeyboardAssociation( 0x4D, "\"\x4D\" add_text" ),
    KeyboardAssociation( 0x4E, "\"\x4E\" add_text" ),
    KeyboardAssociation( 0x4F, "\"\x4F\" add_text" ),
    KeyboardAssociation( 0x50, "\"\x50\" add_text" ),
    KeyboardAssociation( 0x51, "\"\x51\" add_text" ),
    KeyboardAssociation( 0x52, "\"\x52\" add_text" ),
    KeyboardAssociation( 0x53, "\"\x53\" add_text" ),
    KeyboardAssociation( 0x54, "\"\x54\" add_text" ),
    KeyboardAssociation( 0x55, "\"\x55\" add_text" ),
    KeyboardAssociation( 0x56, "\"\x56\" add_text" ),
    KeyboardAssociation( 0x57, "\"\x57\" add_text" ),
    KeyboardAssociation( 0x58, "\"\x58\" add_text" ),
    KeyboardAssociation( 0x59, "\"\x59\" add_text" ),
    KeyboardAssociation( 0x5A, "\"\x5A\" add_text" ),
    KeyboardAssociation( 0x5B, "\"\x5B\" add_text" ),
    KeyboardAssociation( 0x5C, "\"\\\x5C\" add_text" ),  // Quote the backslash.
    KeyboardAssociation( 0x5D, "\"\x5D\" add_text" ),
    KeyboardAssociation( 0x5E, "\"\x5E\" add_text" ),
    KeyboardAssociation( 0x5F, "\"\x5F\" add_text" ),
    KeyboardAssociation( 0x60, "\"\x60\" add_text" ),
    KeyboardAssociation( 0x61, "\"\x61\" add_text" ),
    KeyboardAssociation( 0x62, "\"\x62\" add_text" ),
    KeyboardAssociation( 0x63, "\"\x63\" add_text" ),
    KeyboardAssociation( 0x64, "\"\x64\" add_text" ),
    KeyboardAssociation( 0x65, "\"\x65\" add_text" ),
    KeyboardAssociation( 0x66, "\"\x66\" add_text" ),
    KeyboardAssociation( 0x67, "\"\x67\" add_text" ),
    KeyboardAssociation( 0x68, "\"\x68\" add_text" ),
    KeyboardAssociation( 0x69, "\"\x69\" add_text" ),
    KeyboardAssociation( 0x6A, "\"\x6A\" add_text" ),
    KeyboardAssociation( 0x6B, "\"\x6B\" add_text" ),
    KeyboardAssociation( 0x6C, "\"\x6C\" add_text" ),
    KeyboardAssociation( 0x6D, "\"\x6D\" add_text" ),
    KeyboardAssociation( 0x6E, "\"\x6E\" add_text" ),
    KeyboardAssociation( 0x6F, "\"\x6F\" add_text" ),
    KeyboardAssociation( 0x70, "\"\x70\" add_text" ),
    KeyboardAssociation( 0x71, "\"\x71\" add_text" ),
    KeyboardAssociation( 0x72, "\"\x72\" add_text" ),
    KeyboardAssociation( 0x73, "\"\x73\" add_text" ),
    KeyboardAssociation( 0x74, "\"\x74\" add_text" ),
    KeyboardAssociation( 0x75, "\"\x75\" add_text" ),
    KeyboardAssociation( 0x76, "\"\x76\" add_text" ),
    KeyboardAssociation( 0x77, "\"\x77\" add_text" ),
    KeyboardAssociation( 0x78, "\"\x78\" add_text" ),
    KeyboardAssociation( 0x79, "\"\x79\" add_text" ),
    KeyboardAssociation( 0x7A, "\"\x7A\" add_text" ),
    KeyboardAssociation( 0x7B, "\"\x7B\" add_text" ),
    KeyboardAssociation( 0x7C, "\"\x7C\" add_text" ),
    KeyboardAssociation( 0x7D, "\"\x7D\" add_text" ),
    KeyboardAssociation( 0x7E, "\"\x7E\" add_text" ),
    KeyboardAssociation( 0x7F, "delete_to_sol" ),

    // "Extended" ASCII keys...
    KeyboardAssociation( 0x80, "\"\x80\" add_text" ),
    KeyboardAssociation( 0x81, "\"\x81\" add_text" ),
    KeyboardAssociation( 0x82, "\"\x82\" add_text" ),
    KeyboardAssociation( 0x83, "\"\x83\" add_text" ),
    KeyboardAssociation( 0x84, "\"\x84\" add_text" ),
    KeyboardAssociation( 0x85, "\"\x85\" add_text" ),
    KeyboardAssociation( 0x86, "\"\x86\" add_text" ),
    KeyboardAssociation( 0x87, "\"\x87\" add_text" ),
    KeyboardAssociation( 0x88, "\"\x88\" add_text" ),
    KeyboardAssociation( 0x89, "\"\x89\" add_text" ),
    KeyboardAssociation( 0x8A, "\"\x8A\" add_text" ),
    KeyboardAssociation( 0x8B, "\"\x8B\" add_text" ),
    KeyboardAssociation( 0x8C, "\"\x8C\" add_text" ),
    KeyboardAssociation( 0x8D, "\"\x8D\" add_text" ),
    KeyboardAssociation( 0x8E, "\"\x8E\" add_text" ),
    KeyboardAssociation( 0x8F, "\"\x8F\" add_text" ),
    KeyboardAssociation( 0x90, "\"\x90\" add_text" ),
    KeyboardAssociation( 0x91, "\"\x91\" add_text" ),
    KeyboardAssociation( 0x92, "\"\x92\" add_text" ),
    KeyboardAssociation( 0x93, "\"\x93\" add_text" ),
    KeyboardAssociation( 0x94, "\"\x94\" add_text" ),
    KeyboardAssociation( 0x95, "\"\x95\" add_text" ),
    KeyboardAssociation( 0x96, "\"\x96\" add_text" ),
    KeyboardAssociation( 0x97, "\"\x97\" add_text" ),
    KeyboardAssociation( 0x98, "\"\x98\" add_text" ),
    KeyboardAssociation( 0x99, "\"\x99\" add_text" ),
    KeyboardAssociation( 0x9A, "\"\x9A\" add_text" ),
    KeyboardAssociation( 0x9B, "\"\x9B\" add_text" ),
    KeyboardAssociation( 0x9C, "\"\x9C\" add_text" ),
    KeyboardAssociation( 0x9D, "\"\x9D\" add_text" ),
    KeyboardAssociation( 0x9E, "\"\x9E\" add_text" ),
    KeyboardAssociation( 0x9F, "\"\x9F\" add_text" ),
    KeyboardAssociation( 0xA0, "\"\xA0\" add_text" ),
    KeyboardAssociation( 0xA1, "\"\xA1\" add_text" ),
    KeyboardAssociation( 0xA2, "\"\xA2\" add_text" ),
    KeyboardAssociation( 0xA3, "\"\xA3\" add_text" ),
    KeyboardAssociation( 0xA4, "\"\xA4\" add_text" ),
    KeyboardAssociation( 0xA5, "\"\xA5\" add_text" ),
    KeyboardAssociation( 0xA6, "\"\xA6\" add_text" ),
    KeyboardAssociation( 0xA7, "\"\xA7\" add_text" ),
    KeyboardAssociation( 0xA8, "\"\xA8\" add_text" ),
    KeyboardAssociation( 0xA9, "\"\xA9\" add_text" ),
    KeyboardAssociation( 0xAA, "\"\xAA\" add_text" ),
    KeyboardAssociation( 0xAB, "\"\xAB\" add_text" ),
    KeyboardAssociation( 0xAC, "\"\xAC\" add_text" ),
    KeyboardAssociation( 0xAD, "\"\xAD\" add_text" ),
    KeyboardAssociation( 0xAE, "\"\xAE\" add_text" ),
    KeyboardAssociation( 0xAF, "\"\xAF\" add_text" ),
    KeyboardAssociation( 0xB0, "\"\xB0\" add_text" ),
    KeyboardAssociation( 0xB1, "\"\xB1\" add_text" ),
    KeyboardAssociation( 0xB2, "\"\xB2\" add_text" ),
    KeyboardAssociation( 0xB3, "\"\xB3\" add_text" ),
    KeyboardAssociation( 0xB4, "\"\xB4\" add_text" ),
    KeyboardAssociation( 0xB5, "\"\xB5\" add_text" ),
    KeyboardAssociation( 0xB6, "\"\xB6\" add_text" ),
    KeyboardAssociation( 0xB7, "\"\xB7\" add_text" ),
    KeyboardAssociation( 0xB8, "\"\xB8\" add_text" ),
    KeyboardAssociation( 0xB9, "\"\xB9\" add_text" ),
    KeyboardAssociation( 0xBA, "\"\xBA\" add_text" ),
    KeyboardAssociation( 0xBB, "\"\xBB\" add_text" ),
    KeyboardAssociation( 0xBC, "\"\xBC\" add_text" ),
    KeyboardAssociation( 0xBD, "\"\xBD\" add_text" ),
    KeyboardAssociation( 0xBE, "\"\xBE\" add_text" ),
    KeyboardAssociation( 0xBF, "\"\xBF\" add_text" ),
    KeyboardAssociation( 0xC0, "\"\xC0\" add_text" ),
    KeyboardAssociation( 0xC1, "\"\xC1\" add_text" ),
    KeyboardAssociation( 0xC2, "\"\xC2\" add_text" ),
    KeyboardAssociation( 0xC3, "\"\xC3\" add_text" ),
    KeyboardAssociation( 0xC4, "\"\xC4\" add_text" ),
    KeyboardAssociation( 0xC5, "\"\xC5\" add_text" ),
    KeyboardAssociation( 0xC6, "\"\xC6\" add_text" ),
    KeyboardAssociation( 0xC7, "\"\xC7\" add_text" ),
    KeyboardAssociation( 0xC8, "\"\xC8\" add_text" ),
    KeyboardAssociation( 0xC9, "\"\xC9\" add_text" ),
    KeyboardAssociation( 0xCA, "\"\xCA\" add_text" ),
    KeyboardAssociation( 0xCB, "\"\xCB\" add_text" ),
    KeyboardAssociation( 0xCC, "\"\xCC\" add_text" ),
    KeyboardAssociation( 0xCD, "\"\xCD\" add_text" ),
    KeyboardAssociation( 0xCE, "\"\xCE\" add_text" ),
    KeyboardAssociation( 0xCF, "\"\xCF\" add_text" ),
    KeyboardAssociation( 0xD0, "\"\xD0\" add_text" ),
    KeyboardAssociation( 0xD1, "\"\xD1\" add_text" ),
    KeyboardAssociation( 0xD2, "\"\xD2\" add_text" ),
    KeyboardAssociation( 0xD3, "\"\xD3\" add_text" ),
    KeyboardAssociation( 0xD4, "\"\xD4\" add_text" ),
    KeyboardAssociation( 0xD5, "\"\xD5\" add_text" ),
    KeyboardAssociation( 0xD6, "\"\xD6\" add_text" ),
    KeyboardAssociation( 0xD7, "\"\xD7\" add_text" ),
    KeyboardAssociation( 0xD8, "\"\xD8\" add_text" ),
    KeyboardAssociation( 0xD9, "\"\xD9\" add_text" ),
    KeyboardAssociation( 0xDA, "\"\xDA\" add_text" ),
    KeyboardAssociation( 0xDB, "\"\xDB\" add_text" ),
    KeyboardAssociation( 0xDC, "\"\xDC\" add_text" ),
    KeyboardAssociation( 0xDD, "\"\xDD\" add_text" ),
    KeyboardAssociation( 0xDE, "\"\xDE\" add_text" ),
    KeyboardAssociation( 0xDF, "\"\xDF\" add_text" ),
    KeyboardAssociation( 0xE0, "\"\xE0\" add_text" ),
    KeyboardAssociation( 0xE1, "\"\xE1\" add_text" ),
    KeyboardAssociation( 0xE2, "\"\xE2\" add_text" ),
    KeyboardAssociation( 0xE3, "\"\xE3\" add_text" ),
    KeyboardAssociation( 0xE4, "\"\xE4\" add_text" ),
    KeyboardAssociation( 0xE5, "\"\xE5\" add_text" ),
    KeyboardAssociation( 0xE6, "\"\xE6\" add_text" ),
    KeyboardAssociation( 0xE7, "\"\xE7\" add_text" ),
    KeyboardAssociation( 0xE8, "\"\xE8\" add_text" ),
    KeyboardAssociation( 0xE9, "\"\xE9\" add_text" ),
    KeyboardAssociation( 0xEA, "\"\xEA\" add_text" ),
    KeyboardAssociation( 0xEB, "\"\xEB\" add_text" ),
    KeyboardAssociation( 0xEC, "\"\xEC\" add_text" ),
    KeyboardAssociation( 0xED, "\"\xED\" add_text" ),
    KeyboardAssociation( 0xEE, "\"\xEE\" add_text" ),
    KeyboardAssociation( 0xEF, "\"\xEF\" add_text" ),
    KeyboardAssociation( 0xF0, "\"\xF0\" add_text" ),
    KeyboardAssociation( 0xF1, "\"\xF1\" add_text" ),
    KeyboardAssociation( 0xF2, "\"\xF2\" add_text" ),
    KeyboardAssociation( 0xF3, "\"\xF3\" add_text" ),
    KeyboardAssociation( 0xF4, "\"\xF4\" add_text" ),
    KeyboardAssociation( 0xF5, "\"\xF5\" add_text" ),
    KeyboardAssociation( 0xF6, "\"\xF6\" add_text" ),
    KeyboardAssociation( 0xF7, "\"\xF7\" add_text" ),
    KeyboardAssociation( 0xF8, "\"\xF8\" add_text" ),
    KeyboardAssociation( 0xF9, "\"\xF9\" add_text" ),
    KeyboardAssociation( 0xFA, "\"\xFA\" add_text" ),
    KeyboardAssociation( 0xFB, "\"\xFB\" add_text" ),
    KeyboardAssociation( 0xFC, "\"\xFC\" add_text" ),
    KeyboardAssociation( 0xFD, "\"\xFD\" add_text" ),
    KeyboardAssociation( 0xFE, "\"\xFE\" add_text" ),
    KeyboardAssociation( 0xFF, "\"\xFF\" add_text" ),

    // Command keys on the IBM PC keyboard.
    KeyboardAssociation( scr::K_F1     , "find_file" ),
    KeyboardAssociation( scr::K_F2     , "save_file" ),
    KeyboardAssociation( scr::K_F3     , "next_file" ),
    KeyboardAssociation( scr::K_F4     , "remove_file" ),
    KeyboardAssociation( scr::K_F5     , "toggle_block" ),
    KeyboardAssociation( scr::K_F6     , "cut" ),
    KeyboardAssociation( scr::K_F7     , "paste" ),
    KeyboardAssociation( scr::K_F8     , "insert_file" ),
    KeyboardAssociation( scr::K_F9     , "goto_line" ),
    KeyboardAssociation( scr::K_F10    , "external_command" ),
    KeyboardAssociation( scr::K_SF1    , "help" ),
    KeyboardAssociation( scr::K_SF2    , "editor_info" ),
    KeyboardAssociation( scr::K_SF3    , "legal_info" ),
    KeyboardAssociation( scr::K_SF4    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF5    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF6    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF7    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF8    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF9    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_SF10   , "redirect_to" ),
    KeyboardAssociation( scr::K_CF1    , "search_first" ),
    KeyboardAssociation( scr::K_CF2    , "search_next" ),
    KeyboardAssociation( scr::K_CF3    , "search_replace" ),
    KeyboardAssociation( scr::K_CF4    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_CF5    , "set_mark" ),
    KeyboardAssociation( scr::K_CF6    , "toggle_mark" ),
    KeyboardAssociation( scr::K_CF7    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_CF8    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_CF9    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_CF10   , "redirect_from" ),
    KeyboardAssociation( scr::K_AF1    , "refresh_file" ),
    KeyboardAssociation( scr::K_AF2    , "rename_file" ),
    KeyboardAssociation( scr::K_AF3    , "previous_file" ),
    KeyboardAssociation( scr::K_AF4    , "kill_file" ),
    KeyboardAssociation( scr::K_AF5    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_AF6    , "copy" ),
    KeyboardAssociation( scr::K_AF7    , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_AF8    , "file_insert" ),
    KeyboardAssociation( scr::K_AF9    , "goto_column" ),
    KeyboardAssociation( scr::K_AF10   , "external_filter" ),
    KeyboardAssociation( scr::K_HOME   , "start_of_line" ),
    KeyboardAssociation( scr::K_END    , "end_of_line" ),
    KeyboardAssociation( scr::K_PGUP   , "page_up" ),
    KeyboardAssociation( scr::K_PGDN   , "page_down" ),
    KeyboardAssociation( scr::K_LEFT   , "cursor_left" ),
    KeyboardAssociation( scr::K_RIGHT  , "cursor_right" ),
    KeyboardAssociation( scr::K_UP     , "cursor_up" ),
    KeyboardAssociation( scr::K_DOWN   , "cursor_down" ),
    KeyboardAssociation( scr::K_INS    , "toggle_replace" ),
    KeyboardAssociation( scr::K_DEL    , "delete" ),
    KeyboardAssociation( scr::K_CHOME  , "top_of_file" ),
    KeyboardAssociation( scr::K_CEND   , "end_of_file" ),
    KeyboardAssociation( scr::K_CPGUP  , "previous_procedure" ),
    KeyboardAssociation( scr::K_CPGDN  , "next_procedure" ),
    KeyboardAssociation( scr::K_CLEFT  , "word_left" ),
    KeyboardAssociation( scr::K_CRIGHT , "word_right" ),
    KeyboardAssociation( scr::K_ALTA   , "add_text" ),
    KeyboardAssociation( scr::K_ALTB   , "background_color" ),
    KeyboardAssociation( scr::K_ALTC   , "execute_file" ),
    KeyboardAssociation( scr::K_ALTD   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTE   , "error_message" ),
    KeyboardAssociation( scr::K_ALTF   , "foreground_color" ),
    KeyboardAssociation( scr::K_ALTG   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTH   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTI   , "input" ),
    KeyboardAssociation( scr::K_ALTJ   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTK   , "define_key" ),
    KeyboardAssociation( scr::K_ALTL   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTM   , "execute_macro" ),
    KeyboardAssociation( scr::K_ALTN   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTO   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTP   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTQ   , "quit" ),
    KeyboardAssociation( scr::K_ALTR   , "reformat_paragraph" ),
    KeyboardAssociation( scr::K_ALTS   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTT   , "set_tab" ),
    KeyboardAssociation( scr::K_ALTU   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTV   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTW   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALTX   , "exit" ),
    KeyboardAssociation( scr::K_ALTY   , "yexit" ),
    KeyboardAssociation( scr::K_ALTZ   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT1   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT2   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT3   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT4   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT5   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT6   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT7   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT8   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT9   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( scr::K_ALT0   , "\"Command Unknown\" error_message" ),
    KeyboardAssociation( -1, "" )
};

// The order of the names in this array must match the keys in the array above.
//
static const char *const key_names[] = {
    "K_NULL",  "K_CTRLA",  "K_CTRLB",     "K_CTRLC",  "K_CTRLD",  "K_CTRLE",
    "K_CTRLF", "K_CTRLG",  "K_BACKSPACE", "K_TAB",    "K_CTRLJ",  "K_CTRLK",
    "K_CTRLL", "K_RETURN", "K_CTRLN",     "K_CTRLO",  "K_CTRLP",  "K_CTRLQ",
    "K_CTRLR", "K_CTRLS",  "K_CTRLT",     "K_CTRLU",  "K_CTRLV",  "K_CTRLW",
    "K_CTRLX", "K_CTRLY",  "K_CTRLZ",     "K_ESC",    "K_CTRL\\", "K_CTRL`",
    "K_CTRL=", "K_CTRL-",  "K_SPACE",     "K_!",      "K_\"",     "K_#",
    "K_$",     "K_%",      "K_&",         "K_'",      "K_(",      "K_)",
    "K_*",     "K_+",      "K_,",         "K_-",      "K_.",      "K_/",
    "K_0",     "K_1",      "K_2",         "K_3",      "K_4",      "K_5",
    "K_6",     "K_7",      "K_8",         "K_9",      "K_0",      "K_:",
    "K_;",     "K_<",      "K_>",         "K_?",      "K_@",      "K_A",
    "K_B",     "K_C",      "K_D",         "K_E",      "K_F",      "K_G",
    "K_H",     "K_I",      "K_J",         "K_K",      "K_L",      "K_M",
    "K_N",     "K_O",      "K_P",         "K_Q",      "K_R",      "K_S",
    "K_T",     "K_U",      "K_V",         "K_W",      "K_X",      "K_Y",
    "K_Z",     "K_[",      "K_\\",        "K_]",      "K_^",      "K__",
    "K_`",     "K_a",      "K_b",         "K_c",      "K_d",      "K_e",
    "K_f",     "K_g",      "K_h",         "K_i",      "K_j",      "K_k",
    "K_l",     "K_m",      "K_n",         "K_o",      "K_p",      "K_q",
    "K_r",     "K_s",      "K_t",         "K_u",      "K_v",      "K_w",
    "K_x",     "K_y",      "K_z",         "K_{",      "K_|",      "K_}",
    "K_~",     "K_CRETURN","K_128",       "K_129",    "K_130",    "K_131",
    "K_132",   "K_133",    "K_134",       "K_135",    "K_136",    "K_137",
    "K_138",   "K_139",    "K_140",       "K_141",    "K_142",    "K_143",
    "K_144",   "K_145",    "K_146",       "K_147",    "K_148",    "K_149",
    "K_150",   "K_151",    "K_152",       "K_153",    "K_154",    "K_155",
    "K_156",   "K_157",    "K_158",       "K_159",    "K_160",    "K_161",
    "K_162",   "K_163",    "K_164",       "K_165",    "K_166",    "K_167",
    "K_168",   "K_169",    "K_170",       "K_171",    "K_172",    "K_173",
    "K_174",   "K_175",    "K_176",       "K_177",    "K_178",    "K_179",
    "K_180",   "K_181",    "K_182",       "K_183",    "K_184",    "K_185",
    "K_186",   "K_187",    "K_188",       "K_189",    "K_190",    "K_191",
    "K_192",   "K_193",    "K_194",       "K_195",    "K_196",    "K_197",
    "K_198",   "K_199",    "K_200",       "K_201",    "K_202",    "K_203",
    "K_204",   "K_205",    "K_206",       "K_207",    "K_208",    "K_209",
    "K_210",   "K_211",    "K_212",       "K_213",    "K_214",    "K_215",
    "K_216",   "K_217",    "K_218",       "K_219",    "K_220",    "K_221",
    "K_222",   "K_223",    "K_224",       "K_225",    "K_226",    "K_227",
    "K_228",   "K_229",    "K_230",       "K_231",    "K_232",    "K_233",
    "K_234",   "K_235",    "K_236",       "K_237",    "K_238",    "K_239",
    "K_240",   "K_241",    "K_242",       "K_243",    "K_244",    "K_245",
    "K_246",   "K_247",    "K_248",       "K_249",    "K_250",    "K_251",
    "K_252",   "K_253",    "K_254",       "K_255",    "K_F1",     "K_F2",
    "K_F3",    "K_F4",     "K_F5",        "K_F6",     "K_F7",     "K_F8",
    "K_F9",    "K_F10",    "K_SF1",       "K_SF2",    "K_SF3",    "K_SF4",
    "K_SF5",   "K_SF6",    "K_SF7",       "K_SF8",    "K_SF9",    "K_SF10",
    "K_CF1",   "K_CF2",    "K_CF3",       "K_CF4",    "K_CF5",    "K_CF6",
    "K_CF7",   "K_CF8",    "K_CF9",       "K_CF10",   "K_AF1",    "K_AF2",
    "K_AF3",   "K_AF4",    "K_AF5",       "K_AF6",    "K_AF7",    "K_AF8",
    "K_AF9",   "K_AF10",   "K_HOME",      "K_END",    "K_PGUP",   "K_PGDN",
    "K_LEFT",  "K_RIGHT",  "K_UP",        "K_DOWN",   "K_INS",    "K_DEL",
    "K_CHOME", "K_CEND",   "K_CPGUP",     "K_CPGDN",  "K_CLEFT",  "K_CRIGHT",
    "K_ALTA",  "K_ALTB",   "K_ALTC",      "K_ALTD",   "K_ALTE",   "K_ALTF",
    "K_ALTG",  "K_ALTH",   "K_ALTI",      "K_ALTJ",   "K_ALTK",   "K_ALTL",
    "K_ALTM",  "K_ALTN",   "K_ALTO",      "K_ALTP",   "K_ALTQ",   "K_ALTR",
    "K_ALTS",  "K_ALTT",   "K_ALTU",      "K_ALTV",   "K_ALTW",   "K_ALTX",
    "K_ALTY",  "K_ALTZ",   "K_ALT1",      "K_ALT2",   "K_ALT3",   "K_ALT4",
    "K_ALT5",  "K_ALT6",   "K_ALT7",      "K_ALT8",   "K_ALT9",   "K_ALT0",
    "K_ALTDASH",  "K_ALTEQU",  "K_CTRL_PRTSC",
    nullptr
};

//=========================================================================

void modify_key_association( const char *const key_name, const char *const new_macro_text )
{
    // Search the key name array looking for a matching name.
    int index;
    for( index = 0; key_names[index] != nullptr; index++ )
        if( std::strcmp( key_name, key_names[index] ) == 0 ) break;

    // If we didn't find it, print an error message.
    if( key_names[index] == nullptr ) {
        error_message( "The key name \"%s\" is unrecognized", key_name );
    }
    
    // Otherwise, make the modification.
    else {
        keyboard_map[index].macro_text.erase( );
        keyboard_map[index].macro_text.append( new_macro_text );
    }
}


bool KeyboardWord::get_word( EditBuffer &word )
{
    std::string words;

    // Return a null word to force the main loop to fetch from the new object.
    word.erase( );

    // Get a keystroke from the KeyHandler. Note that the KeyHandler automatically deals with
    // repeat sequences, keyboard macros, and the like. Also get_key( ) will update the screen.
    const int ch = KeyHandler::get_key( );

    // If this is a quoted keystroke, compose a special macro right here. Do NOT search the
    // keyboard mapping table.
    if( ch & 0x8000 ) {
        std::ostringstream formatter;
        formatter << '"' << static_cast<char>( ch ) << "\" ADD_TEXT" << std::ends;
        words = formatter.str( );
        // TODO: How does Open Watcom deallocate the buffer inside the ostrstream object?
        //formatter.freeze( false );
    }

    else {
        // Search the keyboard mapping table. See if we can locate this key.
        const KeyboardAssociation *search = keyboard_map;
        while( search->key_code != -1 ) {
            if( search->key_code == ch ) break;
            search++;
        }

        // If we didn't find it, complain.
        if( search->key_code == -1 ) {
            error_message( "Unknown Keystroke" );
            return true;
            // Let the caller think this worked, so they won't pop the stack!
        }

        words = search->macro_text.to_string( );
    }
      
    // Now let's create a new StringWord containing the macro.
    StringWord *const new_source = new StringWord( words.c_str( ) );

    // Push it onto the word stack for later.
    macro_stack.push( new_source );

    // Don't let this object get popped from the stack!
    return true;
}
