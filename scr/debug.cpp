/*! \file    debug.cpp
    \brief   Implementation of embedded debugging system.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This file contains the debugging system. This system is based, loosely, on the debugging system
described in the book "Debugging C" by Robert Ward. This system does not, however, make any
direct use of the code in Ward's system.

<h1>Introduction</h1>

These days fancy external debuggers are normal. Such debuggers operate on a program the way a
surgeon operates on a person. They open the program up against its wishes and examine its
internals directly. This is perfectly reasonable in a hospital operating room, but it is not
something that can be done in the field.

In contrast the debugging system described here is simply another module in the program. By
creating Tracer objects throughout your code you can define entry points to this resident
module. Because the debugging module is part of the program, it becomes possible to do debugging
in the field. For example, you could initialize the debugging system with an undocumented
command, and then walk your end users through a debugging session over the phone while they are
using <em>their</em> system and <em>their</em> data.

One nice feature of the system described here is that the debugging module can call, on your
interactive command, application specific functions (snap shots) that you write when you are
composing your application. These functions are free to do any reasonable manipulation with
global data including, for example, opening and reading files or communicating over the network.
Your end user might execute a snap shot function that uploads data to your server or even that
takes commands from you remotely!

There can be substaintial effort involved in setting up and using this system---especially if
you write complicated snap shot functions. However, by engaging in that effort you are, in
effect, admitting that you may inadvertantly build some bugs into your code. If nothing else,
this system will help you remove bugs by encouraging you to have a constructive attitude about
bugs. When you use this system, you can't just hide your head in the sand and hope.

<h1>Using the Debug System</h1>

There are two aspects to using the system. The first has to do with inserting Tracer objects
into your program and creating snap shot functions. The second has to do with the commands that
are allowed when you executing program makes an entry into the system.

<h2>Inserting Tracer Objects</h2>

All Tracer objects are potentially entry points into the system. In other words, your program
will stop at each Tracer object, display the command window, and allow you to enter debugging
commands. In fact, Tracer objects can be selectively ignored using various methods. The simplest
has to do with the Tracer object's detail level. Using the LEVEL command, you can set the
sensitivity of the system so that only level 1 calls are actually activated. Alternatively, you
could have all levels from 1 to, say, 5 active.

My recommendation is to put a Tracer object at the start and end of all your functions. Use the
object to display that function's parameters. You should make the detail level larger for lower
level functions.

You can also control which Tracer objects are triggered using the START/STOP facility. At the
debugging control window, you can name a particular Tracer object as a "start point" and another
tracer object as a "stop point." Then you can turn tracing off and run the program normally.
When the start point is encountered, tracing will be turned back on (provided the detail level
allows the start point to be seen) and you can trace in the usual way. When the stop point is
encountered, tracing is turned back off and the program runs in an unihibited manner.

The debugging system allows you to define several start and stop points at once.

<h2>Creating Snapshot Functions</h2>

Global data can be conveniently viewed using a snapshot function. These functions are normal
functions that you write specifically to help debug your application. By registering these
functions with register_snapshot( ), you will be able to execute them at any time using the SNAP
command.

There is no restriction on what a snapshot function could do. Such functions could perform some
calculations on the data before displaying it, open and close files, and even terminate the
program. Snapshot functions could display buffers that are being used by ISRs, communicate over
a network, or interact with a serial port. Snapshot functions could even execute the system( )
command and give the user access to operating system services. Snapshot functions are completely
syncronous with the operation of the application; they execute on the same thread as the one
that entered the function that triggered the Tracer in the first place. They thus appear to be
called indirectly from an application function.

<h2>Conditional Compilation</h2>

You may not want all the debug stuff in your final version. However, you probably should keep
the Tracer objects in your source code. Both of these goals can be realized using conditional
compilation. Snapshot functions, for example, could be bracketed by #ifdef DEBUG ... #endif
pairs. Using the macro:
    
<pre>
#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif
</pre>

you can enable or disable single lines of code easily depending on the state of the macro DEBUG.
For example:

<pre>
D( Initialize_Debugging(); )
...
D( Tracer Object(
     Update_List, 2, "Number of list items = %d, Head title = %s", n_items, head->title); )
</pre>

The second example shows the D() macro being used over multiple lines. This is allowed under the
ANSI standard.

<h2>Debug Commands</h2>
    
The following is a complete list of all the commands you can enter into the debugging command
window.

<dl>
<dt>LEVEL n</dt>
<dd>

This command sets the sensitivity of the debugging system. After this command, only Tracer
objects with a detail level <= n will activate the system. Note there is no problem with using 0
or negative values for either n or the detail level in a Tracer object. By default after
initialization the level is 1.

</dd>

<dt>M</dt>
<dd>

This command moves the command window to its "other" position on the screen. It is provided so
that you can move the command window should it be covering something important.

</dd>

<dt>QUIT</dt>
<dd>

This command throws a const char* exception (a message from the debugging system).

</dd>

<dt>SNAP name</dt>
<dd>

This command causes the snapshot function "name" to execute. The name has to be the same (case
significant) as the name used in the register_snapshot() call. Note that in this version it is
not possible to pass parameters to the snapshot function.

</dd>

<dt>START name</dt>
<dd>

This command sets a start point at the Tracer object named "name". When a start point is
encountered, tracing will be activated (at the current level). Thus you can set a start point,
turn tracing off, and run your program normally. When the start point is reached, the command
window will appear.

</dd>

<dt>STATUS</dt>
<dd>

This command displays the current values of your debugging parameters.

</dd>

<dt>STOP name</dt>
<dd>

This command sets a stop point at the Tracer entry named "name". When a stop point is
encountered, tracing will be turned off. By using stop points together with start points, it's
possible to use the system in only a constrained portion of your program.

</dd>

<dt>TRACE ON|OFF</dt>
<dd>

This command allows you to turn tracing on or off. Note that after a 'trace off' command, the
program will run normally unless a start point is encountered.

</dd>

<dt>&lt;Return&gt;</dt>
<dd>

Striking the return key (ie no command) will cause the Tracer object's constructor or destructor
to return and allow your program to continue normally. Repeated taps on the return key thus
allows you to "single step" through your program (actually, from one Tracer object
construction/destruction to the next).

</dd>
</dl>
*/

