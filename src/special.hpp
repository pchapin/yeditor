/*! \file    special.hpp
 *  \brief   Interface to specialized YEditFile classes.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef SPECIAL_HPP
#define SPECIAL_HPP

#include "scr.hpp"
#include "YEditFile.hpp"

#define FILE_CLASS(language, color, tab_stop)              \
    class language##_YEditFile : public YEditFile {        \
    public:                                                \
        language##_YEditFile( const char *file_name ) :    \
            YEditFile( file_name, tab_stop, color ) { }    \
    };

// Right now, essentially all the types are the same except for tab stop.
FILE_CLASS( DOC,   scr::WHITE, 5 )
FILE_CLASS( OTHER, scr::WHITE, 8 )

class ADA_YEditFile : public YEditFile {
public:
    ADA_YEditFile( const char *file_name ) :
        YEditFile( file_name, 3, scr::WHITE ) { }

    virtual bool next_procedure( );
    virtual bool previous_procedure( );
};

class ASM_YEditFile : public YEditFile {
public:
    ASM_YEditFile( const char *file_name ) :
        YEditFile( file_name, 8, scr::WHITE ) { }

    virtual bool next_procedure( );
    virtual bool previous_procedure( );
};

class C_YEditFile : public YEditFile {
private:
    bool marks_valid;     //!< True if the data below is meaningful.
    long function_head;   //!< Line number of most recent function head.
    long function_brace;  //!< Line number of most recent function brace.

public:
    C_YEditFile( const char *file_name ) :
        YEditFile( file_name, 4, scr::WHITE ), marks_valid( false ) { }

    virtual bool next_procedure( );
    virtual bool previous_procedure( );
    virtual bool extra_indent( );
    virtual bool insert_char( char );
};

class PCD_YEditFile : public YEditFile {
public:
    PCD_YEditFile( const char *file_name ) :
        YEditFile( file_name, 4, scr::WHITE ) { }

    virtual bool next_procedure( );
    virtual bool previous_procedure( );
};

//! For now the SCALA_YEditFile is a copy of C_YEditFile. This won't be true forever, however.
class SCALA_YEditFile : public YEditFile {
private:
    bool marks_valid;     //!< True if the data below is meaningful.
    long function_head;   //!< Line number of most recent function head.
    long function_brace;  //!< Line number of most recent function brace.

public:
    SCALA_YEditFile( const char *file_name ) :
        YEditFile( file_name, 2, scr::WHITE ), marks_valid( false ) { }

    virtual bool next_procedure( );
    virtual bool previous_procedure( );
    virtual bool extra_indent( );
    virtual bool insert_char( char );
};

#endif
