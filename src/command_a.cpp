/*! \file    command_a.cpp
 *  \brief   Implementation of the 'a' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "command.hpp"
#include "FileList.hpp"
#include "parameter_stack.hpp"
#include "yfile.hpp"

bool add_text_command( )
{
    static Parameter parameter( "TEXT TO ADD:" );

    YEditFile &the_file = FileList::active_file( );
    bool        return_value = true;

    if( parameter.get( ) == false ) return false;

    std::string parameter_value = parameter.value( );
    const char *parameter_text = parameter_value.c_str( );

    while( *parameter_text && return_value == true ) {
        if( the_file.insert_mode( ) == YEditFile::INSERT )
            return_value = the_file.insert_char( *parameter_text );
        else
            return_value = the_file.replace_char( *parameter_text );

        parameter_text++;
        if( return_value == true ) the_file.CP( ).cursor_right( );
    }

    return return_value;
}