#include <cctype>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdlib>

#include "debug.hpp"
#include "scr.hpp"
#include "scrtools.hpp"

using namespace std;

//===================================
//          Global Symbols
//===================================

#ifdef DEBUG

#define VERSION  "3.1"

// For Message_Window function.
const int DBG_INTERNAL = 1;
const int DBG_ERROR    = 2;
const int DBG_MESSAGE  = 3;

// The arbitrary limits.
const int MAX_NAME_LEN     =  40; //!< Max length of names.
const int GENERIC_BUF_SIZE = 256; //!< Max length of general purpose buffers.
const int MAX_PARTS        =   5; //!< Max number of parts in a command.
const int MAX_SHOWS        =  10; //!< Max number of show points.
const int MAX_SNAPS        =  10; //!< Max number of snapshot functions.

#endif

//=================================
//          Global Types
//=================================

#ifdef DEBUG

//! Stores information about debugging snapshot functions.
/*!
 * This structure holds information about a single snapshot function. Each snapshot function has
 * a name that the user can use to refer to the function (not necessarly the name of the
 * function itself), and, of course, the function's address.
 */
struct SnapInfo {
    scr::snapshot_function_t function;
    char                     name[MAX_NAME_LEN + 1];
};

//! Saves and restores the cursor position.
/*!
 * This class makes it easy to remember the cursor position. The constructor also makes the
 * cursor "invisible" by moving it to the side.
 */
