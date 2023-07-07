/*! \file    BlockEditFile.hpp
 *  \brief   Interface to class BlockEditFile
 *  \author  Peter C. Chapin <spicacalitkelseymountain.org>
 */

#ifndef BLOCKEDITFILE_HPP
#define BLOCKEDITFILE_HPP

#include "EditFile.hpp"

//! Adds block handling to EditFile.
/*!
 * BlockEditFile adds simple block handling functions to basic EditFile objects. Although
 * EditFile objects know of blocks, they provide no means to manipulate those blocks. This class
 * provides the means.
 *
 * Ideally this class would contain the implementation of the virtual block functions in class
 * EditFile. Those block handling functions are not virtual because Borland's Turbo C++ 1.0 had
 * problems with virtual functions in virtual base classes.
 *
 * This class has no private data. Class EditFile contains the data needed for blocks in its
 * protected section. An ideal implementation would have that private data here with a
 * constructor here to initialize it. Right now, EditFile's constructor is initializing block
 * information members.
 */
class BlockEditFile : private virtual EditFile {
public:

    struct BlockInfo {
        friend class BlockEditFile;
    private:
        bool          is_on;    //!< true if block mode was on.
        long          anchor;   //!< The anchor line number of block mode.
        FilePosition  limit;    //!< The current point of the other end of the block.
    };

    void get_blockinfo( BlockInfo & );
    void set_blockinfo( const BlockInfo & );
    void toggle_block( );
    bool get_block( EditList & );
    void delete_block( );
    bool insert_block( EditList & );
};

#endif

