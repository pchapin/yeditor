/*! \file    windebug.hpp
    \brief   Interface to a Windows run-time debugging module.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This component provides an application with a simple embedded debugger. The debugger supports
traces and breakpoints with a simple logging feature. Many additional enhancements are possible.
Although I certainly make use of traditional debuggers during application development, there are
times when having a debugging facility built into the application is useful. In this way end
users can be walked through debugging operations that can't be reproduced on the development
station.
*/

#ifndef WINDEBUG_H
#define WINDEBUG_H

#include <sstream>

namespace spica {
    namespace Win32 {

        // Displays the debugging window to the user. It is safe to call this function even if
        // the window is already displayed. You can "display" debugging text even if the
        // debugging window is not currently being displayed. Such text is stored and displayed
        // later.
        //
        void create_debugWindow();
        
        // This class is useful for tracing program execution. Construction sends a message to
        // the debug window. Destruction does not send a message. We can't fully control when an
        // object gets destroyed, so such messages would be potentially very misleading. For the
        // moment, the private members are not really being used. In the future I may implement
        // some additional member functions that could call upon that private data. I need more
        // experience with Tracer objects first.
        //
        class Tracer {
        private:
            int   level;          // Trace level. Deep traces might be disabled.
            const char *message;  // The programmer defined message.
            const char *file;     // The name of the file where the Tracer object is located.
            int   line;           // The line number where the Tracer object is located.

        public:
            Tracer(
                   int   trace_level,
                   const char *trace_message,
                   const char *file_name,
                   int   line_number
                   );
        };


        // This macro makes it easier (possible!) to use Tracer objects. You provide the
        // message, and let the preprocessor fill in the file name and line number as
        // appropriate. Default arguments can't be used in the constructor otherwise every
        // Tracer will have a file and line location that points to this header file!
        //
#define Tracer(level, message)                                  \
        spica::Win32::Tracer(level, message, __FILE__, __LINE__);


        //
        // class debugstream
        //
        // This class facilitates the construction of debugging messages. You can treat it like
        // an ostringstream except that when you want to write the message into the debug
        // window, just use the member function say().
        //
        class debugstream : public std::ostringstream {
        public:
            void say(int level = 1);
        };


        //
        // class notifystream
        //
        // This class facilitates the construction of exception notification messages. You can
        // treat it like an ostringstream except that when you want to write the message into a
        // message box, just use the member function say(). Function say() will also write the
        // exception notification message into the debugging window for future reference. All
        // exception notifications are at debugging level zero.
        //
        class notifystream : public std::ostringstream {
        public:
            // window_handle is the handle of the window that will own the message box.
            void say(HWND window_handle = 0);
        };


    } // End of namespace scopes.
}

#endif