class CursorMark {
    int old_row;
    int old_column;

public:
    CursorMark( ) { scr::get_cursor_position( &old_row, &old_column ); scr::set_cursor_position( 1, 1 ); }
   ~CursorMark( ) { scr::set_cursor_position(  old_row,  old_column ); }
};

#endif

//================================
//          Global Data
//================================

#ifdef DEBUG
bool debugging_available = true;
#else
bool debugging_available = false;
#endif

#ifdef DEBUG

static int       detail_level    = 1;     //!< Current detail level.
static bool      trace_state     = true;  //!< True if traces are to be done.
static bool      initialized     = false; //!< True when debugging system initialized.
static char      show_names[MAX_SHOWS][MAX_NAME_LEN+1];  //!< Identifies start points.
static SnapInfo  snappers[MAX_SNAPS];     //!< Holds snap shot functions.
static int       number_of_shows = 0;     //!< Number of start points in Start_Names.
static int       number_of_snaps = 0;     //!< Number of snapshot functions in snappers.
static int       window_position;         //!< Location of command window.
static scr::DebugWindow *command_window = 0;  //!< Pointer to the command window.

#endif

//======================================
//          Private Functions
//======================================

#ifdef DEBUG

//! Converts a string to uppercase.
/*!
 * \param buffer Pointer to the string to convert. The string is converted in place.
 */
static void convert_to_uppercase( char *buffer )
{
    while( *buffer ) {
        if( islower( *buffer ) ) *buffer = toupper( *buffer );
        ++buffer;
    }
}


//! Finds the words in a string.
/*!
 * The following function takes a string to process and a string of delimiters as input
 * parameters. It computes pointers to the various words in the input string (as separated by
 * the delimiters). It installs those pointers into a given array. The maximum size of the array
 * must be provided. The function informs its caller of the actual number of substrings found.
 * The function returns an error code. False means undefined failure.
 *
 * \param buffer The string to be split.
 * \param delimiters A string of delimiter characters.
 * \param strings An array of pointers where the results are returned.
 * \param max_number_of_strings The size of the strings array.
 * \param actual_number_of_strings A pointer to an int that receives the number of words found.
 * \return false if there are too many substrings; true otherwise.
 */
static
bool split_string(
    char *buffer,
    const char *delimiters,
    char *strings[],
    int   max_number_of_strings,
    int  *actual_number_of_strings
    )
{
    char *buffer_pointer;         // Temporary pointer into buffer.
    bool  in_string   = false;    // =false when *buffer_pointer not pointing at str.
    bool  return_code = true;     // Returns false if too many substrings.

    // Initialize result and check for out of bounds input.
    *actual_number_of_strings = 0;
    if( max_number_of_strings < 0 ) return_code = false;
    else {

        // Sweep down string until null byte reached.
        buffer_pointer = buffer;
        while( *buffer_pointer != '\0' ) {

            // Try to process the current character only if no error so far.
            if( return_code == true ) {
        
                // If delimiter found while scanning an argument...
                if( ( strchr( delimiters, *buffer_pointer ) != 0 ) && ( in_string == true ) ) {
                    *buffer_pointer = '\0';
                    in_string = false;
                }

                // If non delimiter found while scanning "white" space...
                else if( ( strchr( delimiters, *buffer_pointer ) == 0 ) && ( in_string == false ) ) {

                    // Install only if space. If no space left set error flag.
                    if( *actual_number_of_strings < max_number_of_strings ) {
                        strings[*actual_number_of_strings] = buffer_pointer;
                        ( *actual_number_of_strings )++;
                        in_string = true;
                    }
                    else return_code = false;
                }
            }
            buffer_pointer++;
        } // End of while loop.
    }   // End of overall if... else...
    return return_code;
}


//! Finds commands.
/*!
 * This function searches the list of possible commands to see if the text pointed at by
 * command_string is in the list. The search is case insensitive (the text pointed at by
 * command_string is raised to upper case as a side effect).
 *
 * \param command_string Pointer to the command to find.
 * \return -1 if command_string is not a command, the command index value otherwise.
 */
