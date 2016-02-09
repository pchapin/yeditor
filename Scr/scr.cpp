/*! \file    scr.cpp
    \brief   Implementation of portable screen handling functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "environ.hpp"

// Produce an error for alien operating systems.
#if !(eOPSYS == eDOS   || \
      eOPSYS == eWIN32 || \
      eOPSYS == eOS2   || \
      eOPSYS == ePOSIX || defined(SCR_ANSI))
#error Scr only supports DOS, OS/2, Win32, Unix (curses), or SCR_ANSI.
#endif

// Force the user to decide: "Direct" video RAM access or ANSI escape sequences. This decision
// does not need to be made on POSIX systems.
//
#if !(defined(SCR_DIRECT) || defined(SCR_ANSI)) && eOPSYS != ePOSIX
#error You must #define SCR_DIRECT or SCR_ANSI.
#endif

// POSIX doesn't support "direct" video access.
#if defined(SCR_DIRECT) && (eOPSYS == ePOSIX)
#error POSIX does not support direct video access.
#endif

// POSIX doesn't need SCR_ANSI specified.
#if defined(SCR_ANSI) && (eOPSYS == ePOSIX)
#error POSIX does not want SCR_ANSI defined. It uses curses.
#endif

#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
// The DOS SCR_DIRECT version needs access to int86() for BIOS stuff.
#include <dos.h>
#ifdef __386__
#define do_interrupt(x,y,z) int386(x,y,z)
#else
#define do_interrupt(x,y,z) int86(x,y,z)
#endif
#endif

#if (eOPSYS == eOS2) && defined(SCR_DIRECT)
#define INCL_VIO
// The OS/2 SCR_DIRECT version needs access to Vio functions.
#include <os2.h>
#endif

#if (eOPSYS == eWIN32) && defined(SCR_DIRECT)
// The Win32 SCR_DIRECT version needs access to console functions.
#include <windows.h>
#endif

#if eOPSYS == ePOSIX
#include <map>
// Curses related material...
#define NOMACROS
#define NCURSES_NOMACROS
#include <ncurses.h>
#include <term.h>

// Crude from <term.h> that interfers with my stuff...
#undef clear_screen
#undef max_colors
#endif

#include "scr.hpp"

#if defined(SCR_ANSI)
// If we're going to use ANSI, we'll need the basic ANSI functions.
#include "ansiscr.hpp"
#endif

namespace scr {

    //=================================
    //           Global Data
    //=================================

#if defined(SCR_DIRECT)
    static int NMBR_ROWS = 25;   // Number of rows on the IBM PC scrren.
    static int NMBR_COLS = 80;   // Number of columns on the IBM PC screen.
    const  int MAX_PRINT_SIZE = 1024; // Largest string Print() can handle.
#endif

#if defined(SCR_ANSI) || (eOPSYS == ePOSIX)
    static int NMBR_ROWS = 24;   // Number of rows on the screen.
    static int NMBR_COLS = 80;   // Number of columns on the screen.
    const  int MAX_PRINT_SIZE = 1024; // Largest string print() can handle.

    static char *physical_image;
    static int   physical_row = 1;     // Actual position of the cursor.
    static int   physical_column = 1;  //   etc...
#endif

#if eOPSYS == ePOSIX
    typedef std::pair< const unsigned char, chtype > CharacterPair;
    typedef std::map< unsigned char, chtype, std::less< unsigned char > > CharacterMap;
    typedef std::pair< const int, int > ColorPair;
    typedef std::map< int, int, std::less< int > > ColorMap;

    // Stores information about a curses "color pair."
    struct CursesColorInfo {
        int foreground;
        int background;
    };

    static CharacterMap box_character_map;  // Maps scr box drawing characters to curses.
    static ColorMap     colors_map;   // Maps portscr colors to curses colors.
    static bool         color_works;  // =true if the terminal supports color.
#endif


#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
#define VIDEO_MONO           0  // Symbolic names for the two video modes.
#define VIDEO_COLOR          1

#define BIOS_VIDEO        0x10  // INT number for BIOS.
#define SET_CRTMODE          0  // BIOS function numbers.
#define SET_CURSORTYPE       1
#define SET_CURSORPOS        2
#define GET_CURSORPOS        3
#define SCROLL_UP            6
#define SCROLL_DOWN          7
#define GET_CRTMODE         15

    static int       old_mode       = 0;             // Video mode before initialize( ).
    static unsigned  screen_segment = 0xB800;        // Segment address of video RAM.
    static int       video_mode     = VIDEO_COLOR;   // Current video mode.
#endif

    // General data required by all versions.

    // Note that if the values in the array below are ever changed, the character_associations
    // array in initialize_character_map will have to be updated as well. That array is used by
    // the curses version of scr to map box drawing characters to a terminal's alternate
    // character set.
    //
    static struct BoxChars box_definitions[]={
        { 205, 186, 201, 187, 200, 188, 181, 198, 208, 210, 206 },  // Double lines.
        { 196, 179, 218, 191, 192, 217, 180, 195, 193, 194, 197 },  // Single lines.
        { 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177 },  // Dark graphic.
        { 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176 },  // Light graphic.
        { 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219 },  // Solid.
        {  45, 124,  43,  43,  43,  43,  43,  43,  43,  43,  43 },  // ASCII.
        {  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32 }   // Blank.
    };

    static int   initialize_counter = 0;  // Counts the number of pending Initialize() calls.
    static int   max_columns = NMBR_COLS; // Usable size of the screen.
    static int   max_rows    = NMBR_ROWS; //   etc...
    static char *screen_image;
    static int   virtual_column = 1;      // Virtual cursor coordinates.
    static int   virtual_row = 1;         //   etc...
    static char  work_buffer[MAX_PRINT_SIZE+1];  // Used by print...
#if eOPSYS == eWIN32
    static CHAR_INFO *console_image;  // Win32 requires this as well
#endif


    //=====================================
    //          Private Functions
    //=====================================

#if eOPSYS == ePOSIX

    static void initialize_character_map( )
    {
        static CharacterPair character_associations[] = {
            CharacterPair( 205, ACS_HLINE ),
            CharacterPair( 186, ACS_VLINE ),
            CharacterPair( 201, ACS_ULCORNER ),
            CharacterPair( 187, ACS_URCORNER ),
            CharacterPair( 200, ACS_LLCORNER ),
            CharacterPair( 188, ACS_LRCORNER ),
            CharacterPair( 181, ACS_RTEE ),
            CharacterPair( 198, ACS_LTEE ),
            CharacterPair( 208, ACS_BTEE ),
            CharacterPair( 210, ACS_TTEE ),
            CharacterPair( 206, ACS_PLUS ),
            CharacterPair( 196, ACS_HLINE ),
            CharacterPair( 179, ACS_VLINE ),
            CharacterPair( 218, ACS_ULCORNER ),
            CharacterPair( 191, ACS_URCORNER ),
            CharacterPair( 192, ACS_LLCORNER ),
            CharacterPair( 217, ACS_LRCORNER ),
            CharacterPair( 180, ACS_RTEE ),
            CharacterPair( 195, ACS_LTEE ),
            CharacterPair( 193, ACS_BTEE ),
            CharacterPair( 194, ACS_TTEE ),
            CharacterPair( 197, ACS_PLUS ),
            CharacterPair( 177, ACS_CKBOARD ),
            CharacterPair( 219, ACS_CKBOARD )
        };

        box_character_map.insert(
            character_associations,
            character_associations + sizeof( character_associations ) / sizeof( CharacterPair ) );
    }


    static void initialize_colors( )
    {
        static ColorPair color_associations[] = {
            ColorPair( WHITE  |REV_BLACK,    0 ),
            ColorPair( BLUE   |REV_BLACK,    1 ),
            ColorPair( GREEN  |REV_BLACK,    2 ),
            ColorPair( CYAN   |REV_BLACK,    3 ),
            ColorPair( RED    |REV_BLACK,    4 ),
            ColorPair( MAGENTA|REV_BLACK,    5 ),
            ColorPair( BROWN  |REV_BLACK,    6 ),
            ColorPair( BLACK  |REV_BLACK,    7 ),

            ColorPair( WHITE  |REV_BLUE,     8 ),
            ColorPair( BLUE   |REV_BLUE,     9 ),
            ColorPair( GREEN  |REV_BLUE,    10 ),
            ColorPair( CYAN   |REV_BLUE,    11 ),
            ColorPair( RED    |REV_BLUE,    12 ),
            ColorPair( MAGENTA|REV_BLUE,    13 ),
            ColorPair( BROWN  |REV_BLUE,    14 ),
            ColorPair( BLACK  |REV_BLUE,    15 ),

            ColorPair( WHITE  |REV_GREEN,   16 ),
            ColorPair( BLUE   |REV_GREEN,   17 ),
            ColorPair( GREEN  |REV_GREEN,   18 ),
            ColorPair( CYAN   |REV_GREEN,   19 ),
            ColorPair( RED    |REV_GREEN,   20 ),
            ColorPair( MAGENTA|REV_GREEN,   21 ),
            ColorPair( BROWN  |REV_GREEN,   22 ),
            ColorPair( BLACK  |REV_GREEN,   23 ),

            ColorPair( WHITE  |REV_CYAN,    24 ),
            ColorPair( BLUE   |REV_CYAN,    25 ),
            ColorPair( GREEN  |REV_CYAN,    26 ),
            ColorPair( CYAN   |REV_CYAN,    27 ),
            ColorPair( RED    |REV_CYAN,    28 ),
            ColorPair( MAGENTA|REV_CYAN,    29 ),
            ColorPair( BROWN  |REV_CYAN,    30 ),
            ColorPair( BLACK  |REV_CYAN,    31 ),

            ColorPair( WHITE  |REV_RED,     32 ),
            ColorPair( BLUE   |REV_RED,     33 ),
            ColorPair( GREEN  |REV_RED,     34 ),
            ColorPair( CYAN   |REV_RED,     35 ),
            ColorPair( RED    |REV_RED,     36 ),
            ColorPair( MAGENTA|REV_RED,     37 ),
            ColorPair( BROWN  |REV_RED,     38 ),
            ColorPair( BLACK  |REV_RED,     39 ),

            ColorPair( WHITE  |REV_MAGENTA, 40 ),
            ColorPair( BLUE   |REV_MAGENTA, 41 ),
            ColorPair( GREEN  |REV_MAGENTA, 42 ),
            ColorPair( CYAN   |REV_MAGENTA, 43 ),
            ColorPair( RED    |REV_MAGENTA, 44 ),
            ColorPair( MAGENTA|REV_MAGENTA, 45 ),
            ColorPair( BROWN  |REV_MAGENTA, 46 ),
            ColorPair( BLACK  |REV_MAGENTA, 47 ),

            ColorPair( WHITE  |REV_BROWN,   48 ),
            ColorPair( BLUE   |REV_BROWN,   49 ),
            ColorPair( GREEN  |REV_BROWN,   50 ),
            ColorPair( CYAN   |REV_BROWN,   51 ),
            ColorPair( RED    |REV_BROWN,   52 ),
            ColorPair( MAGENTA|REV_BROWN,   53 ),
            ColorPair( BROWN  |REV_BROWN,   54 ),
            ColorPair( BLACK  |REV_BROWN,   55 ),

            ColorPair( WHITE  |REV_WHITE,   56 ),
            ColorPair( BLUE   |REV_WHITE,   57 ),
            ColorPair( GREEN  |REV_WHITE,   58 ),
            ColorPair( CYAN   |REV_WHITE,   59 ),
            ColorPair( RED    |REV_WHITE,   60 ),
            ColorPair( MAGENTA|REV_WHITE,   61 ),
            ColorPair( BROWN  |REV_WHITE,   62 ),
            ColorPair( BLACK  |REV_WHITE,   63 )
        };
        static CursesColorInfo color_initializers[] = {
            { COLOR_WHITE,   COLOR_BLACK   },
            { COLOR_BLUE,    COLOR_BLACK   },
            { COLOR_GREEN,   COLOR_BLACK   },
            { COLOR_CYAN,    COLOR_BLACK   },
            { COLOR_RED,     COLOR_BLACK   },
            { COLOR_MAGENTA, COLOR_BLACK   },
            { COLOR_YELLOW,  COLOR_BLACK   },
            { COLOR_BLACK,   COLOR_BLACK   },

            { COLOR_WHITE,   COLOR_BLUE    },
            { COLOR_BLUE,    COLOR_BLUE    },
            { COLOR_GREEN,   COLOR_BLUE    },
            { COLOR_CYAN,    COLOR_BLUE    },
            { COLOR_RED,     COLOR_BLUE    },
            { COLOR_MAGENTA, COLOR_BLUE    },
            { COLOR_YELLOW,  COLOR_BLUE    },
            { COLOR_BLACK,   COLOR_BLUE    },

            { COLOR_WHITE,   COLOR_GREEN   },
            { COLOR_BLUE,    COLOR_GREEN   },
            { COLOR_GREEN,   COLOR_GREEN   },
            { COLOR_CYAN,    COLOR_GREEN   },
            { COLOR_RED,     COLOR_GREEN   },
            { COLOR_MAGENTA, COLOR_GREEN   },
            { COLOR_YELLOW,  COLOR_GREEN   },
            { COLOR_BLACK,   COLOR_GREEN   },

            { COLOR_WHITE,   COLOR_CYAN    },
            { COLOR_BLUE,    COLOR_CYAN    },
            { COLOR_GREEN,   COLOR_CYAN    },
            { COLOR_CYAN,    COLOR_CYAN    },
            { COLOR_RED,     COLOR_CYAN    },
            { COLOR_MAGENTA, COLOR_CYAN    },
            { COLOR_YELLOW,  COLOR_CYAN    },
            { COLOR_BLACK,   COLOR_CYAN    },

            { COLOR_WHITE,   COLOR_RED     },
            { COLOR_BLUE,    COLOR_RED     },
            { COLOR_GREEN,   COLOR_RED     },
            { COLOR_CYAN,    COLOR_RED     },
            { COLOR_RED,     COLOR_RED     },
            { COLOR_MAGENTA, COLOR_RED     },
            { COLOR_YELLOW,  COLOR_RED     },
            { COLOR_BLACK,   COLOR_RED     },

            { COLOR_WHITE,   COLOR_MAGENTA },
            { COLOR_BLUE,    COLOR_MAGENTA },
            { COLOR_GREEN,   COLOR_MAGENTA },
            { COLOR_CYAN,    COLOR_MAGENTA },
            { COLOR_RED,     COLOR_MAGENTA },
            { COLOR_MAGENTA, COLOR_MAGENTA },
            { COLOR_YELLOW,  COLOR_MAGENTA },
            { COLOR_BLACK,   COLOR_MAGENTA },

            { COLOR_WHITE,   COLOR_YELLOW  },
            { COLOR_BLUE,    COLOR_YELLOW  },
            { COLOR_GREEN,   COLOR_YELLOW  },
            { COLOR_CYAN,    COLOR_YELLOW  },
            { COLOR_RED,     COLOR_YELLOW  },
            { COLOR_MAGENTA, COLOR_YELLOW  },
            { COLOR_YELLOW,  COLOR_YELLOW  },
            { COLOR_BLACK,   COLOR_YELLOW  },

            { COLOR_WHITE,   COLOR_WHITE   },
            { COLOR_BLUE,    COLOR_WHITE   },
            { COLOR_GREEN,   COLOR_WHITE   },
            { COLOR_CYAN,    COLOR_WHITE   },
            { COLOR_RED,     COLOR_WHITE   },
            { COLOR_MAGENTA, COLOR_WHITE   },
            { COLOR_YELLOW,  COLOR_WHITE   },
            { COLOR_BLACK,   COLOR_WHITE   }
        };

        color_works = true;
        if( start_color( ) == ERR ) {
            color_works = false;
            return;
        }

        int i;
        int max_colors = COLOR_PAIRS;
        if( max_colors > 64 ) max_colors = 64;

        // Color pair #0 is predefined to be the terminal default (supposedly white on black).
        for( i = 1; i < max_colors; i++ ) {
            init_pair( i, color_initializers[i].foreground, color_initializers[i].background );
        }

        colors_map.insert(
          color_associations,
          color_associations + sizeof( color_associations ) / sizeof( ColorPair ) );
    }

#endif


    //======================================
    //           Public Functions
    //======================================

    extern void initialize_key( );
    extern void terminate_key( );

    //+++++
    // System dependent functions
    //+++++

    //! Initializes the scr library.
    /*!
     * This function must be called before <em>any</em> other function from this package is
     * used. It causes the screen to clear and the cursor to be moved to the home position. Once
     * Initialize has been called, you should not use any other functions for screen or keyboard
     * I/O besides the functions in scr.
     *
     * This function deduces the maximum sized screen supported by the system and arrange things
     * so that information will be returned by number_of_rows and number_of_columns.
     *
     * This function can be called several times. All additional times will be ignored. However,
     * terminate( ) must be called a corresponding number of times before it will shut down the
     * scr library.
     *
     * \return <b>true</b> if the initialization was successful; <b>false</b> otherwise. If this
     * function returns false you should not use any other Scr functions. There is no need to
     * call terminate( ) in such a case.
     */
    bool initialize( )
    {
#if (eOPSYS == eOS2) && defined(SCR_DIRECT)
        VIOMODEINFO screen_info;
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        union REGS r;
#endif

        // Return at once if already initialized.
        if( initialize_counter > 0 ) {
            ++initialize_counter;
            return true;
        }

        initialize_key( );

#if (eOPSYS == eOS2) && defined(SCR_DIRECT)
        VioGetMode( &screen_info, 0 );
        max_rows = NMBR_ROWS = screen_info.row;
        max_columns = NMBR_COLS = screen_info.col;
#endif

#if (eOPSYS == eWIN32) && defined (SCR_DIRECT)
        CONSOLE_SCREEN_BUFFER_INFO scrninfo;

        GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &scrninfo );
        max_rows = NMBR_ROWS = scrninfo.srWindow.Bottom - scrninfo.srWindow.Top + 1;
        max_columns = NMBR_COLS = scrninfo.srWindow.Right - scrninfo.srWindow.Left + 1;
