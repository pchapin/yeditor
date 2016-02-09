/*! \file    debug.hpp
    \brief   Interface to embedded debugging system.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef DEBUG_HPP
#define DEBUG_HPP

#define DEBUG 1

// Activate debugging stuff if and only if DEBUG is defined.
#ifdef DEBUG

#include "scr.hpp"
#include "TextWindow.hpp"

#define DBG_TOP    1
#define DBG_BOTTOM 2

namespace scr {

    typedef void ( *snapshot_function_t )( );

    void register_snapshot( const char *name, snapshot_function_t picture );
    void initialize_debugging( int window_placement = DBG_TOP );
    void terminate_debugging( );

    //! Objects of this type cause breakpoints on construction.
    /*!
     * In a previous version of this package, I also had them cause breakpoints on destruction.
     * However one compiler (Borland) was destroying these objects at unexpected times and that
     * caused for confusing output from the debugging system. In the current version of the
     * software, there is no reason for the data members. Those members existed so the
     * constructor could pass information to the destructor. I'm keeping the members there for
     * now in case I want to go back to using the destructor or in case I think of something
     * else good to do with them.
     */
    class Tracer {
        const char *breakpoint_name;  //!< Name for this Tracer object.
        int   breakpoint_level;       //!< Associated detail level for this object.
        bool  show_trace;             //!< True if destructor is to activate a breakpoint.

    public:
        Tracer( const char *trace_name, int trace_level, const char *format = NULL, ... );

        // ~Tracer( );
    };

    //! Objects of this type are for snapshot functions to use to display stuff.
    class DebugWindow : public TextWindow {
    public:
        DebugWindow( const char *header, int width = 74, int height = 5, int color = REV_WHITE );
       ~DebugWindow( );
    };
}

#endif

namespace scr {

    //! This is true or false depending on the state of DEBUG at compile time.
    extern bool debugging_available;
}

#endif