static int set_command( char *command_string )
{
    int  return_code = -1;  // Command index. -1 implies command not found.
    bool found = false;     // Becomes true if command is found in cmds array.
    int  i;                 // Generic loop counter.

    static const char *commands[] = { // Array of command names.
        "?",
        "LEVEL",
        "M",
        "QUIT",
        "SHOW",
        "SNAP",
        "STATUS",
        "TRACE"
    };
    static int number_of_commands = 8;
    // Number of elements in the above array.

    convert_to_uppercase( command_string );    // Force command to uppercase.
    i = 0;
    while( i < number_of_commands && !found ) {
        if( strcmp( commands[i], command_string ) == 0 ) found = true;
        i++;
    }
    if( found ) return_code = i - 1;
    return( return_code );
}


//! Displays a message.
/*!
 * This function displays a message in the center of the screen via a small window. The
 * errorlevel parameter is one of DBG_INTERNAL, DBG_ERROR, DBG_MESSAGE, depending on the type of
 * message to be displayed (colors change according to the message type).
 */
static void message_window( int error_level, const char *message )
{
    int color;
    static const int int_color     = scr::BRIGHT|scr::BLINK|scr::WHITE|scr::REV_RED;
    static const int error_color   = scr::BRIGHT|scr::WHITE|scr::REV_RED;
    static const int message_color = scr::BRIGHT|scr::WHITE|scr::REV_BROWN;

    switch( error_level ) {
        case DBG_INTERNAL: color = int_color;     break;
        case DBG_ERROR:    color = error_color;   break;
        case DBG_MESSAGE:  color = message_color; break;
        default:
            message_window(DBG_INTERNAL, "Bad errorlevel passed to message_window");
            color = error_color;
            break;
    }

    // Create the window, etc.
    scr::TextWindow the_window;
    int         top_row;
    int         top_column;

    scr::window_center_coordinates( static_cast<int>( strlen( message + 2 ) ), 3, &top_row, &top_column );
    the_window.open(
        top_row,               // Center window.
        top_column,            // Center window.
        static_cast<int>( strlen( message ) + 2 ), // Exact width.
        3,                     // Exact height.
        color,                 // Color.
        scr::SINGLE_LINE       // Border type.
    );

    // Display the message and wait for the user to confirm.
    the_window.print( "%s", message );
    scr::key( );
}


/*!
 * Searches the list of strings (stored in a two dimensional array) looking for the given name.
 * It will search not more than Max_Nmbr strings in the array. It returns -1 if it can't find a
 * match; the index into the array otherwise.
 */
static int in_list(
    const char *name,               // Name to search for.
    char  list[][MAX_NAME_LEN + 1], // Array names in the list.
    int   max_number                // Max number of legit items in the list.
    )
{
    bool found       = false;
    int  return_code = -1;
    int  i           =  0;

    while( i < max_number && !found ) {
        if( strcmp( name, list[i] ) == 0 ) found = true;
        i++;
    }
    if( found ) return_code = i - 1;
    return( return_code );
}


/*!
 * Similar to in_list() above except that it searches an array of SnapInfo structures looking
 * for the snapshot function with the given name.
 */
static int in_snap_list(
    const char *name,      // Name to search for.
    SnapInfo    list[],    // Array names in the list.
    int         max_number // Max number of legit items in the list.
    )
{
    bool found       = false;
    int  return_code = -1;
    int  i           =  0;

    while( i < max_number && !found ) {
        if( strcmp( name, list[i].name ) == 0 ) found = true;
        i++;
    }
    if( found ) return_code = i - 1;
    return( return_code );
}


/*!
 * This function is the one that accepts and processes debugging commands. Notice that it's not
 * responsible for showing or hiding the debug command window.
 */
