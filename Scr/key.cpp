/*! \file    key.cpp
    \brief   Basic keyboard input support.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

These functions return processed keystrokes in a platform independent way. In addition to
returning normal letter keys, various special keys can also be detected. See the list of key
constants in scr.h.
*/

#include "environ.hpp"
#include <cctype>
#include <map>
#include <utility>

// Produce an error for alien operating systems.
#if !(eOPSYS == eDOS   || \
      eOPSYS == eWIN32 || \
      eOPSYS == eOS2   || \
      eOPSYS == ePOSIX || defined(SCR_ANSI))
#error Scr only supports DOS, OS/2, Win32, Unix (curses), or SCR_ANSI.
#endif

#if (eOPSYS == eWIN32)
#include <windows.h>
#endif

#if (eOPSYS == eDOS) || (eOPSYS == eOS2) || (eOPSYS == eWIN32)
// For the getch() function. Used by key().
#include <conio.h>
#endif

#if eOPSYS == ePOSIX
// We need curses for getch().
#define NOMACROS
#define NCURSES_NOMACROS
#include <ncurses.h>
#endif

#include "scr.hpp"

static bool key_refresh = false;

namespace scr {

    /*! \fn void scr::initialize_key( )
     *
     * Called during the initialization of scr to initialize the keyboard handling part of the
     * library. This function should not be called by application code.
     */

     /*! \fn void scr::terminate_key( )
      *
      * Called during the termination of scr to clean up the keyboard handling part of the
      * library. This function should not be called by application code.
      */

    /*! \fn int scr::key( )
     *
     * If the "refresh when waiting for keystroke" flag is set then this function calls
     * scr::refresh. Either way it then calls scr::KeyWait.
     *
     * \brief Get a keystroke.
     *
     * \return The keycode of the key the user pressed. Special keys are returned using extended
     * keycodes.
     */

    /*! \fn void scr::refresh_on_key( bool flag )
     *
     * By default the screen is only refreshed when scr::refresh is called. However, if this
     * feature is turned on then scr::refresh is automatically called whenever scr::Key is
     * called. This insures that the physical display is updated whenever the user is expected
     * to provide keyboard input.
     *
     * \brief Sets the "refresh when waiting for keystroke" flag.
     *
     * \param flag <b>true</b> to turn on the feature; <b>false</b> otherwise.
     */

    /*! \fn int scr::key_wait( )
     *
     * This function waits for the user to press a key. It ignores the "refresh when waiting for
     * keystroke" flag. Note that this function does not echo the keystroke on the screen. Your
     * application must do that if it is necessary.
     *
     * \brief Wait for a keystroke.
     *
     * \return The keycode of the key the user pressed. Special keys are returned using extended
     * keycodes.
     */

#if defined(SCR_ASCIIKEYS) || eOPSYS == ePOSIX

#if eOPSYS == ePOSIX
    typedef std::pair<const int, int> KeyPair;
    typedef std::map< int, int, std::less<int> > KeyMap;

    static KeyMap curses_key_map;   // Maps curses special keys to portscr.

