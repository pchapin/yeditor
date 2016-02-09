/*! \file    actions.cpp
    \brief   This file contains...
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <climits>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

#include "BinaryEntity.hpp"
#include "ComplexEntity.hpp"
#include "Entity.hpp"
#include "FloatEntity.hpp"
#include "IntegerEntity.hpp"
#include "ListEntity.hpp"
#include "MatrixEntity.hpp"
#include "RationalEntity.hpp"
#include "StringEntity.hpp"
#include "VeryLong.hpp"

#include "actions.hpp"
#include "global.hpp"
#include "support.hpp"
#include "WordStream.hpp"

static VeryLong pop_int( Stack &the_stack )
{
    VeryLong return_value;

    Entity *temp = the_stack.pop( );
    if( temp == NULL )
        error_message( "Too few arguments" );
    else {
        IntegerEntity *integer_temp = static_cast<IntegerEntity *>( temp->to_integer( ) );
        if( integer_temp == NULL ) {
            error_message( "Integer argument expected" );
            the_stack.push( temp );
        }
        else {
            return_value = integer_temp->get_value( );
            delete integer_temp;
        }
    }
    return return_value;
}

void do_bin( Stack & )
{
    global::set_base( global::BINARY );
}

void do_clear( Stack &the_stack )
{
    the_stack.clear( );
}

void do_dec( Stack & )
{
    global::set_base( global::DECIMAL );
}

void do_deg( Stack & )
{
    global::set_angle_mode( global::DEG );
}

void do_drop( Stack &the_stack )
{
    the_stack.drop( );
}

void do_dropn( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );

    if( count == 0 ) return;
    for( VeryLong i = 0; i < count; ++i )
        the_stack.drop( );
}

void do_dup( Stack &the_stack )
{
    Entity *new_copy = the_stack.get( 0 );
    if( new_copy != NULL ) {
        new_copy = new_copy->duplicate( );

        if( new_copy != NULL )
            the_stack.push( new_copy );
    }
}

void do_dupn( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );

    if( count == 0 ) return;
    for( VeryLong i = 0; i < count; ++i ) {
        Entity *new_copy = the_stack.get( count - 1 );
        if( new_copy != NULL ) {
            new_copy = new_copy->duplicate( );

            if( new_copy != NULL )
                the_stack.push( new_copy );
        }
    }
}

void do_eng( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );

    if( count == 0 ) return;
    global::set_decimal_count( count );
    global::set_display_mode( global::ENGINEERING );
}

void do_eval( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *file_name;
    if( ( file_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
      error_message( "String expected" );
      the_stack.push( temp );
      return;
    }

    FileStream *new_words = new FileStream( file_name->get_value( ).c_str( ) );
    if( new_words != NULL ) {
      global::word_source( ).push( new_words );
    }
  }

void do_fix( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );
    global::set_decimal_count( count );
    global::set_display_mode( global::FIXED );
}

void do_grad( Stack & )
{
    global::set_angle_mode( global::GRAD );
}

void do_hex( Stack & )
{
    global::set_base( global::HEX );
}

void do_oct( Stack & )
{
    global::set_base( global::OCTAL );
}

void do_polar( Stack & )
{
    global::set_complex_mode( global::POLAR );
}

void do_purge( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *variable_name;
    if( ( variable_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
        error_message( "String expected" );
        the_stack.push( temp );
        return;
    }
    // Gets the root Directory_Entity.
    // global::get_root( ).delete( variable_name->get_value( ).c_str( ) );
    delete variable_name;
}

void do_rad( Stack & )
{
    global::set_angle_mode( global::RAD );
}

void do_rec( Stack & )
{
    global::set_complex_mode( global::RECTANGULAR );
}

static unsigned long read_header( ifstream &in_file )
{
    // TODO: Fix this to use IOStreams properly.
#ifdef NEVER
    unsigned long lulv;  // lulv = local unsigned long variable

    if( fread( &lulv, sizeof( lulv ), 1, in_file ) != 1 )
        return -1;
    else
        return lulv;
#endif
    return 0;
}


static bool read_signature( ifstream &in_file )
{
    // TODO: Fix this to use IOStreams properly.
#ifdef NEVER
    char array[10+1];
    if( fscanf( in_file, "%10c", array ) == EOF ) return false;
    array[10] = '\0';
    if( strcmp( array, "CLAC FILE\x1A" ) != 0 ) return false;
    return true;
#endif
    return false;
}

void do_read( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );
    ifstream in_file;

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *file_name;
    if( ( file_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
        error_message( "String expected" );
        the_stack.push( temp );
        return;
    }
    in_file.open( file_name->get_value( ).c_str( ) );
    if( !in_file ) {
        error_message( "Cannot open %s for reading", file_name->get_value( ).c_str( ) );
        delete file_name;
        return;
    }
    if( ( read_signature( in_file ) == false ) || ( read_header( in_file ) == -1 ) ) {
        error_message( "Bad file format" );
        delete file_name;
        return;
    }
    char type = -1;
    in_file.read( &type, 1 );

    Entity *new_object = NULL;
    switch (type) {
    case BINARY:
        new_object = new BinaryEntity;
        break;
    case COMPLEX:
        new_object = new ComplexEntity( std::complex<double>( 0.0 ) );
        break;
    case FLOAT:
        new_object = new FloatEntity( 0.0 );
        break;
    case INTEGER:
        new_object = new IntegerEntity( 0UL );
        break;
    case LIST:
        new_object = new ListEntity;
        break;
    case MATRIX:
        new_object = new MatrixEntity;
        break;
    case RATIONAL:
        new_object = new RationalEntity( Rational<VeryLong>( ) );
        break;
    case STRING:
        new_object = new StringEntity;
        break;
    default:
        error_message( "Unknown object type in file" );
        break;
    }
    if( new_object == NULL ) {
        delete file_name;
        in_file.close( );
        return;
    }
    // TODO: This is completely broken. The semantics of read must have changed at some point.
    // if( new_object->read( in_file ) == false ) {
    //     error_message( "Unable to read object" );
    //     delete new_object;
    // }
    delete file_name;
    in_file.close( );
    the_stack.push( new_object );
    return;
}

void do_roll_up( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );
    the_stack.roll_up( count );
}

void do_roll_down( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );
    the_stack.roll_down( count );
}

void do_rot( Stack &the_stack )
{
    the_stack.rotate( );
}

void do_run( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *file_name;
    if( ( file_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
        error_message( "String expected" );
        the_stack.push( temp );
        return;
    }

#ifdef NEVER
    FWordStream *new_words = new FWordStream( (char *)*file_name );
    if( new_words != NULL ) {
        global::word_source( ).push( new_words );
    }
#endif
}

void do_sci( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );
    global::set_decimal_count( count );
    global::set_display_mode( global::SCIENTIFIC );
}

void do_store( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *variable_name;
    if( ( variable_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
        error_message( "String expected" );
        the_stack.push( temp );
        return;
    }
    Entity *object = the_stack.pop( );
    if( object == NULL ) {
        error_message( "Too few arguments" );
        delete variable_name;
        return;
    }
#ifdef NEVER
    if( !global::get_root( ).insert( *variable_name, object ) ) {
        error_message( "Unable to save object in current directory" );
        the_stack.push( object );
    }
#endif
    delete variable_name;
    return;
}

void do_stws( Stack &the_stack )
{
    VeryLong count = pop_int( the_stack );

    if( count < 1 ) {
        error_message( "Word size must be at least one bit" );
    }
    else if( count > 32 ) {
        error_message( "Word size must be no more than 32 bits" );
    }
    else {
        global::set_bit_count( count );
    }
}

void do_swap( Stack &the_stack )
{
    the_stack.swap( );
}

#ifdef NEVER
// Probably this function should be in the UI component.
void do_sys( Stack &the_stack )
{
    Entity *command = the_stack.get( 0 );
    if( command == NULL ) {
        underflow( );
    }
    else if( command->my_type( ) != STRING ) {
        error_message( "System command requires string argument" );
    }
    else {
        StringEntity *command_string = (StringEntity *)command;

        // Make sure everything is hidden.
//         global::stack_view( ).hide( );
//         global::flag_view( ).hide( );
//         global::dir_view( ).hide( );
        ScrClear( 1, 1, 80, 25, SCR_WHITE );
        ScrSetCursorPos( 1, 1 );

        // Do the command.
        system( command_string->get_value( ).c_str( ) );
        cout << '\n' << '\n' << "***** Strike any key to return to CLAC *****";
        ScrKey( );

        // Restore screen.
//         global::stack_view ().show( );
//         global::flag_view( ).show( );
//         global::dir_view( ).show( );

        // Get rid of the dos command.
        do_drop( the_stack );
    }
}
#endif

static void write_signature( ofstream &Out_File )
{
    // TODO: Change this to use IOStreams properly.
#ifdef NEVER
    fprintf( out_file, "CLAC FILE\x1A");
#endif
}

static void write_header( ofstream &out_file, Entity *object )
{
    // TODO: Change this to use IOStreams properly.
#ifdef NEVER
    unsigned long length = object->file_size( );
    fwrite( &length, sizeof(length), 1, out_file );
#endif
}

void do_write( Stack &the_stack )
{
    Entity *temp = the_stack.pop( );
    ofstream out_file;

    if( temp == NULL ) {
        error_message( "Too few arguments" );
        return;
    }
    StringEntity *file_name;
    if( ( file_name = dynamic_cast<StringEntity *>( temp ) ) == NULL ) {
        error_message( "String expected" );
        the_stack.push( temp );
        return;
    }
    Entity *object = the_stack.get( 0 );
    if( object == NULL ) {
      error_message( "Too few arguments" );
      delete file_name;
      return;
    }
    out_file.open( file_name->get_value( ).c_str( ) );
    if( !out_file ) {
        error_message( "Cannot open %s for output", file_name->get_value( ).c_str( ) );
        delete file_name;
        return;
    }
    write_signature( out_file );
    write_header( out_file, object );
    object->write( out_file );
    if( !out_file ) {
        error_message( "Error occurred during object write" );
        out_file.close( );
        remove( file_name->get_value( ).c_str( ) );
        delete file_name;
        return;
    }
    out_file.close( );
    delete file_name;
    return;
}

void do_shift_left( Stack & )
{
    return;
}

void do_shift_right( Stack & )
{
    return;
}

void do_ashift_right( Stack & )
{
    return;
}

void do_off( Stack & )
{
    error_message( "Use 'quit' to terminate Clac" );
}