static void command_loop( )
{
    char  command_buffer[GENERIC_BUF_SIZE+1];  // Buffer for command.
    char  buffer[GENERIC_BUF_SIZE+1];      // General purpose buffer.
    int   done = false;                    // =true when time to exit.
    char *parts[MAX_PARTS];                // Points to portions of com str.
    int   number_of_parts;                 // Number of parts in com str.
    int   i;                               // Generic integer.

    do {
        // Print prompt and get user input.
        command_window->set_row(2);
        command_window->print(">>");
        command_buffer[0] = '\0';
        command_window->get( 2, 3, command_buffer, GENERIC_BUF_SIZE );
    
        // Break command string into parts and act on command.
        split_string( command_buffer, " ", parts, MAX_PARTS, &number_of_parts );

        // If no command, return. No command means the user's done.
        if( number_of_parts == 0 ) done = true;
        else {

            // Look up command number and process it.
            switch( set_command( parts[0] ) ) {

                default:
                    message_window( DBG_ERROR, "Command unknown" );
                    break;

                case 0: {  // HELP (?) command.
                    scr::DebugWindow info_window( "Legal Commands", 40 + 2, 8 + 2 );
                    info_window.print( "?      : List legal commands." );
                    info_window.print( "LEVEL  : Select detail level." );
                    info_window.print( "M      : Move command window." );
                    info_window.print( "QUIT   : throw (char *) exception." );
                    info_window.print( "SHOW   : Set show point." );
                    info_window.print( "SNAP   : Execute snapshot function." );
                    info_window.print( "STATUS : Display status information." );
                    info_window.print( "TRACE  : Turn tracing off or on." );
                }
                break;

                case 1:  // LEVEL command.
                    if( number_of_parts < 2 ) {
                        message_window( DBG_ERROR, "What level?" );
                    }
                    else {
                        detail_level = atoi( parts[1] );
                        sprintf( buffer, "Detail level set to %d", detail_level );
                        message_window( DBG_MESSAGE, buffer );
                    }
                    break;

                case 2:  // MOVE command.
                    if( window_position == DBG_TOP ) {
                        command_window->move( scr::number_of_rows( ) - 5, command_window->column( ) - 1 );
                        window_position = DBG_BOTTOM;
                    }
                    else {
                        command_window->move( 2, command_window->column( ) - 1 );
                        window_position = DBG_TOP;
                    }
                    break;

                case 3:  // QUIT command.
                    throw "Exception thrown from debugging system by user command!";
        
                case 4:  // SHOW command.
                    if( number_of_parts < 2 ) {
                        message_window( DBG_ERROR, "Expected name of show point" );
                    }
                    else if( number_of_shows == MAX_SHOWS ) {
                        message_window( DBG_ERROR, "No space to add another show point" );
                    }
                    else {
                        strncpy( show_names[number_of_shows], parts[1], MAX_NAME_LEN );
                        sprintf( buffer, "Show ON at %s", show_names[number_of_shows] );
                        message_window( DBG_MESSAGE, buffer );
                        number_of_shows++;
                    }
                    break;

                case 5:  // SNAP command.
                    if( number_of_parts < 2 ) {
                        message_window( DBG_ERROR, "Which snapshot function?" );
                    }
                    else if( number_of_snaps == 0 ) {
                        message_window( DBG_ERROR, "No snapshot functions registered" );
                    }
                    else if( ( i = in_snap_list( parts[1], snappers, number_of_snaps ) ) == -1 ) {
                        sprintf( buffer, "Snapshot function %s unknown", parts[1] );
                        message_window( DBG_ERROR, buffer );
                    }
                    else {
                        snappers[i].function( );
                    }
                    break;
        
                case 6:  { // STATUS command.
                    int desired_size = number_of_shows + number_of_snaps + 2;
                    if( desired_size > 20 ) desired_size = 20;
        
                    scr::DebugWindow info_window( "Status", 40 + 2, desired_size + 2 );
                    info_window.print( "TS: %s", trace_state ? "ON" : "OFF" );
                    info_window.print( "DL: %d", detail_level );
        
                    int row = 2;
                    int i;
                    for( i = 0; row < desired_size && i < number_of_shows; row++, i++ ) {
                            info_window.print( "SP: %s", show_names[i] );
                    }
                    for( i = 0; row < desired_size && i < number_of_snaps; row++, i++ ) {
                        info_window.print( "SF: %s", snappers[i].name );
                    }
                }
                break;
        
            case 7:  // TRACE command.
                if( number_of_parts < 2 ) {
                    message_window( DBG_ERROR, "Expected either ON or OFF" );
                }
                else if( convert_to_uppercase( parts[1] ), strcmp( "ON", parts[1] ) == 0 ) {
                    trace_state = true;
                    message_window( DBG_MESSAGE, "Tracing is now ON" );
                }
                else if( strcmp( "OFF", parts[1] ) == 0 ) {
                    trace_state = false;
                    message_window( DBG_MESSAGE, "Tracing is now OFF" );
                }
                else {
                    sprintf( buffer, "Command garbled,  what is '%s' ?", parts[1] );
                    message_window( DBG_ERROR, buffer );
                }
                break;
        
            }
        }

        // Clean up command line.
        command_window->clear( 2 );
    } while( !done );
}

