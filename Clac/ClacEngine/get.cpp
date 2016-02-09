/*! \file    get.cpp
 *  \brief   This file contains the implementation of the general Get functions.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

#include "Rational.hpp"

#include "BinaryEntity.hpp"
#include "ComplexEntity.hpp"
#include "FloatEntity.hpp"
#include "IntegerEntity.hpp"
#include "LabeledEntity.hpp"
#include "ListEntity.hpp"
#include "MatrixEntity.hpp"
#include "RationalEntity.hpp"
#include "StringEntity.hpp"
#include "support.hpp"

#include "get.hpp"
#include "global.hpp"
#include "words.hpp"

//
// Each of the following helper functions knows how to read the given entity type.
//
static BinaryEntity   *get_binary  ( const string &s );
static ComplexEntity  *get_complex ( const string &s );
static FloatEntity    *get_float   ( const string &s );
static IntegerEntity  *get_integer ( const string &s );
static ListEntity     *get_list    ( const string &s );
static MatrixEntity   *get_matrix  ( const string &s );
static RationalEntity *get_rational( const string &s );
static StringEntity   *get_string  ( const string &s );

static bool is_special_word( const string &word )
{
    static const char * const special_words[] = {
        "pi", "e", "j", "i", NULL
    };

    const char * const *current_word = special_words;
    while( *current_word != NULL ) {
        if( word == *current_word ) return true;
        current_word++;
    }
    return false;
}

static Entity *get_special_word( const string &word )
{
    Entity *result = NULL;
    if( word == "pi" ) {
        result = new FloatEntity( 3.141592653589793238462643 );
    }
    else if( word == "e" ) {
        result = new FloatEntity( 2.718281828459045235360287 );
    }
    else if( word == "i" ) {
        result = new ComplexEntity( 0.0, 1.0 );
    }
    else if( word == "j" ) {
        result = new ComplexEntity( 0.0, 1.0 );
    }
    return result;
}


/*!
 * The following function returns a pointer to a newly constructed Entity formed by the given
 * word. Since some entities require several words to be read from the input stream, this
 * function will read additional words as needed. It returns NULL if it can't form a valid
 * Entity.
 */
Entity *get_entity( WordStream &word_source )
{
    Entity *return_value = NULL;
    string word = word_source.next_word( );

    switch( word[0] ) {

    case '{' :
        return_value = get_list( word );    break;

    case '#' :
        return_value = get_binary( word );  break;

    case '(' :
        return_value = get_complex( word ); break;

    case '"' :
        return_value = get_string( word );  break;

    case '[' :
        return_value = get_matrix( word );  break;

    default :

        // The other Entities are harder to recognize...
        if( is_rational( word.c_str( ) ) )
            return_value = get_rational( word );

        else if( is_integer( word.c_str( ) ) )
            return_value = get_integer( word );

        else if( is_float( word.c_str( ) ) )
            return_value = get_float( word );

        else {
//            LabeledEntity *item;
            if( word[0] == '\'' ) {
                word = word.substr( 1 );
                return_value = get_string( word );
            }
// [This code pertains to the handling of directory entries... currently not implemented]
//            else if( ( item = global::get_root( ).lookup( word ) ) != NULL ) {
//                Entity *directory_element = *item;
//                return_value = directory_element->duplicate( );
//            }
            else if( is_special_word( word.c_str( ) ) ) {
                return_value = get_special_word( word );
            }
            else {
                return_value = get_string( word );
            }
        }
    }
    return return_value;
}


/*!
 * The following function creates a new IntegerEntity. This function assumes that the give word
 * consists entirely of digit characters with a possible leading sign (no spaces).
 */
