/*! \file    clac.cpp
 *  \brief   Clac main program.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu> and Peter Nikolaidis
 *
 * LICENSE
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 *
 * Please send comments and bug reports to
 *
 *      Peter C. Chapin
 *      c/o Computer Information Systems Department
 *      Vermont Technical College
 *      Williston, VT 05495
 *      PChapin@vtc.vsc.edu
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <memory>

using namespace std;

// clac_entity library.
#include "BinaryEntity.hpp"
#include "FloatEntity.hpp"
#include "IntegerEntity.hpp"
#include "StringEntity.hpp"
#include "convert.hpp"
#include "support.hpp"

// clac_engine library.
#include "actions.hpp"
#include "get.hpp"
#include "global.hpp"
#include "words.hpp"

// scr library.
#include "debug.hpp"
#include "MessageWindow.hpp"
#include "scr.hpp"

// Application specific.
#include "ClacCommandWindow.hpp"
#include "DirectoryWindow.hpp"
#include "StackWindow.hpp"

//=====================================
//           Message Handling
//=====================================

static scr::MessageWindowDescriptor message_descriptors[] = {
    // For scr::MESSAGE_WINDOW_MESSAGE
    { scr::WHITE, scr::SINGLE_LINE, scr::WHITE, NULL, scr::WHITE, scr::MESSAGE_WINDOW_ANY },

    // For scr::MESSAGE_WINDOW_PROMPT
    { scr::WHITE, scr::SINGLE_LINE, scr::WHITE, NULL, scr::WHITE, scr::MESSAGE_WINDOW_ANY },

    // For scr::MESSAGE_WINDOW_WARNING
    { scr::WHITE, scr::SINGLE_LINE, scr::WHITE, NULL, scr::WHITE, scr::MESSAGE_WINDOW_ANY },

    // For scr::MESSAGE_WINDOW_ERROR
    { scr::WHITE, scr::SINGLE_LINE, scr::WHITE, NULL, scr::WHITE, scr::MESSAGE_WINDOW_ANY },

    // For scr::MESSAGE_WINDOW_INT_ERROR
    { scr::WHITE, scr::SINGLE_LINE, scr::WHITE, NULL, scr::WHITE, scr::MESSAGE_WINDOW_ANY }
};

//! Displays an error message.
/*!
 * This function is the master error message handler for all components of Clac. It is actually
 * declared in a low level header (support.hpp) in the Clac entity library. Any code that uses
 * Clac entities must provide an implementation of this function that is application specific.
 */
void error_message( const char *message, ... )
{
    static const char *insults[] = {
        "you blockhead",
        "you bozo",
        "you dimwit",
        "you dumb jerk",
        "you foolish human",
        "you idiot",
        "you ignorant fool",
        "you moron",
        "you moronic idiot",
        "you nimrod",
        "you stupid fool",
        0
    };
    const unsigned int insult_count = sizeof( insults ) / sizeof( char * );

    va_list ap;
    static bool do_seed = true;

    if( do_seed ) {
        time_t raw = time( 0 );
        srand( static_cast< unsigned int >( raw ) );
        do_seed = false;
    }

    va_start( ap, message );
    char message_buffer[128+1];
    vsprintf( message_buffer, message, ap );

    // Generate a random insult and append it to the error message.
    unsigned index = rand( ) % insult_count;
    strcat( message_buffer, " (" );
    strcat( message_buffer, insults[index] );
    strcat( message_buffer, ")" );

    // What happens to the message buffer anyway? It appears to go nowhere.
    scr::MessageWindow error_window( message_buffer, scr::MESSAGE_WINDOW_ERROR );
}

//===========================================
//           Program Initialization
//===========================================

/*!
 * Adjusts the format of __DATE__. Puts a comma after the day of the month and purge leading
 * zeros or spaces from the day of the month.
 *
 * \param ANSI_Date The date in the format given by the __DATE__ macros.
 * \return A pointer to a statically allocated buffer (of size 13) holding the cleaned up date.
 */
static char *AdjDate(const char *ANSI_Date)
{
    static char  buffer[13];
           char *buffer_pointer;

    // Make a working copy of the date as from the ANSI __DATE__ macro.
    strcpy( buffer, ANSI_Date );

    // Open up space for the comma.
    for( buffer_pointer  = strchr( buffer, '\0' );
         buffer_pointer >= &buffer[6];
         buffer_pointer-- ) {
        *(buffer_pointer+1) = *buffer_pointer;
    }

    // Put the comma in.
    buffer[6] = ',';

    // If this is a date from 1 to 9, close up the extra space.
    if( buffer[4] == '0' || buffer[4] == ' ' ) {
        for( buffer_pointer = &buffer[4]; *buffer_pointer; buffer_pointer++ ) {
            *buffer_pointer = *( buffer_pointer + 1 );
        }
    }

    // Return are result.
    return buffer;
}


class SetUp {
public:
    SetUp( bool use_debugger );
   ~SetUp( );
private:
    bool debugging_on;
};


