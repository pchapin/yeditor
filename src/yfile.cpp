/*! \file    yfile.cpp
 *  \brief   Implementation of filelist.yfy handling.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This file contains the implementation of class File_Descriptor. Objects of this class
 * "describe" files in the manner used by filelist.yfy. This module also contains filelist.yfy
 * handling code.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "FileList.hpp"
#include "scr.hpp"
#include "support.hpp"
#include "yfile.hpp"

namespace {

    //! These are the keywords that appear in filelist.yfy.
    const char *key_words[] = {
        "ACTIVE"     , "BLOCK"         , "BLOCK_LINE" ,
        "COLOR"      , "CURSOR_COLUMN" , "CURSOR_LINE",
        "DELETED"    , "INSERT"        , "NAME"       ,
        "TAB_SETTING", "WINDOW_COLUMN" , "WINDOW_LINE",
        0
    };

}

List<FileDescriptor> descriptor_list;

//! Constructor sets defaults for a file loaded into the editor for the first time.
FileDescriptor::FileDescriptor( const EditBuffer &the_name ) : name( the_name )
{
    active_flag     = false;
    block_flag      = false;
    block_line      = 0;
    color_attribute = scr::WHITE|scr::REV_BLACK;
    cursor_column   = 0;
    cursor_line     = 0;
    deleted_flag    = false;
    insert_flag     = true;
    tab_setting     = 2;
    window_column   = 0;
    window_line     = 0;
}


//! Forces the members into something sensible if they were garbage in filelist.yfy
void FileDescriptor::sanity_check( )
{
    if( color_attribute < 0 || color_attribute > 255 )
        color_attribute = scr::WHITE|scr::REV_BLACK;
    if( block_line  <  0 ) block_line  = 0;
    if( cursor_line <  0 ) cursor_line = 0;
    if( tab_setting <= 0 ) tab_setting = 2;
    if( window_line <  0 ) window_line = 0;

    // TODO: We also need to verify that the cursor is in the window.
}


//! Analyzes a line and looks for a keyword.
static int get_keyword( char *line, char **rest )
{
    const char **search;
    char  *p = std::strchr( line, '=' );

    // If there was no '=', this line is invalid. Ignore it.
    if( p == 0 ) return -1;

    // Break the line into two parts and let p point at the end part.
    *p++ = '\0';

    // Scan the list of keywords to see if this is one of them. Notice that this is a case
    // insensitive search.
    //
    for( search = key_words; *search != 0; search++ ) {
        if( my_stricmp( line, *search ) == 0 ) break;
    }

    // If we didn't find it, oh well.
    if( *search == 0 ) return -1;

    // Otherwise return the index into the array of keywords.
    *rest = p;
    return static_cast< int >( search - key_words );
}


//! Reads filelist.yfy and creates a list of FileDescriptor objects.
void read_yfile( )
{
    const int  linebuf_size = 256;
    char       line[linebuf_size + 2];
    // Used for reading lines out of filelist.yfy. Note the static limit.

    FileDescriptor *new_descriptor = nullptr;
    // Points at a descriptor under construction.

    // Attempt to open filelist.yfy. If we can't we're done already.
    std::FILE *yfile;
    if( ( yfile = std::fopen( "filelist.yfy", "r" ) ) == nullptr ) return;

    // See if filelist.yfy is of a compatible version.
    int major = 0;
    int minor = 0;
    // First check if we can even read one line!
    if( std::fgets( line, linebuf_size + 2, yfile ) == nullptr ) {
        std::fclose( yfile );
        return;
    }
    // Now, try to scan the version number out of the file.
    std::sscanf( line, "Y Version %d.%d", &major, &minor );
    if( major != 1 || minor != 90 ) {  // Version 1.90 is temporary!
        std::fclose( yfile );
        return;
    }

    // For each line in filelist.yfy...
    while( std::fgets( line, linebuf_size + 2, yfile ) != nullptr ) {
        char *p;

        // Trim the '\n' if there is one.
        if( ( p = std::strchr( line, '\n' ) ) != 0 ) *p = '\0';

        // If this line is blank or a comment, skip it.
        if( line[0] == ' ' || line[0] == '\t' || line[0] == '\0' ) continue;
        if( line[0] == '#' ) continue;

        // Ok, this is an interesting line. Let's handle it. The cases below correspond to the
        // indices in the key_words array.
        //
        switch( get_keyword( line, &p ) ) {
        case 0:   // ACTIVE
            if( new_descriptor != nullptr ) {
                if( my_stricmp( p, "true" ) == 0 ) new_descriptor->active_flag = true;
                else if( my_stricmp(p, "false" ) == 0 ) new_descriptor->active_flag = false;
            }
            break;

        case 1:   // BLOCK
            if( new_descriptor != nullptr ) {
                if( my_stricmp( p, "true" ) == 0 ) new_descriptor->block_flag = true;
                else if( my_stricmp( p, "false" ) == 0 ) new_descriptor->block_flag = false;
            }
            break;

        case 2:   // BLOCK_LINE
            if( new_descriptor != nullptr ) {
                new_descriptor->block_line = std::atoi( p );
            }
            break;

        case 3:   // COLOR
            if( new_descriptor != nullptr ) {
                new_descriptor->color_attribute = std::atoi( p );
            }
            break;

        case 4:   // CURSOR_COL
            if( new_descriptor != nullptr ) {
                new_descriptor->cursor_column = std::atoi( p );
            }
            break;

        case 5:   // CURSOR_LINE
            if( new_descriptor != nullptr ) {
                new_descriptor->cursor_line = std::atoi( p );
            }
            break;

        case 6:   // DELETED
            if( new_descriptor != nullptr ) {
                if( my_stricmp( p, "true" ) == 0 ) new_descriptor->deleted_flag = true;
                else if( my_stricmp( p, "false" ) == 0 ) new_descriptor->deleted_flag = false;
            }
            break;

        case 7:   // INSERT
            if( new_descriptor != nullptr ) {
                if( my_stricmp( p, "true" ) == 0 ) new_descriptor->insert_flag = true;
                else if( my_stricmp( p, "false" ) == 0 ) new_descriptor->insert_flag = false;
            }
            break;

        case 8:   // NAME
            if( new_descriptor != nullptr ) {
                new_descriptor->sanity_check( );
                descriptor_list.insert( *new_descriptor );
                delete new_descriptor;
            }
            new_descriptor = new FileDescriptor( p );
            break;

        case 9:   // TAB_SETTING
            if( new_descriptor != nullptr ) {
                new_descriptor->tab_setting = std::atoi( p );
            }
            break;

        case 10:  // WINDOW_COLUMN
            if( new_descriptor != nullptr ) {
                new_descriptor->window_column = std::atoi( p );
            }
            break;

        case 11:  // WINDOW_LINE
            if( new_descriptor != nullptr ) {
                new_descriptor->window_line = std::atoi( p );
            }
            break;
        }

    }

    // Make sure the last descriptor in the file is handled.
    if( new_descriptor != nullptr ) {
        new_descriptor->sanity_check( );
        descriptor_list.insert( *new_descriptor );
        delete new_descriptor;
    }

    std::fclose( yfile );

    // Add a sanity check to verify that there is one non-deleted active file.
}


//! Writes a single descriptor into filelist.yfy.
void write_descriptor( std::FILE *output, const FileDescriptor &the_descriptor )
{
    std::fprintf(output, "\n");
    std::fprintf(output, "NAME=%s\n", the_descriptor.name.to_string( ).c_str( ) );
    std::fprintf(output, "ACTIVE=%s\n", (the_descriptor.active_flag == true) ? "true" : "false");
    std::fprintf(output, "BLOCK=%s\n", (the_descriptor.block_flag == true) ? "true" : "false");
    std::fprintf(output, "BLOCK_LINE=%ld\n", the_descriptor.block_line);
    std::fprintf(output, "COLOR=%d\n", the_descriptor.color_attribute);
    std::fprintf(output, "CURSOR_COLUMN=%u\n", the_descriptor.cursor_column);
    std::fprintf(output, "CURSOR_LINE=%ld\n", the_descriptor.cursor_line);
    std::fprintf(output, "DELETED=%s\n", (the_descriptor.deleted_flag == true) ? "true" : "false");
    std::fprintf(output, "INSERT=%s\n", (the_descriptor.insert_flag == true) ? "true" : "false");
    std::fprintf(output, "TAB_SETTING=%d\n", the_descriptor.tab_setting);
    std::fprintf(output, "WINDOW_COLUMN=%u\n", the_descriptor.window_column);
    std::fprintf(output, "WINDOW_LINE=%ld\n", the_descriptor.window_line);
}


//! Writes filelist.yfy from the list of File_Descriptor objects.
void write_yfile( )
{
    List< FileDescriptor >::Iterator stepper( descriptor_list );
    FileDescriptor *next;

    // Try to open filelist.yfy
    std::FILE *yfile;
    if( ( yfile = std::fopen( "filelist.yfy", "w" ) ) == NULL ) {
        warning_message( "Can't open filelist.yfy!" );
        return;
    }

    // Print the preamble into the file.
    std::fprintf( yfile, "Y Version %d.%d\n", 1, 90 );
    std::fprintf( yfile, "#\n# This file was created by Y itself.\n"
                         "# Consult the Y documentation before editing.\n#\n" );

    // First step over the file list and write out those descriptors as well. This code assumes
    // that there is at least one file in the file list.
    //
    YEditFile *start_file = &FileList::active_file( );
    YEditFile *current    = start_file;
    do {
        next = new FileDescriptor( current->name( ) );
        current->set_descriptor( *next );
        if( current != start_file ) next->make_inactive( );
        write_descriptor( yfile, *next );
        delete next;
        FileList::next( );
        current = &FileList::active_file( );
    } while( current != start_file );

    // Now step down the list of file descriptors.
    while( ( next = stepper( ) ) != NULL ) {
        write_descriptor( yfile, *next );
    }

    std::fclose( yfile );
}
