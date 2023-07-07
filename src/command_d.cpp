/*! \file    command_d.cpp
 *  \brief   Implementation of the 'd' command functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>

#include "clipboard.hpp"
#include "command.hpp"
#include "FileList.hpp"
#include "parameter_stack.hpp"
#include "WordSource.hpp"
#include "yfile.hpp"

bool define_key_command( )
{
    static Parameter text( "MACRO TEXT:" );
    static Parameter key ( "KEY NAME:"   );

    if( text.get( ) == false ) return false;
    if( key.get( )  == false ) return false;

    std::string text_value = text.value( );
    std::string key_value  = key.value( );

    modify_key_association( key_value.c_str( ), text_value.c_str( ) );
    return true;
}


bool delete_command( )
{
    return FileList::active_file( ).delete_char( );
}


bool delete_EOL_command( )
{
    YEditFile &the_file = FileList::active_file( );
    the_file.delete_to_EOL( );
    return true;
}


bool delete_SOL_command( )
{
    YEditFile &the_file = FileList::active_file( );
    unsigned   count    = the_file.CP( ).cursor_column( );

    for( unsigned i = 0; i < count; i++ ) backspace_command( );
    return true;
}


bool delete_block_command( )
{
    bool return_value;
    YEditFile &the_file = FileList::active_file( );

    clipboard.clear( );
    return_value = the_file.get_block( clipboard );
    if( return_value == true ) the_file.delete_block( );

    // Turn off block mode if it's on.
    if( the_file.get_block_state( ) ) the_file.toggle_block( );

    return return_value;
}
