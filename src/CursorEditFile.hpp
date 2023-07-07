/*! \file    CursorEditFile.hpp
 *  \brief   Interface to class CursorEditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef CURSOREDITFILE_HPP
#define CURSOREDITFILE_HPP

#include "EditFile.hpp"

//! Adds cursor movement abilities to EditFile.
class CursorEditFile : private virtual EditFile {

public:
    //! Return the current point to the client.
    /*!
     * The client can manipulate the current point freely to implement special movements.
     */
    FilePosition &CP()
        { return current_point; }

    //! Moves current point to the beginning of the line.
    void home( );

    //! Moves current point to the end of the line.
    void end( );

    //! Moves current point to the top of the file.
    void top_of_file( );

    //! Moves current point to the bottom of the file.
    void bottom_of_file( );

};

#endif
