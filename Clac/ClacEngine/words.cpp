/*! \file    words.cpp
    \brief   This file contains...
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cctype>

using namespace std;

#include "words.hpp"

static bool float_number(const char *&);
static bool floating_mantissa(const char *&);
static bool floating_number(const char *&);
static bool digit_list(const char *&);
static bool integer_mantissa(const char *&);
static bool exponent(const char *&);

static bool complex_number(const char *&);
static bool number(const char *&);
static bool white(const char *&);
static bool separator(const char *&);

bool is_complex( const char *word )
{
    if( complex_number( word ) == false ) return false;
    if( *word != '\0' ) return false;
    return true;
}

bool is_integer( const char *word )
{
    if( *word == '+' || *word == '-' ) word++;
    while( *word ) {
        if( !isdigit( *word ) ) return false;
        word++;
    }
    return true;
}

bool is_float( const char *word )
{
    if( float_number( word ) == false ) return false;
    if( *word != '\0' ) return false;
    return true;
}

bool is_rational( const char *word )
{
    if( *word == '-' ) word++;
    do {
        if( !isdigit( *word ) ) return false;
        word++;
    }
    while( *word != '/' );

    word++;
    do {
        if( !isdigit( *word ) ) return false;
        word++;
    } while( *word != '\0' );
    return true;
}


/*!
 * The following is the grammar used to accept or reject a string as a valid floating point
 * number. This grammar is implemented with a hand written recursive descent parser (below).
 *
 * <pre>
 * %token DIGIT
 * %token E
 * %token SIGN
 *
 * %%
 *
 * float_number:
 *         floating_mantissa
 *       | floating_mantissa exponent
 *       | integer_mantissa exponent
 *       ;
 *
 * floating_mantissa:
 *       SIGN floating_number
 *     | floating_number
 *     ;
 *
 * floating_number:
 *         digit_list '.'
 *       | digit_list '.' digit_list
 *       | '.' digit_list
 *       ;
 *
 * digit_list:
 *         DIGIT
 *       | DIGIT digit_list
 *       ;
 *
 * integer_mantissa:
 *         SIGN digit_list
 *       | digit_list
 *       ;
 *
 * exponent:
 *         E digit_list
 *       | E SIGN digit_list
 *       ;
 *
 * %%
 * </pre>
 */
bool float_number( const char *&p ) // A float_number could have an exponent
{                                   // (see also floating_number).
    const char *save_pointer = p;
    if( integer_mantissa( p ) && exponent( p ) )   // If the number is an int with
        return true;                               //  an exp: 1e3.
    else {
        p = save_pointer;                          // Go back to the start of list.
        if( !floating_mantissa( p ) )
            return false;
        else if( *p == 'e' || *p == 'E' )
            return exponent( p );
        else
            return true;
    }
}

bool floating_mantissa( const char *&p )
{
    if( *p == '+' || *p == '-' )                // If the number is signed,
        p++;                                    //  skip the sign
    return floating_number( p );                //  and return a float.
}

bool floating_number( const char *&p ) // A floating_number has no exponent.
{                                      // Ex: 3.14,  31.415, .31415
    if( *p == '.' ) {
        p++;
        return digit_list( p );
    }
    if( !digit_list( p ) )
        return false;
    if( *p !='.' )
        return false;
    p++;
    digit_list( p );
    return true;
}

bool integer_mantissa( const char *&p )
{
    if (*p == '+' || *p == '-')                 // If the number is signed,
        p++;                                    //  skip the sign
    return digit_list(p);                       //  and return a digit_list.
}

bool exponent( const char *&p )
{
    if(*p != 'e' && *p != 'E')                  // char is not e or E.
        return false;
    p++;                                        // Skip the e or E.
    if (*p == '+' || *p == '-')                 // Skip + or - signs.
        p++;
    return digit_list(p);
}

bool digit_list( const char *&p )
{
    if( !isdigit( *p ) )                        // If the char is not a digit.
        return false;
    while( isdigit( *p ) )                      // Scan through the list of
        p++;                                    //  digits.
    return true;
}

/*!
 * The following is the grammar used to accept or reject a string as a valid
 * complex number. This grammar is implemented with a hand written recursive
 * descent parser (below).
 *
 * <pre>
 * complex_number:
 *         OPARN white number white separator white number white CPARN
 *       | OPARN white number white number white CPARN
 *       | OPARN white number white CPARN
 *       ;
 *
 * number:
 *         float_number
 *       | integer_mantissa
 *       ;
 * </pre>
 */
bool number( const char *&p )
{
    const char *save_pointer = p;

    if( float_number( p ) )
        return true;
    p = save_pointer;
    if( integer_mantissa( p ) )
        return true;
    return false;
}

bool white( const char *&p )
{
    while( *p == ' ' ) p++;
    return true;
}

bool separator( const char *&p )
{
    if( *p == ',' || *p == '@' ) {
        p++;
        return true;
    }
    return false;
}

bool complex_number( const char *&p )
{
    if(*p != '(' ) return false;
    p++;
    white( p );
    
    if( !number( p ) ) return false;
    white( p );

    if( *p == ')' ) {
        p++;
        return true;
    }
    if( separator( p ) ) {
        white( p );

        if( !number( p ) ) return false;
        white( p );

        if( *p == ')' ) {
            p++;
            return true;
        }
        return false;
    }
    else {
        if( !number( p ) ) return false;
        white( p );
        if( *p == ')' ) {
            p++;
            return true;
        }
        return false;
    }
}

