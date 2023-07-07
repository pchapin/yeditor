/*! \file    CharacterEditFile.hpp
 *  \brief   Interface to class CharacterEditFile
 *  \author  Peter C. Chapin <spicacalitkelseymountain.org>
 */

#ifndef CHARACTEREDITFILE_HPP
#define CHARACTEREDITFILE_HPP

#include "EditFile.hpp"

//! Adds character handling to EditFile.
/*!
 * CharacterEditFile adds basic character manipulations to EditFile objects. It is this class
 * that is responsible for extending short lines as necessary.
 */
class CharacterEditFile : private virtual EditFile {

public:
    //! The possible editing states.
    enum InsertMode { INSERT, REPLACE };

private:
    int        tab_stop;      //!< Distance between tab stops for this file.
    InsertMode insert_state;  //!< Current mode for this file.

public:
    CharacterEditFile( int tab_distance ) :
        tab_stop( tab_distance ), insert_state( INSERT )
        { }

    int        tab_distance( ) { return tab_stop;     }
    InsertMode insert_mode( )  { return insert_state; }

    //! Change the tab distance.
    void set_tab( int new_tab ) { tab_stop = new_tab; }

    void toggle_insert( );
    void set_insert( InsertMode new_mode );
    bool new_line( );
    bool insert_char( char letter );
    bool replace_char( char letter );
    bool backspace( );
    bool delete_char( );
};

#endif
