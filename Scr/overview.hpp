/*! \file    overview.hpp
    \brief   Overview documentation of the scr library.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

/*!

\mainpage

\section Introduction

<p>This library contains a number of screen and keyboard handling functions at both a low and
high level. The functions are suitable for dealing with a "text mode" screen where the screen is
divided into fixed sized character positions. This is not a graphics package. Functions are
provided for both reading the screen as well as writing to the screen. Functions are provided
for dealing with strings as well as blocks of text. Functions are provided to move the cursor.
Finally, functions are provided to get keystrokes from the user.</p>

<p>One of Scr's main features is support for various operating systems and screen handling
techniques. At the time of this writing Scr supports DOS, OS/2, Win32, and Unix/Posix systems
using curses. Programs that use Scr will display correctly on the text displays of all these
operating systems. Furthermore keyboard input will be handled in a uniform manner. Scr was
originally created under DOS where it did direct video access. The design of Scr reflects this
heritage.</p>

<p>Scr's model of the screen is one of delayed action. All functions that output to the screen
only update Scr's internal representation of the screen. You must call the function scr::Refresh
to actually synchronize the real screen with Scr's internal representation.</p>

<p>Scr makes no promises about the size of the screen. You must call the functions scr::NmbrRows
and scr::NmbrCols to learn about the screen's real dimensions. A correct program based on Scr
should never assume anything about the size of the screen. If your program requires screens of a
certain size, you should exit with an error message if the size is inappropriate. The current
version of Scr does assume that the screen size will not change during the lifetime of the
program. This is not necessarily true on some platforms. A future version of Scr may address
this issue.</p>

\section Compiling

<p>Scr uses several macros to control its compilation. These are in addition to the symbols
defined in environ.h (see environ.h for more information). The symbols used specifically by Scr
are as follows.</p>

<dl>

<dt><b>SCR_ANSI</b> and <b>SCR_DIRECT</b>

<dd><p>These symbols define which screen handling method you want Scr to use. If SCR_ANSI is
defined, Scr will use ANSI escape sequences to position the cursor and select colors. This tends
to be highly portable and is a good choice if you are attempting to get Scr to work on a
platform which is not specifically supported. Scr will try to move the cursor somewhat optimally
when you specify SCR_ANSI so screen updates are reasonably fast despite the overhead.</p>

<p>If SCR_DIRECT is defined, Scr will use "direct" video access in a way that makes sense for
the platform you are using. In the original MS-DOS implementation of Scr, direct video access
was exactly that. However, on the currently supported platforms "direct" video access involves
going through some appropriate, specialized operating system API. Nevertheless this is often
faster than using the SCR_ANSI approach.</p>

<p>You must specify either SCR_ANSI or SCR_DIRECT when you compile Scr. There is no default.</p>

<dt><b>SCR_ASCIIBOXES</b>

<dd><p>When SCR_ASCIIBOXES is defined, Scr will only use ASCII characters for box drawing
characters. This will be true even if the host environment supports box drawing characters
normally. Scr will still honor requests to use fancy boxes, but it will translate all such
requests into plain ASCII character boxes.</p>

<dt><b>SCR_ASCIIKEYS</b>

<dd><p>When SCR_ASCIIKEYS is defined, Scr will simulate the special keys (function keys, alt
keys, arrow keys, etc) using control characters. Note that the POSIX version of Scr normally
supports this simulation in addition to honoring whatever special characters the current
terminal can supply. Since not all terminals can supply all special characters, the ASCIIKEYS
option is always active when the environment is POSIX. However, if you explicit define
SCR_ASCIIKEYS, then <em>only</em> the simulated keys are active.</p>

</dl>

\section Defining Screen Regions

<p>The upper left corner of the screen is row 1, column 1. The maximum supported row and column
numbers are determined by Scr when you call scr::Initialize. You must use the functions
scr::NmbrRows and scr::NmbrCols at run-time to learn these values. A screen region is defined by
specifying the row and column of the upper left corner of the region followed by the size of the
region. The size is always given as the width in columns followed by the height in rows.</p>

<p>The functions are guaranteed to work only for regions sized between 1x1 and the entire
screen. Do not attempt to use them on a zero sized region. Do not specify dimensions that
overlap any of the screen's edges or that have negative sizes.</p>

\section Defining Screen Attributes

<p>A screen attribute is an 8-bit quantity specified by ORing together the following
symbols.</p>

<pre>
WHITE           REV_WHITE
BLACK           REV_BLACK
RED             REV_RED
GREEN           REV_GREEN
BLUE            REV_BLUE
CYAN            REV_CYAN
MAGENTA         REV_MAGENTA
BROWN           REV_BROWN

BLINK
BRIGHT
</pre>

<p>Foreground colors are specified using the first column while background colors are specified
using the second column. Black is the default color. Thus you can specify only a foreground
color if you want the background to be black (or visa versa).</p>

<p>The 8 bits of the attribute are composed of 3 bits for the foreground color, 3 bits for the
background color, and 1 bit each for the blink and the bright attribute. Each color is really
just a combination of the 3 primitive colors red, green, and blue. Each primitive color has one
bit associated with it. A color (or blink or bright) is activated when it's bit is on. For
example, MAGENTA is really just RED | BLUE. Similarly WHITE is just RED | GREEN | BLUE.</p>

<p>Although attribute parameters are generally declared as int, only the lower 8 bits of the int
(the unsigned char part) is used. Note that when the attributes are stored in the arrays used by
scr::Read and scr::Write, they are stored as plain char.</p>

<p>Some platforms do not support color. In that case the function
scr::IsMonochrome returns true. You can test for that case in your
application and adjust the colors accordingly. The function scr::CvtAttr()
is provided to facilitate converting color attributes from general colors
to colors suitable for a monochrome display.</p>

*/
