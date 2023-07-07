/*! \file    keyboard.cpp
 *  \brief   Implementation of the keyboard handler.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstring>

#include "command.hpp"
#include "FileList.hpp"
#include "keyboard.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "YEditFile.hpp"

#define MAX_MACRO_LENGTH  256     // Max number of keystrokes in the keyboard macro.
#define MAX_NESTED_MACROS   4     // Max number of nested keyboard macros.

/*======================================*/
/*           Internal Classes           */
/*======================================*/

/*!
 * This class is used as a base for the different types of sources for keystrokes. The keyboard
 * handler maintains an array of pointers to this class to keep track of the currently active
 * source. This organization makes it easy to handle the interactions between the keyboard macro
 * and the repeat sequences.
 */
class KeyboardScript {
public:
    virtual ~KeyboardScript( ) { return; }
    virtual  int  get_keystroke( ) = 0;
    virtual  bool is_dynamic( )    = 0;
};


/*!
 * This class provides a basic source of keystrokes. It never returns the END indicator (-1).
 * One object of this type is installed at the base of the array of open keyboard sources. It is
 * never deleted.
 */
class NeverEndingSource : public KeyboardScript {
public:
    virtual int   get_keystroke( );
    virtual bool is_dynamic( ) { return false; }
};


/*!
 * This function gets a keystroke from a NeverEndingSource object. It is complicated by the
 * mouse handling. Mouse activity is detected and handled here in a way which is transparent to
 * the rest of the program. If the mouse is used to invoke a command, that command is directly
 * called (rather than just returning the corresponding command character). This is done to
 * allow the mouse-invoked commands to be ignored during keyboard macro learning, etc. Also, if
 * the keyboard is reconfigured, this code still works.
 */
int NeverEndingSource::get_keystroke( )
{
    // Display everytime a keystroke is obtained from a NeverEnding_Source.
    FileList::active_file().display();
    
    // Read a keystroke.
    int return_value = scr::key();

    // If this is a quoted character, turn on it's MSB!
    if( return_value == scr::K_CTRLQ ) return_value = scr::key( ) | 0x8000;
    return return_value;
}


/*!
 * This class deals with the repeat sequences (start with ^R).
 */
class RepeatSequence : public KeyboardScript {
private:
    int             repeat_key;      //!< The key to be repeated.
    int             repeat_count;    //!< The number of times to repeat.
    bool            get_count;       //!< =true when Count and Key have not been read.
    KeyboardScript *key_source;      //!< Where to get the Count and Key keystrokes.

public:
    RepeatSequence( KeyboardScript *source ) :
        get_count( true ), key_source( source ) { }
    
    virtual int get_keystroke( );
    virtual bool is_dynamic( ) { return true; }
};


/*!
 * This function returns keystrokes from a repeat sequence. The first time a request for a
 * keystroke is made from this object, it reads the source of keystrokes given to the object's
 * constructor to get a repeat count and key. The object will return keystrokes until the repeat
 * count reaches zero at which point it returns the END indicator (-1).
 */
int RepeatSequence::get_keystroke( )
{
    int return_value;

    // If no repeat_count and repeat_key yet, get them...
    if( get_count ) {
        int next_key;

        repeat_count = 0;

        // The following does not use isdigit( ). That function is thrown off by the large
        // keycodes returned by get_key( ).
        //
        while( next_key = key_source->get_keystroke( ), next_key >= '0' &&  next_key <= '9' ) {
            repeat_count = 10 * repeat_count + ( next_key - '0' );
        }
        repeat_key = next_key;
        get_count = false;
    }

    // If no repeats to do, return END indicator.
    if( repeat_count == 0 ) {
        return_value = -1;
    }

    // Otherwise, return an instance of the repeated key.
    else {
        repeat_count--;
        return_value = repeat_key;
    }

    return return_value;
}


/*!
 * This class stores and manages a learned keyboard macro. Unlike objects of type
 * RepeatSequence, objects of this type cannot be allocated dynamically in the same way. They
 * must be kept around so that a learned macro can be replayed several times.
 */
