/*! \file    DiskEditFile.hpp
 *  \brief   Interface to class DiskEditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef DISKEDITFILE_HPP
#define DISKEDITFILE_HPP

#include <cstdio>
#include <ctime>

#include "environ.hpp"
#include "EditFile.hpp"

//! Adds disk I/O features to EditFile.
/*!
 * DiskEditFile adds basic disk I/O operations to EditFile objects. Notice that this class does
 * not know the name of the file. It does disk reads and writes for any file requested and make
 * no assumptions about file names.
 */
class DiskEditFile : private virtual EditFile {

private:
  #if eOPSYS == ePOSIX
    time_t file_time;
  #else
    unsigned  file_date;          //!< Date stamp of file.
    unsigned  file_time;          //!< Time stamp of file.
  #endif

protected:
    bool read_disk( std::FILE * );
    bool write_disk( std::FILE * );
    bool write_disk_block( std::FILE * );

public:
  #if eOPSYS == ePOSIX
    time_t   time( )  { return file_time; }
  #else
    unsigned date( )  { return file_date;  }
    unsigned time( )  { return file_time;  }
  #endif

    bool changed( )  { return is_changed; }
    void set_timestamp( const char *name );
    void mark_as_changed( );
    void mark_as_unchanged( );

    enum Mode { ALL, BLOCK_ONLY };
    bool load( const char *the_name );
    bool save( const char *the_name, Mode save_mode = ALL );
};

#endif