SetUp::SetUp( bool use_debugger ) : debugging_on( false )
{
    scr::initialize( );
    scr::refresh_on_key( true );
    scr::MessageWindow::set_descriptors( message_descriptors );

    if( use_debugger ) {
        scr::initialize_debugging( DBG_TOP );
        debugging_on = true;
    }
    // TODO: Reload the calculator state (if there's a saved one to be found).
}


SetUp::~SetUp( )
{
    // Save the calculator state.
    if( debugging_on ) {
        scr::terminate_debugging( );
    }
    scr::terminate( );
    cout << "CLAC Version 0.00a  Compiled: " << AdjDate( __DATE__ ) << '\n'
         << "(C) Copyright 2013 by Peter Chapin and Peter Nikolaidis" << endl;
}

//=================================================
//           Generic Operation Functions
//=================================================

static void do_unary( Stack &the_stack, Entity *(Entity::*unary_operation)( ) const )
{
    // Get a pointer to the object on stack level zero.
    Entity *thing = the_stack.get( 0 );

    // If it didn't work, we have an error!
    if( thing == NULL ) underflow( );
    else {

        // Do the unary operation. If it works, we will get a pointer to a new object.
        Entity *new_thing = ( thing->*unary_operation )( );
        the_stack.put( new_thing );
    }
}

static void do_binary( Stack &the_stack, Entity *(Entity::*binary_operation)( const Entity * ) const )
{
    Entity *left  = the_stack.get( 1 );
    Entity *right = the_stack.get( 0 );

    if( left == NULL || right == NULL ) underflow( );
    else {
        Entity *(Entity::*conversion)( ) const =
            convert_table[left->my_type( )][right->my_type( )];

        if( conversion == NULL ) {
            error_message( "Required implicit conversion not implemented!" );
            return;
        }

        auto_ptr<Entity> new_left( (left->*conversion)( ) );
        auto_ptr<Entity> new_right( (right->*conversion)( ) );

        Entity *T1 = new_left.get( );
        Entity *T2 = new_right.get( );

        Entity *new_thing = (T1->*binary_operation)( T2 );

        Entity *old_level_zero = the_stack.pop( );
        Entity *old_level_one  = the_stack.pop( );
        delete old_level_zero;
        delete old_level_one;
        the_stack.push( new_thing );
    }
}

struct BuiltinBinary {
    const char  *word;
    Entity *(Entity::*binary_operation)( const Entity * ) const;
};

struct BuiltinUnary {
    const char  *word;
    Entity *(Entity::*unary_operation)( ) const;
};

struct BuiltinAction {
    const char  *word;
    void (*operation)( Stack & );
};

static BuiltinBinary binary_words[] = {
    { "+",      &Entity::plus              },
    { "-",      &Entity::minus             },
    { "*",      &Entity::multiply          },
    { "/",      &Entity::divide            },
    { "==",     &Entity::is_equal          },
    { "!=",     &Entity::is_notequal       },
    { ">",      &Entity::is_greater        },
    { ">=",     &Entity::is_greaterorequal },
    { "<",      &Entity::is_less           },
    { "<=",     &Entity::is_lessorequal    },
    { "mod",    &Entity::modulo            },
    { "^",      &Entity::power             },
    { NULL,      NULL                      }
};

static BuiltinUnary unary_words[] = {
    { "abs",    &Entity::abs               },
    { "acos",   &Entity::acos              },
    { "alog",   &Entity::exp10             },
    { "asin",   &Entity::asin              },
    { "atan",   &Entity::atan              },
    { "conj",   &Entity::complex_conjugate },
    { "cos",    &Entity::cos               },
    { "exp",    &Entity::exp               },
    { "frac",   &Entity::fractional_part   },
    { "im",     &Entity::imaginary_part    },
    { "inv",    &Entity::inv               },
    { "ln",     &Entity::ln                },
    { "log",    &Entity::log               },
    { "neg",    &Entity::neg               },
    { "re",     &Entity::real_part         },
    { "sgn",    &Entity::sign              },
    { "sin",    &Entity::sin               },
    { "sq",     &Entity::sq                },
    { "sqrt",   &Entity::sqrt              },
    { "tan",    &Entity::tan               },

    { ">bin",   &Entity::to_binary         },
    { ">cmplx", &Entity::to_complex        },
    { ">flt",   &Entity::to_float          },
    { ">int",   &Entity::to_integer        },
    { ">list",  &Entity::to_list           },
    { ">mat",   &Entity::to_matrix         },
    { ">rat",   &Entity::to_rational       },
    { ">str",   &Entity::to_string         },
    { NULL,      NULL                      }
};