class KeyboardMacro : public KeyboardScript {
private:
    int  macro_buffer[MAX_MACRO_LENGTH]; //!< Contains text of the macro.
    int  macro_size;                     //!< Number of valid characters in macro.
    int  macro_index;                    //!< Index of next character to use.
    bool learning;                       //!< =true when recording a macro.
    bool learned;                        //!< =true when macro is defined.
    KeyboardScript *key_source;          //!< Source of keystrokes when recording.

public:
    KeyboardMacro( ) { learned = false; }

            void learn( KeyboardScript *source );   // Start recording.
            void prepare( );                        // Get ready to execute.
    virtual int  get_keystroke( );
    virtual bool is_dynamic( ) { return false; }
};


/*!
 * This function must be called to prepare a Keyboard_Macro for learning.
 */
void KeyboardMacro::learn(KeyboardScript *source)
{
    macro_size  = 0;
    macro_index = 0;
    learning    = true;
    key_source  = source;
    info_message( "Recording keyboard macro" );
    return;
}


/*!
 * This function must be called to prepare a Keyboard_Macro for execution.
 */
void KeyboardMacro::prepare( )
{
    macro_index = 0;
    learning    = false;
    return;
}


/*!
 * This function either learns a keyboard macro or executes one depending on which of learn() or
 * prepare() was called beforehand.
 */
int KeyboardMacro::get_keystroke( )
{
    int return_value;

    if( learning ) {
        return_value = key_source->get_keystroke( );
        if( return_value == scr::K_CTRLK ) {
            info_message( "Finished" );
            learning     = false;
            learned      = true;
            return_value = -1;
        }
        else if( return_value == scr::K_CTRLE ) {
            error_message( "Can't execute a keyboard macro recursively" );
            info_message( "Finished" );
            learning     = false;
            learned      = true;
            return_value = -1;
        }
        else if( macro_size == MAX_MACRO_LENGTH ) {
            warning_message( "Keyboard macro buffer is full" );
        }
        else {
            macro_buffer[macro_size++] = return_value;
        }
    }

    else if( !learned ) {
        error_message( "Keyboard macro is not defined" );
        return_value = -1;
    }

    else {
        if( macro_index == macro_size ) return_value = -1;
        else {
            return_value = macro_buffer[macro_index++];
        }
    }

    return return_value;
}

/*==================================*/
/*           Private Data           */
/*==================================*/


// The following data sets up a stack of keyboard source activation records. At the bottom of
// that stack is a never ending source of keystrokes from the standard input device. This stack
// greatly simplifies problems that arise from interactions between repeat sequences and the
// keyboard macro. It also sets the stage for multiple keyboard macros and macro files.
//
static NeverEndingSource standard_input;
static KeyboardScript   *activations[MAX_NESTED_MACROS] = { &standard_input };
static KeyboardScript  **current_script = &activations[0];
static KeyboardMacro     primary_macro;

/*=====================================================*/
/*           Member Functions of Key_Handler           */
/*=====================================================*/

namespace KeyHandler {

    /*!
     * This function gets a keystroke from the user. It waits until an acceptable keystroke is
     * received. It returns keycodes in the same form as returned by scr::key( ).
     */
    int get_key( )
    {
        int key_code;

        do {
            switch( key_code = ( *current_script )->get_keystroke( ) ) {
            case -1:
                if( ( *current_script )->is_dynamic( ) ) delete *current_script;
                current_script--;
                break;

            case scr::K_CTRLR: {
                    KeyboardScript *previous = *current_script;
                    *++current_script = new RepeatSequence( previous );
                    if( *current_script == NULL ) {
                        memory_message( "Can't process repeat request" );
                        current_script--;
                    }
                    key_code = -1;
                }
                break;

            case scr::K_CTRLK: {
                    KeyboardScript *previous = *current_script;
                    primary_macro.learn( previous );
                    *++current_script = &primary_macro;
                    key_code = -1;
                }
                break;

            case scr::K_CTRLE: {
                    primary_macro.prepare( );
                    *++current_script = &primary_macro;
                    key_code = -1;
                }
                break;
            }
        } while( key_code == -1 );

        return key_code;
    }

}