#endif

//=====================================
//          Public Functions
//=====================================

#ifdef DEBUG

namespace scr {

    //! Registers a snap shot function.
    /*!
     * This function allows the client to register snapshot functions with the debugging module.
     * These functions MUST be global functions of type 'void ()' Note that this function can be
     * used even if debugging has not been initialized. It ignores attempts to register too many
     * functions.
     */
    void register_snapshot( const char *name, snapshot_function_t picture )
    {
        // Be sure there's space.
        if( number_of_snaps >= MAX_SNAPS ) return;

        // Install the information about the function and adjust count.
        strncpy( snappers[number_of_snaps].name, name, MAX_NAME_LEN );
        snappers[number_of_snaps].name[MAX_NAME_LEN] = '\0';
        snappers[number_of_snaps].function = picture;

        number_of_snaps++;
    }


    //! Initialize the debugging system.
    /*!
     * This function intializes scr and sets referesh_on_key to true (thus causing a display
     * refresh after every keystroke.
     *
     * \param window_placement Either DBG_TOP or DBG_BOTTOM depending on where the caller wants
     * the debugging window to initially appear.
     */
    void initialize_debugging( int window_placement )
    {
        TextWindow *title_window;

        initialize( );
        refresh_on_key( true );

        CursorMark object;
        
        // Create the command window.
        command_window =
            new DebugWindow( "Debugging Command Window", number_of_columns( ) - 8, 5 );
        if( command_window == 0 ) return;

        // Create the title window.
        title_window = new TextWindow;
        if( title_window != NULL ) {
            title_window->open( command_window->row( ) - 4, command_window->column( ) - 3, 23, 6, BRIGHT|WHITE|REV_MAGENTA, DOUBLE_LINE );
            title_window->print( "C++ Debugging System" );
            title_window->print( "Version " VERSION );
            title_window->print( "" );
            title_window->print( "INITIALIZED" );

            // Let the user see the title window. Then kill it.
            key( );
            delete title_window;
        }

        initialized = true;

        // Position the command window as appropriate.
        if( window_placement == DBG_TOP ) {
            command_window->move( 2, command_window->column( ) - 1 );
            window_position = DBG_TOP;
        }
        else {
            command_window->move( number_of_rows( ) - 5, command_window->column( ) - 1 );
            window_position = DBG_BOTTOM;
        }

        // Process input from user.
        command_loop( );

        // Return to main program.
        command_window->hide( );
    }


    //! Clean up the debugging system.
    /*!
     * This function should be called to shut down the debugging system. After this function has
     * been called it is possible to re-initialize debugging with a later call to
     * initialize_debugging( ).
     */
    void terminate_debugging( )
    {
        delete command_window;
        initialized = false;
        terminate( );
    }