IntegerEntity *get_integer( const string &word )
{
    string workspace( word );
    int sign_flag = 1;    // Assume the number is positive.

    // If a negative sign found, handle it.
    if( workspace[0] == '-' ) {
        sign_flag = -1;
        workspace = workspace.substr( 1 );
    }

    // If a plus sign found, ignore it.
    else if( workspace[0] == '+' ) {
        workspace = workspace.substr( 1 );
    }

    VeryLong value( workspace );
    if( sign_flag == -1 ) value = -value;
    return new IntegerEntity( value );
}


/*!
 * The following function creates a new FloatEntity. This function assumes that the given word
 * satisfies is_float( ).
 */
FloatEntity *get_float( const string &word )
{
    double result;

    sscanf( word.c_str( ), "%lf", &result );
    return new FloatEntity( result );
}


/*!
 * The following function creates a new RationalEntity. This function assumes that the given
 * word satisfies is_rational( ).
 */
RationalEntity *get_rational( const string &word )
{
    string workspace( word );
    int sign_flag = 1;    // Assume that number is positive.

    // If a negative sign found, handle it.
    if( workspace[0] == '-' ) {
        sign_flag = -1;
        workspace = workspace.substr( 1 );
    }

    // If a positive sign found, ignore it.
    else if( workspace[0] == '+' ) {
        workspace = workspace.substr( 1 );
    }

    // Split off the top and bottom parts of the rational, etc...
    // TODO: Converting the return value of c_str() to char* is a hack. This should be rewritten in a better way.
    char *numerator   = strtok( const_cast<char *>(workspace.c_str( )), "/" );
    char *denominator = strtok( NULL, "/" );

    VeryLong vl_numerator( numerator );
    VeryLong vl_denominator( denominator );
    if( sign_flag == -1 ) vl_numerator = -vl_numerator;

    return new RationalEntity( Rational< VeryLong >( vl_numerator, vl_denominator ) );
}


/*!
 * The following function creates a new StringEntity. If the given word is surrounded by
 * quotation marks, the marks are removed. Note that if there is no trailing mark, this function
 * has no problems.
 */
StringEntity *get_string( const string &word )
{
    string workspace( word );
    const char *end_pointer;

    // Get rid of quotation marks if there are any.
    if( workspace[0] == '"' ) {
        workspace = workspace.substr( 1 );
        if( ( end_pointer = strchr( workspace.c_str( ), '"' ) ) != NULL )
            workspace = workspace.substr(0, workspace.length( ) - 1 );
    }

    return new StringEntity( workspace );
}


/*!
 * The following function creates a new ComplexEntity.
 */
ComplexEntity *get_complex( const string &word )
{
    double first_part = 0.0,
           second_part = 0.0;           // Angle or imaginary magnitude.
    char working_buffer[128+1];

    strcpy( working_buffer, word.c_str( ) );
    while( strchr( working_buffer, ')' ) == NULL ) {
        string word_buffer;

        word_buffer = global::word_source( ).next_word( );
        strcat( working_buffer, " " );
        strcat( working_buffer, word_buffer.c_str( ) );
    }
    if( !is_complex( working_buffer ) ) {
        error_message( "%s is an invalid complex number", working_buffer );
        return NULL;
    }
    int format_flag = 1;              // Check for polar or rectangular mode.
    if( strchr( working_buffer, '@' ) != NULL )
        format_flag = -1;

    // Scan working_buffer for anything but these 5 characters.
    char *number = strtok( working_buffer, "(), @" );

    sscanf( number, "%lf", &first_part );
    number = strtok( NULL, "(), @" );
    if( number != NULL )
        sscanf( number, "%lf", &second_part );
    if( format_flag == 1 )
        return new ComplexEntity( first_part, second_part );

    second_part = to_radians( second_part );
    return new ComplexEntity( first_part * cos( second_part ),
                              first_part * sin( second_part ) );
}

/*!
 * The following function creates a new BinaryEntity.
 */
