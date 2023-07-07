/*! \file    command_i.cpp
 *  \brief   Implementation of the 'i' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "command.hpp"
#include "FileList.hpp"
#include "parameter_stack.hpp"
#include "YEditFile.hpp"

bool input_command( )
{
    const char *input_string;

    static Parameter parameter( "PROMPT:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    Parameter new_string( parameter_value.c_str( ) );
    if( new_string.get( false ) == false ) input_string = "";
    else {
        std::string new_value = new_string.value( );
        input_string = new_value.c_str( );
    }

    parameter_stack.push( input_string );
    return true;
}


bool insert_command( )
{
    FileList::active_file( ).toggle_insert( );
    return true;
}


bool insert_file_command( )
{
    static Parameter parameter( "FILE TO INSERT:" );
    if( parameter.get( ) == false ) return false;
    std::string parameter_value = parameter.value( );

    YEditFile &the_file = FileList::active_file( );

    // It would be better to check for file existance before deleting the block.
    if( the_file.get_block_state( ) ) {
        the_file.delete_block( );
        the_file.toggle_block( );
    }

    bool return_value = the_file.load( parameter_value.c_str( ) );
    if( return_value == true ) the_file.mark_as_changed( );
    return return_value;
}
