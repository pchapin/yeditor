/*! \file    LineEditFile.hpp
 *  \brief   Interface to clss LineEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef LINEEDITFILE_HPP
#define LINEEDITFILE_HPP

#include "EditFile.hpp"

class LineEditFile : private virtual EditFile {

  public:

    /*!
     * Returns a pointer to the text of the current line. This function will cause the file to
     * extend if the current line is off the end of the data. It returns NULL if the extension
     * fails.
     */
    const EditBuffer *get_line( );

    //! Returns length of current line.
    unsigned CP_line_length( );

    //! Inserts parameter before current line. Returns false if out of memory.
    bool insert_line( const EditBuffer *new_line );

    //! Replaces current line with parameter. Returns false if out of memory.
    bool replace_line( const EditBuffer *new_line );

    //! Deletes the current line.
    void delete_line( );

    //! Deletes to the end of the line.
    void delete_to_EOL( );
  };

#endif
