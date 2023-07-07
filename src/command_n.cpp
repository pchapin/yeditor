/*! \file    command_n.cpp
 *  \brief   Implementation of the 'n' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstddef>

#include "command.hpp"
#include "FileList.hpp"
#include "YEditFile.hpp"

bool new_line_command( )
{
    YEditFile &the_file      = FileList::active_file( );
    bool        return_value = true;

    // Do the actual work.
    if( the_file.insert_mode( ) == YEditFile::INSERT )
        return_value = the_file.new_line();

    // Learn how far the current line is indented.
    const EditBuffer *current_line = the_file.get_line( );
    std::size_t line_offset = 0;
    while( (*current_line)[line_offset] == ' ' ) line_offset++;

    // Move cursor to start of next line.
    the_file.CP( ).cursor_down( );
    the_file.CP( ).jump_to_column( 0 );

    // Insert spaces to indent as long as things are working.
    std::ptrdiff_t count = line_offset;
    while( count-- && return_value == true ) {

        if( the_file.insert_mode( ) == YEditFile::INSERT )
            return_value = the_file.insert_char( ' ' );

        if( return_value == true ) the_file.CP( ).cursor_right( );
    }

    // If this line is start of new block. Indent again.
    if( the_file.extra_indent( )  &&  return_value == true ) {
        return_value = tab_command( );
    }

    return return_value;
}


bool next_file_command( )
{
    FileList::next( );
    return true;
}


bool next_procedure_command( )
{
    return FileList::active_file( ).next_procedure( );
}