    static void initialize_key_map( )
    {
        static KeyPair key_associations[] = {
            KeyPair(KEY_F(1),      K_F1),
            KeyPair(KEY_F(2),      K_F2),
            KeyPair(KEY_F(3),      K_F3),
            KeyPair(KEY_F(4),      K_F4),
            KeyPair(KEY_F(5),      K_F5),
            KeyPair(KEY_F(6),      K_F6),
            KeyPair(KEY_F(7),      K_F7),
            KeyPair(KEY_F(8),      K_F8),
            KeyPair(KEY_F(9),      K_F9),
            KeyPair(KEY_F(10),     K_F10),

            KeyPair(KEY_F(11),     K_SF1),
            KeyPair(KEY_F(12),     K_SF2),
            KeyPair(KEY_F(13),     K_SF3),
            KeyPair(KEY_F(14),     K_SF4),
            KeyPair(KEY_F(15),     K_SF5),
            KeyPair(KEY_F(16),     K_SF6),
            KeyPair(KEY_F(17),     K_SF7),
            KeyPair(KEY_F(18),     K_SF8),
            KeyPair(KEY_F(19),     K_SF9),
            KeyPair(KEY_F(20),     K_SF10),

            KeyPair(KEY_F(21),     K_CF1),
            KeyPair(KEY_F(22),     K_CF2),
            KeyPair(KEY_F(23),     K_CF3),
            KeyPair(KEY_F(24),     K_CF4),
            KeyPair(KEY_F(25),     K_CF5),
            KeyPair(KEY_F(26),     K_CF6),
            KeyPair(KEY_F(27),     K_CF7),
            KeyPair(KEY_F(28),     K_CF8),
            KeyPair(KEY_F(29),     K_CF9),
            KeyPair(KEY_F(30),     K_CF10),

            KeyPair(KEY_F(31),     K_AF1),
            KeyPair(KEY_F(32),     K_AF2),
            KeyPair(KEY_F(33),     K_AF3),
            KeyPair(KEY_F(34),     K_AF4),
            KeyPair(KEY_F(35),     K_AF5),
            KeyPair(KEY_F(36),     K_AF6),
            KeyPair(KEY_F(37),     K_AF7),
            KeyPair(KEY_F(38),     K_AF8),
            KeyPair(KEY_F(39),     K_AF9),
            KeyPair(KEY_F(40),     K_AF10),

            KeyPair(KEY_LEFT,      K_LEFT),
            KeyPair(KEY_RIGHT,     K_RIGHT),
            KeyPair(KEY_UP,        K_UP),
            KeyPair(KEY_DOWN,      K_DOWN),
            KeyPair(KEY_NPAGE,     K_PGDN),
            KeyPair(KEY_PPAGE,     K_PGUP),
            KeyPair(KEY_HOME,      K_HOME),
            KeyPair(KEY_END,       K_END),
            KeyPair(KEY_IC,        K_INS),
            KeyPair(KEY_DC,        K_DEL),
            KeyPair(KEY_BACKSPACE, K_BACKSPACE),
            KeyPair(KEY_ENTER,     K_RETURN),
            KeyPair(KEY_SHOME,     K_CHOME),
            KeyPair(KEY_SEND,      K_CEND),
            KeyPair(KEY_SNEXT,     K_CPGDN),
            KeyPair(KEY_SPREVIOUS, K_CPGUP),
            KeyPair(KEY_SLEFT,     K_CLEFT),
            KeyPair(KEY_SRIGHT,    K_CRIGHT)
        };

        curses_key_map.insert(
            key_associations, key_associations + sizeof( key_associations )/sizeof( KeyPair ) );
   }

#endif


    // Remap the control keys to the special movement keys.
    static int control_translation[] = {
    '*', K_CTRLA,  K_LEFT ,  K_CTRLC,  K_CHOME,  K_CTRLE,  K_RIGHT,  K_CEND ,
         K_CTRLH,  K_CTRLI,  K_CTRLJ,  K_CLEFT,  K_CRIGHT, K_CTRLM,  K_DOWN ,
         K_CPGDN,  K_UP   ,  K_CTRLQ,  K_END  ,  K_CTRLS,  K_CTRLT,  K_CPGUP,
         K_PGDN ,  K_HOME ,  K_DEL  ,  K_INS  ,  K_PGUP
    };

    // Mappings from normal letters to the K_ALT... codes.
    static int alt_translation[] = {
        K_ALTA, K_ALTB, K_ALTC, K_ALTD, K_ALTE, K_ALTF, K_ALTG, K_ALTH, K_ALTI,
        K_ALTJ, K_ALTK, K_ALTL, K_ALTM, K_ALTN, K_ALTO, K_ALTP, K_ALTQ, K_ALTR,
        K_ALTS, K_ALTT, K_ALTU, K_ALTV, K_ALTW, K_ALTX, K_ALTY, K_ALTZ
    };

    // Mappings from the number keys to the K_ALT... codes.
    static int alt_number_translation[] = {
        K_ALT0, K_ALT1, K_ALT2, K_ALT3, K_ALT4, K_ALT5, K_ALT6, K_ALT7, K_ALT8, K_ALT9
    };

    // Mappings from the number keys to various types of function key codes.
    static int function_translation[] = {
        K_F10, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9
    };

