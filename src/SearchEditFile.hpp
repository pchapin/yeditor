/*! \file    SearchEditFile.hpp
 *  \brief   Interface to class SearchEditFile.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef SEARCHEDITFILE_HPP
#define SEARCHEDITFILE_HPP

#include "EditFile.hpp"

//! Adds simple search abilities to class EditFile.
class SearchEditFile : private virtual EditFile {
public:
    //! Adjusts current point to start of string if found.
    bool simple_search( const char *search_string );
};

#endif
