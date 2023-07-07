/*! \file    yfile.hpp
 *  \brief   Interface to filelist.yfy handling.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef YFILE_HPP
#define YFILE_HPP

#include "EditBuffer.hpp"
#include "mylist.hpp"
#include "YEditFile.hpp"

class YEditFile;

void read_yfile( );

//! Describes information about a file that is saved between invocations of Y.
class FileDescriptor {

    /*!
     * This function reads filelist.yfy and parses it into a list of File_Descriptor objects.
     * Files that are to be loaded into the editor are loaded later and their descriptors are
     * removed from this list.
     */
    friend void read_yfile( );

    //! This function writes information from a descriptor into filelist.yfy.
    friend void write_descriptor( std::FILE *, const FileDescriptor & );

    //! Used when a file is loaded to set all the attributes from a file descriptor in this list.
    friend void YEditFile::set_attributes( );

    /*!
     * Used when the yfile is saved so that a YEditFile object has a way of reporting on its
     * current status.
     */
    friend void YEditFile::set_descriptor( FileDescriptor & );

private:
    bool        active_flag;      //!< =true if this is the active file.
    bool        block_flag;       //!< =true if block mode is on.
    long        block_line;       //!< Line number of start of block.
    int         color_attribute;  //!< Color used to display this file.
    unsigned    cursor_column;    //!< Column number of cursor position.
    long        cursor_line;      //!< Line number of cursor position.
    bool        deleted_flag;     //!< =true if this file not loaded in editor.
    bool        insert_flag;      //!< =true if file in insert mode.
    EditBuffer  name;             //!< Pathname of file.
    int         tab_setting;      //!< Tab stop distance.
    unsigned    window_column;    //!< Column number of left side of window.
    long        window_line;      //!< Line number of top of window.
    
public:
    FileDescriptor( const EditBuffer &the_name );

    bool is_deleted( )             { return deleted_flag; }
    bool is_active( )              { return active_flag;  }
    const EditBuffer &get_name( )  { return name;         }

    void make_deleted( )  { deleted_flag = true; }
    void make_inactive( ) { active_flag = false; }

    // Forces the members of a FileDescriptor into something sensible.
    void sanity_check();
};

//! This list contains file descriptors as placed in filelist.yfy.
extern List<FileDescriptor> descriptor_list;

/*!
 * This function writes the entire filelist.yfy file using the current descriptor list of
 * deleted file entries and the current list of files loaded into the editor.
 */
void write_yfile( );

#endif
