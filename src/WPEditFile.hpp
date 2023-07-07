/*! \file    WPEditFile.hpp
 *  \brief   Interface to class WPEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef WPEDITFILE_HPP
#define WPEDITFILE_HPP

#include "EditFile.hpp"

//! Adds simple word processing featurs to an EditFile.
class WPEditFile : private virtual EditFile {
public:
    //! Reformats paragraph containing the current point.
    bool reformat_paragraph( );
};

#endif