    static int shift_function_translation[] = {
        K_SF10, K_SF1, K_SF2, K_SF3, K_SF4, K_SF5, K_SF6, K_SF7, K_SF8, K_SF9
    };

    static int control_function_translation[] = {
        K_CF10, K_CF1, K_CF2, K_CF3, K_CF4, K_CF5, K_CF6, K_CF7, K_CF8, K_CF9
    };

    static int alt_function_translation[] = {
        K_AF10, K_AF1, K_AF2, K_AF3, K_AF4, K_AF5, K_AF6, K_AF7, K_AF8, K_AF9
    };

#if eOPSYS != ePOSIX
    struct KeyEscape {
        char *escape_sequence;
        int   key_code;
    };

    static KeyEscape special_key_escapes[] = {
        { "\033",   K_ESC  }, // Two ESC in a row becomes a single ESC.

        { "[A",     K_UP   },
        { "[B",     K_DOWN },
        { "[C",     K_RIGHT},
        { "[D",     K_LEFT },

        { "[H",     K_HOME },
        { "[F",     K_END  },
        { "[2~",    K_INS  },
        { "[3~",    K_DEL  },
        { "[5~",    K_PGUP },
        { "[6~",    K_PGDN },

        { "OP",     K_F1   },
        { "OQ",     K_F2   },
        { "OR",     K_F3   },
        { "OS",     K_F4   },
        { "[15~",   K_F5   },
        { "[17~",   K_F6   },
        { "[18~",   K_F7   },
        { "[19~",   K_F8   },
        { "[20~",   K_F9   },
        { "[21~",   K_F10  },

        { "O2P",    K_SF1  },
        { "O2Q",    K_SF2  },
        { "O2R",    K_SF3  },
        { "O2S",    K_SF4  },
        { "[15;2~", K_SF5  },
        { "[17;2~", K_SF6  },
        { "[18;2~", K_SF7  },
        { "[19;2~", K_SF8  },
        { "[20;2~", K_SF9  },
        { "[21;2~", K_SF10 },

        { NULL, 0 }
    };
#endif


    void initialize_key( )
    {
#if eOPSYS == ePOSIX
        initialize_key_map( );
#endif
    }


    void terminate_key( )
    {
#if eOPSYS == ePOSIX
        curses_key_map.erase( curses_key_map.begin( ), curses_key_map.end( ) );
#endif
    }


    void refresh_on_key( bool flag )
    {
        key_refresh = flag;
    }


    int key( )
    {
        if( key_refresh ) refresh( );
        return key_wait( );
    }