static BuiltinAction action_words[] = {
    // Normal actions.
    { "bin",    do_bin      },
    { "clear",  do_clear    },
    { "dec",    do_dec      },
    { "deg",    do_deg      },
    { "drop",   do_drop     },
    { "dropn",  do_dropn    },
    { "dup",    do_dup      },
    { "dupn",   do_dupn     },
    { "eng",    do_eng      },
    { "eval",   do_eval     },
    { "fix",    do_fix      },
    { "grad",   do_grad     },
    { "hex",    do_hex      },
    { "oct",    do_oct      },
    { "polar",  do_polar    },
    { "purge",  do_purge    },
    { "rad",    do_rad      },
    { "read",   do_read     },
    { "rec",    do_rec      },
    { "roll" ,  do_roll_up  },
    { "rolld",  do_roll_down},
    { "rot",    do_rot      },
    { "run",    do_run      },
    { "sci",    do_sci      },
    { "sto",    do_store    },
    { "stws",   do_stws     },
    { "swap",   do_swap     },
    //{ "sys",    do_sys      },
    { "write",  do_write    },

    // Unary actions.
    { "sl",     do_shift_left   },
    { "sr",     do_shift_right  },
    //{ "asr",    do_ashift_right },

    // Off. Currently the 'quit' command is handled in a special way to ensure that the window
    // system is shut down properly. The other versions of "off" just produce a message telling
    // the user to use the 'quit' command.
    //
    { "exit",   do_off },
    { "off",    do_off },
    { "quit",   do_off },

    { NULL, NULL }
};

static bool process_binary( Stack &the_stack, const std::string &word_buffer )
{
    // Scan the list of builtin binary words.
    BuiltinBinary *bin_op = binary_words;
    while( bin_op->word != NULL ) {
        if( bin_op->word == word_buffer ) break;
        bin_op++;
    }

    // If we found it, do the operation.
    if( bin_op->word != NULL ) {
        do_binary( the_stack, bin_op->binary_operation );
        return true;
    }
    return false;
}

static bool process_unary( Stack &the_stack, const std::string &word_buffer )
{
    // Scan the list of built in unary words.
    BuiltinUnary *unary_op = unary_words;
    while( unary_op->word != NULL ) {
        if( unary_op->word == word_buffer ) break;
        unary_op++;
    }

    // If we found it, do the operation.
    if( unary_op->word != NULL ) {
        do_unary( the_stack, unary_op->unary_operation );
        return true;
    }
    return false;
}

static bool process_action( Stack &the_stack, const std::string &word_buffer )
{
    // Scan the list of builtin action words.
    BuiltinAction *action_op = action_words;
    while( action_op->word != NULL ) {
        if( action_op->word == word_buffer ) break;
        action_op++;
    }

    // If we found it, do the operation.
    if( action_op->word != NULL ) {
        action_op->operation( the_stack );
        return true;
    }
    return false;
  }


//! Process the words on the master stream, executing them one at a time.
/*!
 * This function continues working until the master stream is completely empty. Note that
 * some words cause new WordStream objects to be pushed onto the master stream. This function
 * continues until all of those streams are exhausted as well.
 *
 * \return true if the program should continue; false if the "quit" word was encountered.
 */
bool process_words( )
{
    scr::Tracer( "process_words", 1 );

    while (1) {
        try {
            std::string new_word( global::word_source( ).next_word( ) );
        
            // The master stream is exhausted.
            if( new_word.length( ) == 0 ) return true;

            // See if we got the null word. [Can this ever happen?]
            if( new_word[0] == '\0' ) {
                do_dup( global::the_stack( ) );
                continue;
            }

            // Should we quit?
            if( new_word == "quit" ) return false;

            // See if it is a built in word.
            if( process_binary( global::the_stack( ), new_word) ) continue;
            if( process_unary ( global::the_stack( ), new_word) ) continue;
            if( process_action( global::the_stack( ), new_word) ) continue;

            StringStream stream( new_word );
            Entity *new_object = get_entity( stream );
            if( new_object != NULL )
                global::the_stack( ).push( new_object );
        }
        catch( const char *the_message ) {
            error_message( "Exception: %s", the_message );
        }
        catch( const std::exception &e ) {
            error_message( "Exception: %s", e.what( ) );
        }
    }
}

//==================================
//           Main Program
//==================================

int Main( int argc, char **argv )
{
    bool use_debugger = false;

    // Command line analysis.
    // TODO: Improve and generalize the handling of the command line.
    for( int i = 1; i < argc; ++i ) {
        if( strcmp( argv[i], "-d" ) == 0 ) use_debugger = true;
    }

    SetUp the_program( use_debugger );
    const int screen_rows = scr::number_of_rows( );
    const int screen_cols = scr::number_of_columns( );
    const int half_width  = screen_cols / 2;

    scr::Manager window_manager;

    // Create windows.
    StackWindow stack_view(
        window_manager,  2,  2, half_width - 2, screen_rows - 5 );

    DirectoryWindow directory_view(
        window_manager,  2, half_width + 2, half_width - 2, screen_rows - 5 );

    ClacCommandWindow command_view(
        &window_manager, screen_rows - 1,  2, screen_cols - 2,  1 );

    // Initialize windows.
    stack_view.associate( &global::the_stack( ) );
    command_view.set_prompt( "=> " );

    // Interact with the user.
    window_manager.input_loop( );

    return 0;
}

//===================================
//           The real main
//===================================

int main( int argc, char **argv )
{
    // Set the default exit status in case an unhandled exception propagates through main.
    int return_value = EXIT_FAILURE;

    try {
        return_value = Main( argc, argv );
    }
    catch (...) {
        cerr << "Panic! Unhandled exception propagated through main()" << endl;
    }
    return return_value;
}
