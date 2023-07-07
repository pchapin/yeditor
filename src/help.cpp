/*! \file    help.cpp
 *  \brief   Text of the help screens.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * The help text is out of date; it needs a major overhaul.
 *
 * This file contains only the declaration of several large arrays of char pointers. The text
 * defined by these arrays form all of the help screens used by Y. The format is not the easiest
 * to process, but it is easy to produce and change.
 */

#include <cstdio>

#include "help.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "TextWindow.hpp"

static const char *const help_1[] = {
    "HELP ON HELP",
    "",
    "PgUp/PgDn      Page through the help screens.",
    "ESC            Resume editing.",
    "",
    "The commands above apply to all the sets of help screens.",
    "These include the screens accessible via the Y commands",
    "below.",
    "",
    "Shift+F1       Command help",
    "Shift+F2       Editor notes and acknowledgments",
    "Shift+F3       License information and legal notes.",
    "",
    "Shift+F5       Technical information on Y\'s file list.",
    "Shift+F6       Technical information on the current file.",
    NULL
};

static const char *const help_2[] = {
    "INVOKING AND TERMINATING",
    "",
    "The Y command line may contain a list of files to load.",
    "The list may contain names with wildcard characters. If",
    "just an extension is specified, the base name of the",
    "previous file on the list will be used.",
    "",
    "Typing \"Y\" alone will cause the editor to read a",
    "previously saved state, if any, and reload all files",
    "described by that state.",
    "",
    "Alt+Y          Exit, save changes, save state information.",
    "Alt+X          Exit, save changes.",
    "Alt+Q          Exit, abandon changes.",
    NULL
};

static const char *const help_3[] = {
    "MOVING THE CURSOR",
    "",
    "The cursor can be moved off the end of the file. Y extends",
    "the current line and adds blank lines as needed.",
    "",
    "Arrows         Moves the cursor the expected way.",
    "PgDn/PgUp      Next/Previous screen.",
    "Home           Start of the current line.",
    "End            Just past the end of the current line.",
    "",
    "Ctrl+Right/Left Arrow    Next/Previous word.",
    "Ctrl+Home                Top of file.",
    "Ctrl+End                 Just past the bottom of file.",
    "Ctrl+PgDn/PgUp           Next/Previous procedure.",
    "",
    "F9                       Jump to line number.",
    "Alt+F9                   Jump to column number.",
    NULL
};

static const char *const help_4[] = {
    "CREATING AND DESTROYING TEXT",
    "",
    "If block mode is on, these operations occur on every line",
    "in the block. In replace mode only Delete and Ctrl+RETURN",
    "destroy characters.",
    "",
    "Printable character      Insert/Replace at current point.",
    "Backspace                Delete character to the left.",
    "Delete                   Delete current character.",
    "Ctrl+RETURN              Delete to end of current line.",
    "Ctrl+Backspace           Delete to start of line.",
    "Tab                      Jump to next tab stop.",
    "Insert                   Toggle insert/replace mode.",
    "",
    "Certain control characters must be prefixed with ^Q  to be",
    "put into the file. Namely  ^E, ^I (TAB), ^K, ^M (CR),  ^Q,",
    "^R, and ^[ (ESC). Don\'t try to put ^J or ^@ into the file.",
    NULL
};

static const char *const help_5[] = {
    "HANDLING FILES",
    "",
    "Whenever files (or blocks) are saved, Y strips trailing",
    "whitespace from each line. Y expands tabs to spaces when",
    "reading files.",
    "",
    "F1        Edit another file (wildcards ok).",
    "F2        Save current file or save block to a new file.",
    "F3        Switch to the next file in Y\'s file list.",
    "F4        Remove file from list, saving it.",
    "Alt+F1    Reload a fresh copy of file from disk.",
    "Alt+F2    Rename the current file or block.",
    "Alt+F3    Switch to the previous file in Y\'s file list.",
    "Alt+F4    Remove file or block without saving.",
    "",
    "F8        Insert a file into the current file.",
    "Alt+F8    Insert current file or block into a file.",
    NULL
};

static const char *const help_6[] = {
    "BLOCK MODE",
    "",
    "Many commands act differently  when block mode is on. In",
    "general, commands that insert the clipboard will delete",
    "(forever) any currently active block.",
    "",
    "F5                  Toggle block mode.",
    "ESC                 Turn block mode off.",
    "",
    "Alt+F4              Cut current block forever.",
    "F6                  Cut current block/line to clipboard.",
    "Alt+F6              Copy current block/line to clipboard.",
    "F7                  Insert clipboard above cursor.",
    "",
    "If block mode is not on, F6 will cut the current line to",
    "the clipboard. Alt+F6 works similarly.",
    NULL
};

static const char *const help_7[] = {
    "USING EXTERNAL COMMANDS",
    "",
    "When an external command is run, Y first saves all changed",
    "files. After the command finishes, Y reloads from disk any",
    "files changed by the command.",
    "",
    "F10       Run an external program. RETURN at prompt gives",
    "          a new shell. Use EXIT to return to Y.",
    "",
    "Alt+F10   Filter file or block through an external filter",
    "          program. Replace any currently defined block.",
    "",
    "Ctrl+F10  Insert STDOUT of external program into file.",
    "          Replace any currently defined block.",
    "",
    "Shft+F10  Pipe file or block into STDIN of external",
    "          program.",
    NULL
};

static const char *const help_8[] = {
    "SEARCH AND REPLACE",
    "",
    "Ctrl+F1   Set search string and search for first match.",
    "Ctrl+F2   Search for next match.",
    "Ctrl+F3   Search and replace.",
    "",
    "When doing a search and replace, for each match the",
    "following options are presented.",
    "",
    "  \'Y\'     Do the replacement and continue (default).",
    "  \'N\'     No replacement, but look for next match.",
    "  \'A\'     Do all replacements without further prompting.",
    "  ESC     Abort the operation.",
    "",
    "If block mode is on, a search and replace operation is",
    "done only over the block.",
    NULL
};