    //! Construct a Tracer; may case a breakpoint.
    /*!
     * The constructor for class Tracer may cause a breakpoint. If the debugging system has not
     * been initialized, the action is a trivial return. Otherwise, a breakpoint is done if
     * appropriate conditions are met. Note that each tracer object has a name and a detail
     * level. The (optional) format string allows the Tracer object to print information at
     * construction time into the debug command window.
     */
    Tracer::Tracer(
        const char *trace_name,    //!< Name of this entry point.
        int   trace_level,         //!< Detail level of this entry point.
        const char *format_string, //!< Printf style format string for trace message.
        ...                        //!< Additional args to satisfy format_string.
    )
    {
        show_trace = false;    // Insure dtor won't trace.
        if( !initialized ) return;

        char    buffer[MAX_NAME_LEN + GENERIC_BUF_SIZE + 1];
        va_list args;

        // Save this information for latter.
        breakpoint_name  = trace_name;
        breakpoint_level = trace_level;

        // Should we take this debug entry seriously?
        if( in_list( trace_name, show_names, number_of_shows ) == -1  &&
            ( trace_state == false || detail_level < trace_level ) ) return;
        show_trace = true;

        CursorMark object;

        // Open command window.
        command_window->show( );

        // Prepare buffer for information output. BUG: problem if buffer overflows!
        va_start( args, format_string );
        if( format_string != NULL )
            vsprintf( buffer, format_string, args );
        else
            buffer[0] = '\0';
        va_end( args );

        // Output the information on row 1.
        command_window->print_at( 1, 0, "%d: %s: %s", trace_level, trace_name, buffer );

        // Get and handle user input.
        command_loop( );

        // Return to main program.
        command_window->clear( 1 );
        command_window->hide( );
    }

#ifdef NEVER


    //! Destructor of Tracer may cause a breakpoint.
    /*!
     * The destructor will cause a breakpoint if and only if the constructor did. This is true
     * regardless of commands subseqently executed after the constructor was entered.
     *
     * NOTE: The destructor has actually been removed from the class. See my notes in the header
     * file debug.hpp for more information. I'm keeping this code here for reference or in case
     * I want to go back to it some day.
     */
    Tracer::~Tracer()
    {
        // Check to see if we have to do anything.
        if( !initialized ) return;
        if( show_trace == false ) return;

        CursorMark object;

        // Yes. Let the user interact with the system.
        // Open command window.
        command_window->show( );

        // Output the information on row 1.
        command_window->print_at( 1, 0, "%d: EXIT %s", breakpoint_level, breakpoint_name );
        
        // Get and handle user input.
        command_loop( );

        // Return to main program.
        command_window->clear( 1 );
        command_window->hide( );
    }

#endif


    //! Construct a debug window.
    /*!
     * DebugWindows are just like TextWindows except that their constructor always opens them in
     * the center of the screen. They also allow their clients to specify a header that will be
     * printed into the upper right corner of their border. Finally, they always wait for the
     * user to strike a key before being destroyed.
     */
    DebugWindow::DebugWindow(const char *header, int full_width, int full_height, int color)
    {
        int  top_row;
        int  left_column;
        char header_buffer[GENERIC_BUF_SIZE+1];
   
        window_center_coordinates( full_width, full_height, &top_row, &left_column );
        open( top_row, left_column, full_width, full_height, color, DOUBLE_LINE );

        sprintf( header_buffer, " %s ", header );
        print_text(
            row( ) - 1,
            column( ) + width( ) - 1 - static_cast<int>( strlen( header_buffer ) ),
            strlen( header_buffer ),
            header_buffer
        );
    }


    //! Destroy a debug window.
    DebugWindow::~DebugWindow( )
    {
        if( !is_hidden ) key( );
    }

}

#endif