#endif

#if (eOPSYS == ePOSIX)
        // Initialize the curses package.
        initscr( );
        raw( );
        noecho( );
        nonl();
        intrflush( stdscr, FALSE );
        keypad( stdscr, TRUE );
        initialize_character_map( );
        initialize_colors( );

        // How much screen space do we have?
        max_rows = NMBR_ROWS = LINES;
        max_columns = NMBR_COLS = COLS;
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        // Learn what video mode is currently being used.
        r.h.ah = static_cast< unsigned char >( GET_CRTMODE );
        do_interrupt( BIOS_VIDEO, &r, &r );

        // Save the old video mode.
        old_mode = r.h.al;

        // If it's mode 7 (monochrome), adjust our records.
        if( r.h.al == 7 ) {
            screen_segment = 0xB000;
            video_mode = VIDEO_MONO;
        }

        // Otherwise, force mode 3 (25x80 color).
        else if( r.h.al != 3 ) {
            r.h.ah = static_cast< unsigned char >( SET_CRTMODE );
            r.h.al = 3;
            do_interrupt( BIOS_VIDEO, &r, &r );
        }
#endif

        // Allocate screen images.
        screen_image = new char[NMBR_ROWS * 2 * NMBR_COLS];
#if (eOPSYS == ePOSIX) || defined(SCR_ANSI)
        physical_image = new char[NMBR_ROWS * 2 * NMBR_COLS];