static const char *const help_9[] = {
    "KEYBOARD MACROS",
    "",
    "^R   Repeat prefix. All keystrokes can be repeated. This",
    "     includes command keys. After the ^R type a count (not",
    "     displayed). The first non digit key typed will be",
    "     repeated. For example \"^R75=\". To repeat a digit, use",
    "     ^Q to quote the digit.",
    "",
    "^K   Toggle keyboard macro learn mode. Keystrokes entered",
    "     after the first ^K are remembered in the macro",
    "     buffer. The second ^K turns off the learning.",
    "",
    "^E   Playback (execute) a previously learned keyboard",
    "     macro.",
    "",
    "Keyboard macros can contain repeat sequences. Keyboard",
    "macros can be repeated.",
    NULL
};

static const char *const help_10[] = {
    "MISC COMMANDS",
    "",
    "Alt+B     Change background color of the current file.",
    "",
    "Alt+F     Change foreground color of the current file.",
    "",
    "Alt+R     Reformats the current paragraph. Long lines are",
    "          wrapped and short lines are filled.",
    "",
    "Alt+T     Change tab stop distance for the current file.",
    NULL
};

const HelpScreen h_screens[] = {
    { help_1,  &h_screens[ 1], &h_screens[ 9] },
    { help_2,  &h_screens[ 2], &h_screens[ 0] },
    { help_3,  &h_screens[ 3], &h_screens[ 1] },
    { help_4,  &h_screens[ 4], &h_screens[ 2] },
    { help_5,  &h_screens[ 5], &h_screens[ 3] },
    { help_6,  &h_screens[ 6], &h_screens[ 4] },
    { help_7,  &h_screens[ 7], &h_screens[ 5] },
    { help_8,  &h_screens[ 8], &h_screens[ 6] },
    { help_9,  &h_screens[ 9], &h_screens[ 7] },
    { help_10, &h_screens[ 0], &h_screens[ 8] }
};

static const char *const editor_1[]={
    "                             Y",
    "                A Programmer\'s Text Editor",
    "",
    "Y is a fast, lightweight text editor suitable for editing",
    "programs, configuration files, and text documents of all",
    "kinds. Y is written in C++ and can be compiled for DOS",
    "Windows, OS/2, and Linux.",
    "",
    "Y was written to support my own programming needs and the",
    "needs of the students at Vermont Technical College in",
    "Randolph Center, VT. I\'d like to thank the students and",
    "faculty of VTC  for finding so many bugs in the earlier",
    "versions of this program.",
    NULL
};

const HelpScreen e_screens[] = {
    { editor_1, &e_screens[0], &e_screens[0] }
};

static const char *const legal_1[]={
    "                  LICENSE INFORMATION",
    "",
    "This program is free software; you can redistribute it and",
    "or modify it under the terms of the GNU General Public",
    "License as published by the Free Software Foundation;",
    "either version 2 of the License, or (at your option) any",
    "later version.",
    "",
    "This program is distributed in the hope that it will be",
    "useful, but WITHOUT ANY WARRANTY; without even the implied",
    "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR",
    "PURPOSE. See the GNU General Public License for more",
    "details.",
    NULL
};

static const char *const legal_2[]={
    "                  CONTACT INFORMATION",
    "",
    "Y\'s author can be reached at",
    "",
    "     Peter C. Chapin",
    "     Vermont Technical College",
    "     Williston, VT 05495 (USA)",
    "     chapinp@acm.org",
    NULL
}; 

const HelpScreen l_screens[] = {
    { legal_1, &l_screens[1], &l_screens[1] },
    { legal_2, &l_screens[0], &l_screens[0] }
};


const HelpScreen *display_screens
    ( const HelpScreen *const base, const HelpScreen *current, const int size )
{
    const int viewer_width = 60;   // If these change, the data above will need to be reformated
    const int viewer_height = 20;
    scr::TextWindow view_port;

    view_port.open(
        ( scr::number_of_rows( )/2 - viewer_height/2 ) + 1,
        ( scr::number_of_columns( )/2 - viewer_width/2 ) + 1,
        viewer_width,
        viewer_height,
        scr::BLACK | scr::REV_WHITE, scr::DOUBLE_LINE );

    // Turn cursor off.
    scr::set_cursor_position( scr::number_of_rows( ) + 1, 1 );

    // Loop until the user says "enough!!"
    for( ;; ) {

        // Write the screen number in the lower left corner.
        char buffer[20];
        std::sprintf( buffer, "%2d/%2d", static_cast<int>( current - base ) + 1, size );
        view_port.print_at( viewer_height - 3, viewer_width - 8, buffer );

        // Paint the help screen.
        int i;
        bool stop = false;
        view_port.home( );
        for( i = 1; !stop && i <= viewer_height - 2; i++ ) {
            if( current->current_screen[i-1] == NULL )
                stop = true;
            else
                view_port.print( current->current_screen[i-1] );
        }
        
        int ch;
        switch( ch = scr::key( ) ) {
        case scr::K_PGDN:
            current = current->next_screen;
            break;

        case scr::K_PGUP:
            current = current->previous_screen;
            break;

        default:
            // We intentionally ignore all other keys.
            ;
        }

        if( ch == scr::K_PGUP  ||  ch == scr::K_PGDN ) {
            scr::clear(
                view_port.row( ),   view_port.column( ),
                view_port.width( ), view_port.height( ),
                view_port.color( )
            );
        }

        // Break out of infinite loop if user's seen enough.
        if( ch == scr::K_ESC ) break;
    }
    return current;
}
