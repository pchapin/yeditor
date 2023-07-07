/*! \file    YEditFile.hpp
 *  \brief   Interface to class YEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef YEDITFILE_HPP
#define YEDITFILE_HPP

#include <string>

#include "BlockEditFile.hpp"
#include "CharacterEditFile.hpp"
#include "CursorEditFile.hpp"
#include "DiskEditFile.hpp"
#include "EditBuffer.hpp"
#include "EditFile.hpp"
#include "LineEditFile.hpp"
#include "SearchEditFile.hpp"
#include "WPEditFile.hpp"

class FileDescriptor;

class YEditFile :
  public virtual EditFile,           // Needed to ctor and dtor virtual base.
  public         BlockEditFile,      // YEditFiles can do blocks.
  public         CharacterEditFile,  //    ... character insertions, etc.
  public         CursorEditFile,     //    ... cursor movement.
  public         DiskEditFile,       //    ... disk I/O.
  public         LineEditFile,       //    ... line operations.
  public         SearchEditFile,     //    ... search and replace.
  public         WPEditFile          //    ... simple word processing.
{
private:
    std::string  file_name;          // Name of file.
    int          color;              // Color attribute for text.

public:
    //! Constructor.
    YEditFile( const char *name_of_file, int tab_distance, int file_color );

    //! Virtual destructor so specialized YEditFiles can clean up.
    virtual ~YEditFile( );

    // Access.
    const char   *name( )            { return file_name.c_str( ); }
          int     color_attribute( ) { return color; }

    //! Adjusting color attribute. This function must update Screen also.
    void set_color( int new_column );

    /*!
     * Scan the file descriptor list and adjust all internal attributes as necessary. If the
     * file is not found in the descriptor list, this function does nothing.
     */
    void set_attributes( );

    //! Load up the given descriptor with information from 'this' file.
    void set_descriptor( FileDescriptor & );

    // These have to work differently for each type of file.
    virtual bool next_procedure( );
    virtual bool previous_procedure( );
    virtual bool extra_indent( );
    virtual bool insert_char( char );

    //! Redraws entire display.
    void display( );
};

#endif
