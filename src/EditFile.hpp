/*! \file    EditFile.hpp
 *  \brief   Interface to class EditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef EDITFILE_HPP
#define EDITFILE_HPP

#include "EditList.hpp"
#include "FilePosition.hpp"

/*===================================================*/
/*           Definition of class EditFile           */
/*===================================================*/

//! Base class of the EditFile hierarchy.
/*!
 * This class contains the text of a file being edited. It is intended to be used as a base of
 * class derivations. Each derived class adds functionality to this class in a well defined
 * manner. Several derived classes can be pasted together with multiple inheritance to give the
 * final, fully functional class.
 *
 * In addition, this class knows enough about blocks to allow derived classes access to the
 * block information they need. Ideally, these block handling functions should be virtual with
 * implementations in Block_EditFile. However, Borland's Turbo C++ version 1.0 manifests
 * compiler bugs when virtual functions are used in a virtual base class.
 *
 * \todo Make the block manipulation functions in EditFile virtual.
 */
class EditFile {

    friend class Block_EditFile;

private:
    bool constructed_ok;         //!< True if this object is ok.

protected:
    // Constructors and destructors.
    EditFile( );
    virtual ~EditFile( );

    EditList     file_data;      //!< The actual data in the file.
    FilePosition current_point;  //!< This file's current point.
    bool         block;          //!< True when block mode is ON.
    long         anchor;         //!< Line number of one side of the block.
    bool         is_changed;     //!< True if data "changed."

    void erase( );
    bool extend_to_line( long );
    bool built_ok( );

public:
    // NOTE **** The following functions should really be virtual ****

    void block_limits( long &top_line, long &bottom_line );
    void set_block_state( bool );
    bool get_block_state( );
    bool top_of_block( );
  };

#endif