BinaryEntity *get_binary( const string &word )
{
    string workspace( word );
    string word_buffer;

    workspace = workspace.substr( 1 );         // Skip the '#'.
    if( workspace.length( ) == 0 ) {
        word_buffer = global::word_source( ).next_word( );
    }
    else
        word_buffer = workspace;

    global::BaseType input_base = global::get_base( );

    string::size_type trailing = word_buffer.length( ) - 1;
    switch( word_buffer[trailing] ) {
      case 'h':
        input_base = global::HEX;
        break;
      case 'b':
        input_base = global::BINARY;
        break;
      case 'd':
        input_base = global::DECIMAL;
        break;
      case 'o': case 'q':
        input_base = global::OCTAL;
        break;
    }
    unsigned long value = 0UL;
    string value_buffer( word_buffer.substr( 0, trailing ) );
    const char *pointer = value_buffer.c_str( );

    bool error = false;
    while( *pointer && !error ) {
        switch( input_base ) {
        case global::DECIMAL:
            if( !isdigit( *pointer ) )
                error = true;
            else {
                value *= 10;
                value += *pointer - '0';
            }
            break;
        case global::BINARY:
            if( !( *pointer == '0' || *pointer == '1' ) )
                error = true;
            else {
                value *= 2;
                value += *pointer - '0';
            }
            break;
        case global::OCTAL:
            if( !isdigit( *pointer ) || *pointer == '8' || *pointer == '9' )
                error = true;
            else {
                value *= 8;
                value += *pointer - '0';
            }
            break;
        case global::HEX:
            if( !( isdigit( *pointer ) || ( *pointer >= 'A' && *pointer <= 'F' ) ) )
                error = true;
            else {
                value *= 16;
                if( !isdigit( *pointer ) )
                    value += *pointer - 'A' + 10;
                else
                    value += *pointer - '0';
            }
            break;
        }
        pointer++;
    }
    if( error ) {
        error_message( "%s is not a legal binary in the selected base", word_buffer.c_str( ) );
        return NULL;
    }
    else
        return new BinaryEntity( value );
  }


/*!
 * The following function creates a new ListEntity.
 */
ListEntity *get_list( const string &word )
{
    string      workspace( word );
    string      word_buffer;
    ListEntity *new_object =  new ListEntity;
    if( new_object == NULL )
        return NULL;

    workspace = workspace.substr( 1 );
    if( workspace.length( ) == 0 )
        word_buffer = global::word_source( ).next_word( );
    else
        word_buffer = workspace;

    while( word_buffer[0] != '}' ) {
        StringStream stream( word_buffer );
        Entity *list_element = get_entity( stream );
        if( list_element != NULL )
            new_object->plus( list_element );
        word_buffer = global::word_source( ).next_word( );
    }
    return new_object;
}


/*!
 * The following function creates a new MatrixEntity.
 */
MatrixEntity *get_matrix( const string &word )
{
    string workspace( word );
    string word_buffer;
    long   row_count    = 0;
    long   column_count = 0;
    bool   in_row       = false;

    MatrixEntity *new_object =  new MatrixEntity;
    if( new_object == NULL )
        return NULL;

    workspace = workspace.substr( 1 );
    if( workspace.length( ) == 0 )
        word_buffer = global::word_source( ).next_word( );
    else
        word_buffer = workspace;

    for( ;; ) {
        if( word_buffer[0] == '[' ) {
            if( in_row ) error_message( "Cannot make a matrix of matrices" );
            else in_row = true;
        }
        else if( word_buffer[0] == ']' ) {
            if( in_row ) {
                row_count++;
                column_count = 0;
                in_row = false;
            }
            else break;
        }
        else {
// [The Matrix implementation is not mature enough to support adding items to a matrix.]
//            StringStream stream( word_buffer );
//            Entity *matrix_element = get_entity( stream );
//            if( matrix_element != NULL )
//                new_object->install( matrix_element, row_count, column_count++ );
        }
        word_buffer = global::word_source( ).next_word( );
    }
    return new_object;
}