#endif
#if (eOPSYS == eWIN32) && defined(SCR_DIRECT)
        console_image = new CHAR_INFO[NMBR_ROWS * NMBR_COLS];
#endif

        // In any case, clear the screen and home the cursor.
        clear_screen( );

        // Adjust our records.
        ++initialize_counter;

        return true;
    }


    //! Shutdown the Scr library.
    /*!
     * This function must be called before the program exits. Once it has been called, no other
     * function from this package can be used (except initialize( )). This function causes the
     * screen to clear and the cursor to be moved to the home position.
     *
     * Calls to terminate( ) must be matched with calls to initialize( ). Only the last call to
     * terminate( ), matching the first call to initialize( ), will actually shut down the scr
     * library.
     */
    void terminate( )
    {
#if (((eOPSYS == eOS2) || (eOPSYS == eWIN32)) && defined(SCR_DIRECT)) || (eOPSYS == ePOSIX)
        int counter;
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        union REGS  r;
        int   counter;
#endif

        if( initialize_counter == 0 ) return;  // initialize( ) never called.
        --initialize_counter;
        if( initialize_counter != 0 ) return;  // initialize( ) called more than once.

#if ((eOPSYS == eOS2) || (eOPSYS == eWIN32)) && defined(SCR_DIRECT)
        // Clear the screen and home the cursor.
        for( counter = 0; counter < NMBR_ROWS * 2 * NMBR_COLS; counter += 2 ) {
            screen_image[counter]     = ' ';
            screen_image[counter + 1] = WHITE|REV_BLACK;
        }
        virtual_row = 1;
        virtual_column = 1;
        redraw( );
#endif

#if defined(SCR_ANSI)
        // Clear the screen and home the cursor. (We don't need to update screen_image[] or
        // physical_image[]. This is the last scr... function that will be called until another
        // initialize( )).
        //
        reset_screen( );
        erase_screen( );
        position_cursor( 1, 1 );
#endif

#if (eOPSYS == ePOSIX)
        // Clear the screen and home the cursor.
        for( counter = 0; counter < NMBR_ROWS * 2 * NMBR_COLS; counter += 2 ) {
            screen_image[counter]     = ' ';
            screen_image[counter + 1] = WHITE|REV_BLACK;
        }
        virtual_row = 1;
        virtual_column = 1;
        redraw( );

        // Clean up the curses routines.
        endwin( );

        box_character_map.erase( box_character_map.begin( ), box_character_map.end( ) );
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        // Learn what video mode is currently being used. It's either 3 or 7.
        r.h.ah = static_cast< unsigned char >( GET_CRTMODE );
        do_interrupt( BIOS_VIDEO, &r, &r );

        // If this is different than the old mode, change back to the old mode.
        if( r.h.al != old_mode ) {
            r.h.ah = static_cast< unsigned char >( SET_CRTMODE );
            r.h.al = static_cast< unsigned char >( old_mode );
            do_interrupt( BIOS_VIDEO, &r, &r );
        }

        // Otherwise clear the screen and home the cursor (mode switching has this effect so we
        // don't need to mess with this above).
        //
        else {
            for( counter = 0; counter < NMBR_ROWS * 2 * NMBR_COLS; counter += 2 ) {
                screen_image[counter]     = ' ';
                screen_image[counter + 1] = WHITE|REV_BLACK;
            }
            virtual_row = 1;
            virtual_column = 1;
            redraw( );
        }
#endif

        // Free dynamic data structures.
        delete [] screen_image; screen_image = NULL;
#if eOPSYS == ePOSIX || defined(SCR_ANSI)
        delete [] physical_image; physical_image = NULL;
#endif
#if eOPSYS == eWIN32 && defined(SCR_DIRECT)
        delete [] console_image; console_image = NULL;
#endif

        terminate_key( );
    }


    //! Return the box drawing characters associated with a certain box type.
    /*!
     * Use this function to inspect the specific box drawing characters associated with a
     * particular box type. In the current version of Scr it is permissible to modify the
     * structure pointed at by the return value. Such modifications change the box drawing
     * characters for that box type throughout the program.
     */
    struct BoxChars *get_box_characters( enum BoxType the_type )
    {
#if defined(SCR_ASCIIBOXES)
        if( the_type == BLANK_BOX ) return &box_definitions[BLANK_BOX];
        else return &box_definitions[ASCII];
#else
        return &box_definitions[the_type];
#endif
    }


    //! Indicate if a monochrome monitor is being used.
    /*!
     *  Although the functions in this package do color conversions for monochrome monitors in
     *  most cases automatically, it is sometimes desirable to handle color in a special way for
     *  monochrome screens.
     *
     *  Note that scr assumes all OS/2 and Win32 systems can do color. This function might
     *  return true only on MS-DOS and POSIX systems.
     *
     *  \return <b>true</b> if a monochrome display is being used; <b>false</b> otherwise.
     */
    bool is_monochrome( )
    {
#if (eOPSYS == eOS2) && defined(SCR_DIRECT)
        // Assume all OS/2 machines can do color.
        return false;
#endif

#if (eOPSYS == eWIN32) && defined(SCR_DIRECT)
        // Assume all Win32 machines can do color.
        return false;
#endif

#if defined(SCR_ANSI)
        // Assume all ANSI terminals can do color.
        return false;
#endif

#if (eOPSYS == ePOSIX)
        return color_works == false;
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        return video_mode == VIDEO_MONO;
#endif
    }


    //! Modify a color attribute for use on a monochrome monitor.
    /*!
     * This function adjusts a color attribute so that the text will remain visible even if a
     * monochrome monitor is being used.
     */
    int convert_attribute( int attribute )
    {
        // I assume that ANSI screen handling and all OS/2 and Win32 systems can do color.

#if (eOPSYS == ePOSIX)
        // Return at once with the attribute unchanged if not the monochrome mode.
        if( color_works ) return attribute;

        // If black background, force white foreground.
        if( ( attribute & 0x70 ) == REV_BLACK ) {
            attribute |= WHITE;
        }

        // Otherwise there's a colored background, force reverse video.
        else {
            attribute |= REV_WHITE;
            attribute &= 0xF8;         // Zero out (make black) foreground only.
        }
#endif

#if (eOPSYS == eDOS) && defined(SCR_DIRECT)
        if( video_mode != VIDEO_MONO ) return attribute;

        if( ( attribute & 0x70 ) == REV_BLACK ) {
            attribute |= WHITE;
        }

        else {
            attribute |= REV_WHITE;
            attribute &= 0xF8;
        }
#endif

        return attribute;
    }


    //+++++
    // System independent functions
    //+++++

    //! Reverse a color attribute.
    /*!
     * There is no effect on the blink or bright bits of the attribute.
     *
     * \param attribute The attribute to be reversed.
     * \return A color attribute with foreground and background colors of \p attribute swapped.
     */
    int reverse_attribute( int attribute )
    {
        // Extract the foreground and background colors.
        int first  = attribute & 0x07;
        int second = ( attribute & 0x70 ) >> 4;

        // Erase the colors in the original attribute.
        attribute &= 0xf8;
        attribute &= 0x8f;

        // Reinstall the colors in the opposite places.
        attribute |= first << 4;
        attribute |= second;

        return attribute;
    }


    //! Force a region to be in bounds.
    /*!
     * This function accepts a specification of a region and forces that region to be contained
     * on the screen. It verifies the sanity of the row and column position of the region's
     * upper left corner and to restricts the region's width and height so that the region does
     * not overlap the edge of the screen.
     *
     * \param r <b>[in-out]</b> Pointer to the starting row of the region.
     * \param c <b>[in-out]</b> Pointer to the starting column of the region.
     * \param w <b>[in-out]</b> Pointer to the width of the region.
     * \param h <b>[in-out]</b> Pointer to the height of the region.
     */
    void adjust_dimensions( int *r, int *c, int *w, int *h )
    {
        if( *r < 1 )         *r = 1;
        if( *r > NMBR_ROWS ) *r = NMBR_ROWS;
        if( *c < 1 )         *c = 1;
        if( *c > NMBR_COLS ) *c = NMBR_COLS;
        if( *h <= 0 )        *h = 1;
        if( *w <= 0 )        *w = 1;
        if( *r + *h - 1 > NMBR_ROWS ) *h = NMBR_ROWS - *r + 1;
        if( *c + *w - 1 > NMBR_COLS ) *w = NMBR_COLS - *c + 1;
    }


    //! Return the number of rows on the screen.
    int number_of_rows( )
    {
        return max_rows;
    }


    //! Return the number of columns on the screen.
    int number_of_columns( )
    {
        return max_columns;
    }


    //! Read from a region of the screen.
    /*!
     *  When this function returns the buffer will be filled with alternating characters and
     *  attribute bytes. The buffer must be 2 * width * height in size. This function does not
     *  check this.
     *
     *  \param row The row number of the region's upper left corner.
     *  \param column The column number of the region's upper left corner.
     *  \param width The width of the region in columns.
     *  \param height The height of the region in rows.
     *  \param buffer <b>[out]</b> Pointer to a region of memory to receive the text and
     *  attributes.
     */
    void read( int row, int column, int width, int height, char *buffer )
    {
        unsigned  number_of_rows;  // Loop index.
        unsigned  row_length;      // Number of bytes in row of region.
        char     *screen_pointer;  // Pointer into the screen image.

        adjust_dimensions( &row, &column, &width, &height );

        row_length = 2 * width;
        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;

        // Loop over all rows in the region.
        for( number_of_rows = height; number_of_rows > 0; number_of_rows-- ) {

            // Copy data from screen image to buffer and adjust offsets.
            std::memcpy( buffer, screen_pointer, row_length );
            screen_pointer += 2 * NMBR_COLS;
            buffer   += row_length;
        }
    }


    //! Read the text from a region of the screen.
    /*!
     * This function is similar to read( ) except that it only returns the text in the region
     * instead of the text and the attributes. When this function returns the buffer will be
     * filled with characters only. The buffer must be width * height in size. This function
     * does not check this.
     *
     *  \param row The row number of the region's upper left corner.
     *  \param column The column number of the region's upper left corner.
     *  \param width The width of the region in columns.
     *  \param height The height of the region in rows.
     *  \param buffer <b>[out]</b> Pointer to a region of memory to receive the text.
     */
    void read_text( int row, int column, int width, int height, char *buffer )
    {
        char *screen_pointer;
        char *source;
        int   i;

        adjust_dimensions( &row, &column, &width, &height );

        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        for( ; height > 0; height-- ) {
            source = screen_pointer;
            for( i = 0; i < width; i++, source++ ) *buffer++ = *source++;
            screen_pointer += 2 * NMBR_COLS;
        }
    }


    //! Write to a region of the screen.
    /*!
     * When this function returns the scr internal screen buffer will be filled with alternating
     * characters and attribute bytes from the given memory space. The buffer is assumed to be
     * 2 * width * height in size.
     *
     *  \param row The row number of the region's upper left corner.
     *  \param column The column number of the region's upper left corner.
     *  \param width The width of the region in columns.
     *  \param height The height of the region in rows.
     *  \param buffer <b>[in]</b> Pointer to a region of memory from which the text and
     *  attributes will be taken.
     */
    void write( int row, int column, int width, int height, const char *buffer )
    {
        unsigned  number_of_rows;    // Loop index.
        unsigned  row_length;      // Number of bytes in row of region.
        char     *screen_pointer;        // Pointer into the screen image.

        adjust_dimensions( &row, &column, &width, &height );

        row_length = 2 * width;
        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;

        // Loop over all rows in the region.
        for( number_of_rows = height; number_of_rows > 0; number_of_rows-- ) {

            // Copy data from buffer to screen image and adjust offsets.
            std::memcpy( screen_pointer, buffer, row_length );
            screen_pointer += 2 * NMBR_COLS;
            buffer   += row_length;
        }
    }


    //! Write text to a region of the screen.
    /*!
     * This function is similar to scr::Write except that it only writes text. When this
     * function returns the scr internal screen buffer will be filled with characters from the
     * given memory space. The buffer is assumed to be width * height in size.
     *
     *  \param row The row number of the region's upper left corner.
     *  \param column The column number of the region's upper left corner.
     *  \param width The width of the region in columns.
     *  \param height The height of the region in rows.
     *  \param buffer <b>[in]</b> Pointer to a region of memory from which the text will be
     *  taken.
     */
    void write_text( int row, int column, int width, int height, const char *buffer )
    {
        char *screen_pointer;
        char *destination;
        int   i;

        adjust_dimensions( &row, &column, &width, &height );

        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        for( ; height > 0; height-- ) {
            destination = screen_pointer;
            for( i = 0; i < width; i++, destination++ ) *destination++ = *buffer++;
            screen_pointer += 2 * NMBR_COLS;
        }
    }


    //! Print formatted text and attributes.
    /*!
     *  This function rewrites both the characters and the attributes on the screen.
     *
     *  \param row The row number where the text will be printed.
     *  \param column The column number of the text's starting position.
     *  \param Count The maximum number of characters to print.
     *  \param attribute The color attribute to use during the printing.
     *  \param format <b>[in]</b>A printf-like format string describing what is to be printed.
     *  \param ... Any additional parameters as required by the format string.
     */
    void print( int row, int column, std::size_t Count, int attribute, const char *format, ... )
    {
        char   *screen_pointer;
        char   *string       = work_buffer;
        int     dummy_height = 1;
        int     width = static_cast<int>( Count );
        std::va_list args;

        adjust_dimensions( &row, &column, &width, &dummy_height );
        attribute = convert_attribute( attribute );

        va_start( args, format );
        std::vsprintf( work_buffer, format, args );
        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        while( *string && width-- ) {
            *screen_pointer++ = *string++;
            *screen_pointer++ = static_cast< char >( attribute );
        }
        va_end( args );
    }


    //! Print formatted text.
    /*!
     * This function is similar to print( ) except that it uses the color attributes currently
     * on the screen instead of writing new attributes.
     *
     *  \param row The row number where the text will be printed.
     *  \param column The column number of the text's starting position.
     *  \param Count The maximum number of characters to print.
     *  \param format <b>[in]</b> A printf-like format string describing what is to be printed.
     *  \param ... Any additional parameters as required by the format string.
     */
    void print_text( int row, int column, std::size_t Count, const char *format, ... )
    {
        char   *screen_pointer;
        char   *string       = work_buffer;
        int     dummy_height = 1;
        int     width = static_cast<int>( Count );
        std::va_list args;

        adjust_dimensions( &row, &column, &width, &dummy_height );

        va_start( args, format );
        std::vsprintf( work_buffer, format, args );
        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        while( *string && width-- ) {
            *screen_pointer++ = *string++;
            screen_pointer++;
        }
        va_end( args );
    }


    //! Erases a region of the screen.
    /*!
     * This function clears the region by writing spaces over the entire region.
     *
     * \param row The row number of the region's upper left corner.
     * \param column The column number of the region's upper left corner.
     * \param width The width of the region in columns.
     * \param height The height of the region in rows.
     * \param attribute The color attribute to use in the cleared region.
     */
    void clear( int row, int column, int width, int height, int attribute )
    {
        char *screen_pointer;
        char *destination;
        int   i;

        adjust_dimensions( &row, &column, &width, &height );
        attribute = convert_attribute( attribute );

        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        for( ; height > 0; height-- ) {
            destination = screen_pointer;
            for( i = 0; i < width; i++ ) {
                *destination++ = ' ';
                *destination++ = static_cast< char >( attribute );
            }
            screen_pointer += 2 * NMBR_COLS;
        }
    }


    //! Changes the color of a region of the screen.
    /*!
     * This function is similar to clear( ) except that it does not erase any of the text in the
     * region. It simply changes the color attribute for every character in the region to \p
     * attribute.
     *
     * \param row The row number of the region's upper left corner.
     * \param column The column number of the region's upper left corner.
     * \param width The width of the region in columns.
     * \param height The height of the region in rows.
     * \param attribute The color attribute to use in the region.
     */
    void set_color( int row, int column, int width, int height, int attribute )
    {
        char *screen_pointer;
        char *destination;
        int   i;

        adjust_dimensions( &row, &column, &width, &height );
        attribute = convert_attribute( attribute );

        screen_pointer = screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
        for( ; height > 0; height-- ) {
            destination = screen_pointer;
            for( i = 0; i < width; i++ ) {
                destination++;
                *destination++ = static_cast< char >( attribute );
            }
            screen_pointer += 2 * NMBR_COLS;
        }
    }


    //! Scroll a region.
    /*!
     * This function scrolls a region. Note that "UP" means that the text and attributes of the
     * specified region move up on the screen. The new color attribute used in the lines that
     * are cleared is \p attribute. If \p number_of_rows is greater than or equal to \p height,
     * the entire region is cleared as if a call was made to clear( ).
     *
     *  \param Direction The direction to scroll. Must be either scr::UP or scr::DOWN.
     *  \param row The row number of the region's upper left corner.
     *  \param column The column number of the region's upper left corner.
     *  \param width The width of the region in columns.
     *  \param height The height of the region in rows.
     *  \param number_of_rows The number of rows to scroll.
     *  \param attribute The color attribute to use in the newly "exposed" space.
     */
    void scroll( direction_t direction,
                 int row, int column, int width, int height, int number_of_rows, int attribute )
    {
        char *screen_pointer;
        char *source_pointer;
        int   row_counter;

        if( number_of_rows <= 0 ) return;
        adjust_dimensions( &row, &column, &width, &height );
        attribute = convert_attribute( attribute );

        // If we're trying to scroll too much, just clear the region and return.
        if( number_of_rows >= height ) {
            clear( row, column, width, height, attribute );
            return;
        }

        if( direction == UP ) {
            screen_pointer =
                screen_image + ( ( row - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
            source_pointer = screen_pointer + ( number_of_rows * 2 * NMBR_COLS );
            for( row_counter = 0; row_counter < height - number_of_rows; row_counter++ ) {
                std::memcpy( screen_pointer, source_pointer, 2 * width );
                screen_pointer += 2 * NMBR_COLS;
                source_pointer += 2 * NMBR_COLS;
            }
            clear( row + ( height - number_of_rows ), column, width, number_of_rows, attribute );
        }

        // Otherwise we're trying to scroll down.
        else {
            screen_pointer =
                screen_image + ( ( row - 1 + height - 1 ) * 2 * NMBR_COLS ) + ( column - 1 ) * 2;
            source_pointer = screen_pointer - ( number_of_rows * 2 * NMBR_COLS );
            for( row_counter = 0; row_counter < height - number_of_rows; row_counter++ ) {
                std::memcpy( screen_pointer, source_pointer, 2 * width );
                screen_pointer -= 2 * NMBR_COLS;
                source_pointer -= 2 * NMBR_COLS;
            }
            clear( row, column, width, number_of_rows, attribute );
        }
    }


    //! Move the cursor to a new position.
    /*!
     * If an attempt is made to move the cursor off the screen, its motion is clipped to stay on
     * the screen.
     *
     * \param row The new cursor row position.
     * \param column The new cursor column position.
     */
    void set_cursor_position( int row, int column )
    {
        // Be sure the coordinates are in bounds.
        if( row < 1 )            row    = 1;
        if( row > NMBR_ROWS )    row    = NMBR_ROWS;
        if( column < 1 )         column = 1;
        if( column > NMBR_COLS ) column = NMBR_COLS;

        virtual_row = row;
        virtual_column = column;
    }


    //! Retrieves the cursor's current position.
    /*!
     * \param row <b>[out]</b> Points to a variable that receives the cursor's current row.
     * \param column <b>[out]</b> Points to a variable that receives cursor's current column.
     */
    void get_cursor_position( int *row, int *column )
    {
        *row    = virtual_row;
        *column = virtual_column;
    }


    //+++++
    // System dependent functions
    //+++++

    // There are three subsections below. The first applies to systems that do "direct" video
    // access. The second uses only ANSI escape sequences to update the display. The third uses
    // curses.

    /*! \fn void scr::clear_screen( )
     * \brief Fill the entire screen with spaces.
     *
     *  This function fills the screen with WHITE|REV_BLACK attributes. On some systems it might
     *  use special techniques to clear the screen and hence be faster than an equivalent clear(
     *  ) call. Note that, unlike the other scr functions, this function's effects are
     *  immediate. You do not need to call refresh( ).
     */

    /*! \fn void scr::redraw( )
     *  \brief Redraw the entire screen.
     *
     *  Unlike refresh( ), this function always updates every character position on the screen.
     *  As a result it is likely to be slower than refresh( ). You should use this function only
     *  when you want to recover the screen completely.
     */

    /*! \fn void scr::refresh( )
     *  \brief Synchronize the physical screen with scr's internal buffer.
     *
     *  This function causes the phyiscal screen to display the same contents as is stored in
     *  scr's internal buffer. With the exception of clear_screen( ), the effects of no scr
     *  function is evident until refresh( ) (or redraw( )) is called. On some systems, this
     *  function might use a clever approach to updating the display so as to minimize the
     *  number of characters that are actually changed.
     */

    /*! \fn void scr::off( )
     *  \brief Shuts down scr-mediated operation of the screen temporarily.
     *
     *  This function reverts the display to normal stream I/O. No further use of scr should be
     *  made except for on( ). This function is intended to be used when "shelling out" of a scr
     *  based program.
     */

    /*! \fn void scr::on( )
     *  \brief Restores scr-mediated operation of the screen.
     *
     *  This function undoes the effect of off( ). It is intended to be used when returning from
     *  an external shell.
     */

#if defined(SCR_DIRECT)

    void clear_screen( )
    {
        // This will update both the screen image and the physical screen.
        clear( 1, 1, NMBR_COLS, NMBR_ROWS, WHITE|REV_BLACK );
        set_cursor_position( 1, 1 );
        refresh( );
    }


    void redraw( )
    {
#if (eOPSYS == eDOS)
        union REGS r;
        char far *screen_pointer = ( char far * )screen_image;

#ifndef __386__
        // Use movedata() if we are not using the DOS/4GW DOS extender.
        // Put the text and attributes into video RAM.
        movedata(
            FP_SEG( screen_pointer ),
            FP_OFF( screen_pointer ),
            screen_segment,
            0x0000,
            NMBR_ROWS * 2 * NMBR_COLS);
#else
        // Just copy stuff to raw linear addresses if we are using the extender.
        std::memcpy( ( char * )0x000B8000, screen_image, NMBR_ROWS * 2 * NMBR_COLS );
#endif

        // Position the cursor to the correct location.
        r.h.ah = ( unsigned char ) SET_CURSORPOS;
        r.h.dh = ( unsigned char ) ( virtual_row - 1 );
        r.h.dl = ( unsigned char ) ( virtual_column - 1 );
        r.h.bh = ( unsigned char ) 0;
        do_interrupt( BIOS_VIDEO, &r, &r );
#endif

#if (eOPSYS == eOS2)
        // Put the text and attributes into video RAM.
        VioWrtCellStr( screen_image, NMBR_ROWS * 2 * NMBR_COLS, 0, 0, 0 );

        // Position the cursor to the correct location.
        VioSetCurPos( virtual_row - 1, virtual_column - 1, 0 );
#endif

#if (eOPSYS == eWIN32)
        // This assumes NMBR_COLS and NMBR_ROWS are both < 32k.
        COORD  where = { 0, 0 };
        COORD  size  = { static_cast< SHORT >( NMBR_COLS ),
                         static_cast< SHORT >( NMBR_ROWS ) };

        SMALL_RECT rwhere = { 0, 0,
                              static_cast< SHORT >( NMBR_COLS - 1 ),
                              static_cast< SHORT >( NMBR_ROWS - 1 ) };
        HANDLE hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );

        // Copy the screen image to the console image.
        for( int cc = 0; cc < NMBR_ROWS * NMBR_COLS; cc++ ) {
            console_image[cc].Char.UnicodeChar = 0;
            console_image[cc].Char.AsciiChar = screen_image[cc * 2];
            console_image[cc].Attributes = screen_image[cc * 2 + 1];
        }

        WriteConsoleOutput(
            hStdOut,                       // Which console
            ( CHAR_INFO * )console_image,  // Buffer address
            size,                          // Screen size of buffer image
            where,                         // Starting point (upper left)
            &rwhere                        // Where actually written
        );

        // This assumes virtual_column and virtual_row both < 32k.
        where.X = static_cast< SHORT >( virtual_column - 1 );
        where.Y = static_cast< SHORT >( virtual_row - 1 );
        SetConsoleCursorPosition( hStdOut, where );
#endif
    }


    void refresh( )
    {
        redraw( );
    }


    void off( )
    {
        // Do nothing. (Is this really right?)
    }


    void on( )
    {
        // Do nothing. (Is this really right?)
    }

#endif


#if defined(SCR_ANSI)

    static int foreground_table[] = {
        /* 0 => Black   */ F_BLACK,
        /* 1 => Blue    */ F_BLUE,
        /* 2 => Green   */ F_GREEN,
        /* 3 => Cyan    */ F_CYAN,
        /* 4 => Red     */ F_RED,
        /* 5 => Magenta */ F_MAGENTA,
        /* 6 => Brown   */ F_YELLOW,
        /* 7 => White   */ F_WHITE
    };

    static int background_table[] = {
        /* 0 => Black   */ B_BLACK,
        /* 1 => Blue    */ B_BLUE,
        /* 2 => Green   */ B_GREEN,
        /* 3 => Cyan    */ B_CYAN,
        /* 4 => Red     */ B_RED,
        /* 5 => Magenta */ B_MAGENTA,
        /* 6 => Brown   */ B_YELLOW,
        /* 7 => White   */ B_WHITE
    };


    void clear_screen( )
    {
        int counter;

        // Clear the physical screen.
        reset_screen( );
        erase_screen( );

        // Make the arrays correct.
        for( counter = 0; counter < NMBR_ROWS * 2 * NMBR_COLS; counter += 2 ) {
            screen_image[counter]        = ' ';
            screen_image[counter + 1]    = WHITE|REV_BLACK;
            physical_image[counter]      = ' ';
            physical_image[counter + 1]  = WHITE|REV_BLACK;
        }

        // Position the cursor. Record the location of the real cursor.
        position_cursor( 1, 1 );
        virtual_row     = 1;
        virtual_column  = 1;
        physical_row    = 1;
        physical_column = 1;
    }


    void redraw( )
    {
        int     current_attribute; // Color/attribute of last character printed.
        int     row_count;
        int     column_count;
        char   *current_character; // Points at next character in the screen
                                   // image that we want to print.

        // Scan over the entire screen, one row at a time.
        for( row_count = 1; row_count <= NMBR_ROWS; row_count++ ) {

            // Put the cursor at the start of the row and force the current color/attribute
            // state to something we know.
            //
            current_attribute = WHITE|REV_BLACK;
            position_cursor( row_count, 1 );
            reset_screen( );

            // Point into the screen image.
            current_character = screen_image + ( row_count - 1 ) *  2 * NMBR_COLS;

            // Scan down the row...
            for( column_count = 1; column_count <= NMBR_COLS; column_count++ ) {

                // If the current character's color is different than the current state, change
                // color/attribute as needed.
                //
                if( current_attribute != current_character[1] ) {
                    reset_screen( );
                    if( current_character[1] & BRIGHT ) bold_on( );
                    if( current_character[1] & BLINK  ) blink_on( );
                    set_color( foreground_table[current_character[1] & 0x07] );
                    set_color( background_table[( current_character[1] & 0x70 ) >> 4] );
                    current_attribute = current_character[1];
                }

                // Print the character.
                std::putchar( *current_character );
                current_character += 2;
            }
        }

        // Ok. We're done with the screen. Now we've got to position the cursor and reset the
        // screen.
        //
        position_cursor( virtual_row, virtual_column );
        reset_screen( );
    }


    void refresh( )
    {
        int   current_attribute;    // Current printing attribute.
        int   desired_attribute;    // The total attribute that we want.
        int   desired_foreground;   // The ANSI color number of what we want.
        int   desired_background;   // The ANSI color number of what we want.
        int   row_count;
        int   column_count;

        // Set the colors to a known value.
        reset_screen( );
        current_attribute = WHITE|REV_BLACK;

        // Loop over the entire screen.
        for( row_count = 1; row_count <= NMBR_ROWS; row_count++ ) {
            for( column_count = 1; column_count <= NMBR_COLS; column_count++ ) {

                // Compute offset into the image arrays of the current character.
                int array_index = ( row_count - 1 ) * 2 * NMBR_COLS + 2 * ( column_count - 1 );

                // If this character is already what we want, skip it.
                if( screen_image[array_index]     == physical_image[array_index] &&
                    screen_image[array_index + 1] == physical_image[array_index + 1] )
                    continue;

                // Otherwise, position the cursor if it's in the wrong place.
                if( row_count != physical_row || column_count != physical_column ) {
                    position_cursor( row_count, column_count );
                    physical_row = row_count;
                    physical_column = column_count;
                }

                // Is the current color what we want? If not, adjust it.
                desired_attribute  = screen_image[array_index + 1];

                if( desired_attribute != current_attribute ) {
                    reset_screen( );
                    desired_foreground = foreground_table[  desired_attribute & 0x07];
                    desired_background = background_table[( desired_attribute & 0x70 ) >> 4];
                    set_color( desired_foreground );
                    set_color( desired_background );
                    if( desired_attribute & BRIGHT ) bold_on( );
                    if( desired_attribute & BLINK ) blink_on( );
                    current_attribute = desired_attribute;
                }

                // Print the character! (And adjust our record of the cursor position).
                std::putchar( screen_image[array_index] );
                physical_column++;

                // Update the physical image.
                physical_image[array_index]     = screen_image[array_index];
                physical_image[array_index + 1] = screen_image[array_index + 1];
            }
        }

        // Position the cursor to its final resting place.
        position_cursor( virtual_row, virtual_column );
        physical_row = virtual_row;
        physical_column = virtual_column;
    }


    void off( )
    {
        // Do nothing. (Is this really right?)
    }


    void on( )
    {
        // Do nothing. (Is this really right?)
    }

#endif


#if eOPSYS == ePOSIX

    void clear_screen( )
    {
        int counter;

        // Clear the physical screen.
        werase( stdscr );

        // Make the arrays correct.
        for( counter = 0; counter < NMBR_ROWS * 2 * NMBR_COLS; counter += 2 ) {
            screen_image[counter]        = ' ';
            screen_image[counter + 1]    = WHITE|REV_BLACK;
            physical_image[counter]      = ' ';
            physical_image[counter + 1]  = WHITE|REV_BLACK;
        }

        // Position the cursor. Record the location of the real cursor.
        move( 0, 0 );
        virtual_row     = 1;
        virtual_column  = 1;
        physical_row    = 1;
        physical_column = 1;

        // Tell curses to do the update on the "real" physical screen.
        ::refresh( );
    }


    void redraw( )
    {
        int     row_count;
        int     column_count;
        char   *current_character;   // Points at next character in the screen
                                     // image that we want to print.

        // Scan over the entire screen, one row at a time.
        for( row_count = 1; row_count <= NMBR_ROWS; row_count++ ) {

            // Put the cursor at the start of the row.
            move( row_count - 1, 0 );

            // Point into the screen image.
            current_character = screen_image + ( row_count - 1 ) * 2 * NMBR_COLS;

            // Scan down the row...
            for( column_count = 1; column_count <= NMBR_COLS; column_count++ ) {

                // Print the character.
                chtype new_character;
                CharacterMap::iterator p = box_character_map.find( *current_character );
                if( p == box_character_map.end( ) )
                    new_character = ( unsigned char )*current_character;
                else
                    new_character = box_character_map[*current_character];

                if( *( current_character + 1 ) & BLINK )  new_character |= A_BLINK;
                if( *( current_character + 1 ) & BRIGHT ) new_character |= A_BOLD;

                // Add color information if we are supposed to.
                if( color_works ) {
                    int just_color = *( current_character + 1 ) & ~( BLINK|BRIGHT );
                    ColorMap::iterator p = colors_map.find( just_color );
                    if( p != colors_map.end( ) ) {
                        new_character |= COLOR_PAIR( colors_map[just_color] );
                    }
                }

                // Finally! Print the character.
                addch( new_character );
                current_character += 2;
            }
        }

        // Ok. We're done with the screen. Now we've got to position the cursor and reset the
        // screen.
        //
        move( virtual_row - 1, virtual_column - 1 );

        // Tell curses to do the update on the "real" physical screen.
        ::refresh( );
    }


    void refresh( )
    {
        int   row_count;
        int   column_count;

        // Loop over the entire screen.
        for( row_count = 1; row_count <= NMBR_ROWS; row_count++ ) {
            for( column_count = 1; column_count <= NMBR_COLS; column_count++ ) {

                // Compute offset into the image arrays of the current character.
                int array_index = ( row_count - 1 ) * 2 * NMBR_COLS + 2 * ( column_count - 1 );

                // If this character is already what we want, skip it.
                if( screen_image[array_index]   == physical_image[array_index]  &&
                    screen_image[array_index+1] == physical_image[array_index+1] )
                    continue;

                // Otherwise, position the cursor if it's in the wrong place.
                if( row_count != physical_row || column_count != physical_column ) {
                    move( row_count - 1, column_count - 1 );
                    physical_row = row_count;
                    physical_column = column_count;
                }

                // Print the character! (And adjust our record of the cursor position.
                chtype new_character;
                CharacterMap::iterator p = box_character_map.find( screen_image[array_index] );
                if( p == box_character_map.end( ) )
                    new_character = ( unsigned char )screen_image[array_index];
                else
                    new_character = box_character_map[screen_image[array_index]];

                // Add the attributes if necessary.
                if( screen_image[array_index + 1] & BLINK )  new_character |= A_BLINK;
                if( screen_image[array_index + 1] & BRIGHT ) new_character |= A_BOLD;

                // Add color information if we are supposed to.
                if( color_works ) {
                    int just_color = screen_image[array_index+1] & ~( BLINK|BRIGHT );
                    ColorMap::iterator p = colors_map.find( just_color );
                    if( p != colors_map.end( ) ) {
                        new_character |= COLOR_PAIR( colors_map[just_color] );
                    }
                }

                // Finally! Print the character.
                addch( new_character );
                physical_column++;

                // Update the physical image.
                physical_image[array_index]     = screen_image[array_index];
                physical_image[array_index + 1] = screen_image[array_index + 1];
            }
        }

        // Position the cursor to its final resting place.
        move( virtual_row - 1, virtual_column - 1 );
        physical_row    = virtual_row;
        physical_column = virtual_column;

        // Tell curses to do the update on the "real" physical screen.
        ::refresh( );
    }


    void off( )
    {
        reset_shell_mode( );
        putp( exit_ca_mode );
    }


    void on( )
    {
        putp( enter_ca_mode );
        reset_prog_mode( );
        ::refresh( );
    }

#endif

}
