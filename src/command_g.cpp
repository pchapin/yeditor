/*! \file    command_g.cpp
 *  \brief   Implementation of the 'g' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "FileList.hpp"
#include "parameter_stack.hpp"
#include "YEditFile.hpp"

bool goto_column_command( )
{
    unsigned  column_value;

    static Parameter parameter( "COLUMN NUMBER:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    // Decode text entered by user (User see first column as line 1).
    column_value = static_cast< unsigned >( std::atol( parameter_value.c_str( ) ) - 1U );

    // Perform the action.
    YEditFile &the_file = FileList::active_file( );
    the_file.CP( ).jump_to_column( column_value );

    return true;
}


bool goto_file_end_command( )
{
    FileList::active_file( ).bottom_of_file( );
    return true;
}


bool goto_file_start_command( )
{
    FileList::active_file( ).top_of_file( );
    FileList::active_file( ).CP( ).jump_to_column( 0 );
    return true;
}


bool goto_line_command( )
{
    long line_value;

    static Parameter parameter("LINE NUMBER:");
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    // Decode text entered by user (User sees first line as line 1).
    line_value = std::atol( parameter_value.c_str( ) ) - 1L;

    // Perform the action.
    FileList::active_file( ).CP( ).jump_to_line( line_value );
    return true;
  }


bool goto_line_end_command( )
{
    FileList::active_file( ).end( );
    return true;
}


bool goto_line_start_command( )
{
    FileList::active_file( ).home( );
    return true;
}