    int key_wait( )
    {
        int ch;

#if eOPSYS != ePOSIX
        static char  sequence_buffer[8];
        static char *sequence_ptr = NULL;

        // We are processing the leftovers of an aborted escape sequence...
        if( sequence_ptr != NULL ) {
            int return_value = *sequence_ptr++;
            if( *sequence_ptr == '\0' ) sequence_ptr = NULL;
            return return_value;
        }
#endif

        ch = getch( );

#if eOPSYS == ePOSIX
        // If this is a special character that curses is handing back, process it.
        if( ch >= KEY_MIN ) {
            KeyMap::iterator p = curses_key_map.find( ch );
            if( p != curses_key_map.end( ) ) {
                return ( *p ).second;
            }
        }

#else
        // If the character is the escape character, deal with special keys. This is for
        // non-curses systems (like DOS) under the SCR_ASCIIKEYS option.
        //
        if( ch == K_ESC ) {
            sequence_buffer[0] = '\0';
            sequence_ptr = sequence_buffer;

            // This could be done a lot more efficiently and more nicely.
            while( 1 ) {
                ch = getch( );
                *sequence_ptr++ = static_cast< char >( ch );
                *sequence_ptr   = '\0';

                bool prefix_found = false;
                KeyEscape *escape_table = special_key_escapes;
                while( escape_table->escape_sequence ) {
                    char *result = std::strstr( escape_table->escape_sequence, sequence_buffer );

                    if( result == escape_table->escape_sequence ) {
                        prefix_found = true;
                        if( std::strcmp( escape_table->escape_sequence, sequence_buffer ) == 0 ) {
                            sequence_buffer = NULL;
                            return escape_table->key_code;
                        }
                    }
                    escape_table++;
                }

                // Do we still have a chance?
                if( !prefix_found ) {
                    sequence_ptr = sequence_buffer;
                    return *sequence_ptr++;
                }
            }
        } // The code in the if should never fall through.

#endif

        // If the character is not a control character, return it as is.
        if( ch > K_CTRLZ ) return ch;

        // If it's CTRLF, figure out which function key code to return.
        if( ch == K_CTRLF ) {
            ch = getch( );
            if( std::isdigit( ch ) ) return function_translation[ch - '0'];
            return '*';
        }

        // If it's CTRLS, figure out which shifted function key code to return.
        if( ch == K_CTRLS ) {
            ch = getch( );
            if( std::isdigit( ch ) ) return shift_function_translation[ch - '0'];
            return '*';
        }

        // If it's CTRLC, figure out which control function key code to return.
        if( ch == K_CTRLC ) {
            ch = getch( );
            if( std::isdigit( ch ) ) return control_function_translation[ch - '0'];
            return '*';
        }

        // If it's CTRLA, handle ALT+letter and ALT+function key codes.
        if( ch == K_CTRLA ) {
            ch = getch( );
            if( ch == '-'  ) return K_ALTDASH;
            if( ch == '='  ) return K_ALTEQU;
            if( std::isdigit( ch ) ) return alt_function_translation[ch - '0'];
            if( std::isalpha( ch ) ) return alt_translation[std::tolower( ch ) - 'a'];
            return '*';
        }

        // Otherwise look up the translated character.
        return control_translation[ch];
    }

#else  // Not SCR_ASCIIKEYS and not ePOSIX

    void initialize_key( )
    {
    }


    void terminate_key( )
    {
    }


    void refresh_on_key( bool flag )
    {
        key_refresh = flag;
    }


    int key( )
    {
        if( key_refresh ) refresh( );
        return key_wait( );
    }

    // See comments in key_wait below. This table is necessary because the values associated with
    // the ALT+letter keys are not in any order.
    //
#if eOPSYS == eWIN32
    static int alt_letter_table[] = {
        K_ALTA, K_ALTB, K_ALTC, K_ALTD, K_ALTE, K_ALTF, K_ALTG, K_ALTH,
        K_ALTI, K_ALTJ, K_ALTK, K_ALTL, K_ALTM, K_ALTN, K_ALTO, K_ALTP,
        K_ALTQ, K_ALTR, K_ALTS, K_ALTT, K_ALTU, K_ALTV, K_ALTW, K_ALTX,
        K_ALTY, K_ALTZ
    };
    static int alt_number_table[] = {
        K_ALT0, K_ALT1, K_ALT2, K_ALT3, K_ALT4,
        K_ALT5, K_ALT6, K_ALT7, K_ALT8, K_ALT9
    };
#endif


    int key_wait( )
    {
        int ch;

#if eOPSYS == eDOS
        if( ( ch = getch( ) ) != 0 ) return( ch );
        return getch( ) + XF;

#elif eOPSYS == eWIN32
        if ( ( (ch = _getch( ) ) != 0 ) && ( ch != 0xE0 ) ) {
            // This is a bit of a hack. The VC++ getch() does not process ALT+letter
            // keys the way it was done by MS-DOS. Instead of rewriting getch() using
            // the console API, I'm using a trick to check if the ALT key is pressed.
            //
            SHORT key_state = GetAsyncKeyState( VK_MENU );
            if( key_state & 0x8000 ) {
                if( 'a' <= ch && ch <= 'z' ) ch = alt_letter_table[ch - 'a'];
                else if( '0' <= ch && ch <= '9' ) ch = alt_number_table[ch - '0'];
            }
            return ch;
        }
        return _getch( ) + XF;

#elif eOPSYS == eOS2
        if ( ( (ch = getch( ) ) != 0 ) && ( ch != 0xE0 ) ) return ch;
        return getch( ) + XF;

#elif eOPSYS == ePOSIX
        return getch( );
#endif
    }

#endif

}
