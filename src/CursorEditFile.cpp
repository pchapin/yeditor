/*! \file    CursorEditFile.cpp
 *  \brief   Implementation of class CursorEditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstring>

#include "EditBuffer.hpp"
#include "CursorEditFile.hpp"


void CursorEditFile::home( )
{
    current_point.jump_to_column( 0 );
}


void CursorEditFile::end( )
{
    file_data.jump_to( current_point.cursor_line( ) );
    EditBuffer *current_line = file_data.get( );

    if( current_line == NULL ) current_point.jump_to_column( 0 );
    else current_point.jump_to_column( current_line->length( ) );
}


void CursorEditFile::top_of_file( )
{
    current_point.jump_to_line( 0L );
    home( );
}


void CursorEditFile::bottom_of_file( )
{
    current_point.jump_to_line( file_data.size( ) );
    end( );
}
