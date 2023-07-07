/*! \file    FileList.cpp
 *  \brief   Implementation of the FileList abstract object.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "EditBuffer.hpp"
#include "FileList.hpp"
#include "FileNameMatcher.hpp"
#include "mylist.hpp"
#include "special.hpp"
#include "support.hpp"
#include "YEditFile.hpp"
#include "yfile.hpp"

/*==================================*/
/*           Helper class           */
/*==================================*/

//! List of YEditFile objects forming the file list.
/*!
 * This list is implemented as a list of pointers since the actual types of objects on the list
 * are from various classes derived from YEditFile. This class introduces a destructor so that
 * the YEditFile objects are properly deleted when this list is destroyed (and not *just* the
 * pointers to those objects). As a result, the pointers on this list must point at dynamically
 * allocated YEditFile objects.
 */
class YFileList : public List< YEditFile * > {
public:
   ~YFileList( );
};

YFileList::~YFileList( )
{
    YEditFile **File;

    jump_to( 0 );
    while( ( File = next( ) ) != NULL ) delete *File;
}

/*=========================================*/
/*           Private Global Data           */
/*=========================================*/

static EditBuffer   mark_name;  //!< The name of the file with the mark.
static FilePosition mark_point; //!< The position of the mark in the file.
static YFileList    the_list;   //!< This is the file list itself.

// static OTHER_YEditFile scratch_file( "scratch.yfy" );
//
// This file is held and managed outside of the file list. FileList::active_file() returns a
// reference to this file if the list is empty. In this way, attempts to manipulate a file
// before any is loaded into the editor will simple manipulate this scratch file. Note that if
// "scratch.yfy" exists, it will be loaded! However unless the user explicitly saves
// scratch.yfy, it will never be saved.

enum FileType { ADA, ASM, C, DOC, PCD, SCALA, OTHER };

struct InitialAttributes {
    const char *extension;
    FileType type;
};

/*
 * The following array defines the initial attributes of files with the indicated extensions.
 * This array must include an entry for the extension "". Such an entry is used for all files
 * not otherwise defined.
 */
static InitialAttributes default_attributes[] = {

    // Ada
    { ".ADB", ADA },
    { ".ADS", ADA },
    { ".SEP", ADA },

    // Assembly language.
    { ".ASM", ASM }, { ".INC", ASM }, { ".MAC", ASM },

    // C/C++
    { ".C"  , C }, { ".CC" , C },
    { ".H"  , C }, { ".HH" , C },
    { ".CPP", C }, { ".HPP", C },
    { ".CXX", C }, { ".HXX", C },

    // D (for now just use the C support)
    { ".D",   C },

    // Documentation.
    { ".MD", DOC }, { ".TEX", DOC }, { ".TXT", DOC },

    // Pseudocode.
    { ".PCD", PCD },

    // Scala
    { ".SCALA", SCALA },

    // Anything else. This entry has to be at the end of the list.
    { ""    , OTHER }
};

/*======================================*/
/*           Public Functions           */
/*======================================*/

namespace FileList {

    bool new_file( const char *name )
    {
        char raw_extension[256];
        // Allow for extensions that are longer than three characters.

        // Find the extension and copy it into raw_extension[]. The check for backslash is so
        // that something like: \sub.dir\afile is handled correctly. Notice that the backslash
        // as a path delimiter is operating system dependent.
        //
        const char *end_pointer = std::strchr( name, '\0' );
        while( (  end_pointer >  name ) &&
               ( *end_pointer != '.'  ) &&
               ( *end_pointer != '\\' ) ) end_pointer--;
        if( *end_pointer == '.' ) std::strcpy( raw_extension, end_pointer );
        else raw_extension[0] = '\0';

        // Let's see if this file has an extension we know about.
        int i;
        for( i = 0; default_attributes[i].extension[0] != '\0'; i++ ) {
            if( my_stricmp( raw_extension, default_attributes[i].extension ) == 0 ) break;
        }

        FileType this_type = default_attributes[i].type;
        bool     return_value = false;

        // Create the appropriate type of YEditFile. Note that this is the only place this type
        // of switch is needed. Virtual functions handle it from here.

        YEditFile *new_thing;
        switch( this_type ) {
        case ADA  : new_thing = new   ADA_YEditFile( name ); break;
        case ASM  : new_thing = new   ASM_YEditFile( name ); break;
        case C    : new_thing = new     C_YEditFile( name ); break;
        case DOC  : new_thing = new   DOC_YEditFile( name ); break;
        case PCD  : new_thing = new   PCD_YEditFile( name ); break;
        case SCALA: new_thing = new SCALA_YEditFile( name ); break;
        case OTHER: new_thing = new OTHER_YEditFile( name ); break;
        default:    new_thing = NULL;                        break;
        }

        // Do the following only if a YEditFile was created.
        if( new_thing != NULL ) {

            // Try to insert the new file into the list after the current file.
            the_list.next( );
            if( the_list.insert( new_thing ) == NULL ) {
                // It didn't work. Get rid of the file and go back where we were.
                delete new_thing;
                the_list.previous( );
            }
            else {
                // It did work. Make the new file the currently active one.
                return_value = true;
                the_list.previous( );

                // See if this file has been in the editor before and if so set up its
                // attributes to agree with the descriptor.
                //
                active_file( ).set_attributes( );
            }
        }
        return return_value;
    }


    bool lookup( const char *the_name )
    {
        YEditFile **file;
        long current_index = the_list.current_index( );

        the_list.jump_to( 0 );
        while( ( file = the_list.next( ) ) != NULL )
            if( my_stricmp( ( *file )->name( ), the_name ) == 0 ) {
                the_list.previous( );
                return true;
            }

        the_list.jump_to( current_index );
        return false;
    }


    void next( )
    {
        // Try to advance. If it fails, wrap to the head of the list.
        the_list.next( );
        if( the_list.get( ) == NULL ) the_list.jump_to( 0 );
    }


    void previous( )
    {
        // Try to back up. If it fails, wrap to the far end.
        if( the_list.previous( ) == NULL ) {
            the_list.jump_to( YFileList::off_end );
            the_list.previous( );
        }
    }


    YEditFile &active_file()
    {
        YEditFile **the_file = the_list.get( );
        return **the_file;
    }


    bool insert_active( const char *new_name )
    {
        bool return_value = false;
        YEditFile     &old_entry = active_file();
        YEditFile     *new_entry;
        FilePosition old_CP = old_entry.CP();

        // If we are trying to insert this file into itself, do nothing.
        if( my_stricmp( old_entry.name(), new_name ) == 0 ) {
            return true;
        }

        // If a file with the new name exists, make it the active one.
        if( lookup( new_name ) == true ) {
            new_entry = *the_list.get( );
            return_value = true;
        }

        // No such file, try to create/load one.
        else if( new_file(new_name ) == true ) {
            new_entry = *the_list.get( );
            return_value = true;
        }

        // If I've got a target file, then transfer the text.
        if( return_value == true ) {
            long top;
            long bottom;
            bool old_state = old_entry.get_block_state( );

            // If the old file didn't have block mode on, then block entire file.
            if( old_state == false ) {
                old_entry.top_of_file( );
                old_entry.toggle_block( );
                old_entry.bottom_of_file( );
            }
            old_entry.block_limits( top, bottom );
            old_entry.CP( ).jump_to_line( top );

            // Adjust bottom if entire file is blocked.
            bottom = ( old_state == true ) ? bottom : bottom - 1;

            // Copy the text of the original file into the new file.
            for( long line_number = top;
                 return_value == true && line_number <= bottom;
                 line_number++) {
                if( new_entry->insert_line( old_entry.get_line( ) ) == false ) {
                    error_message( "Unable to completely build new file object" );
                    return_value = false;
                }
                else {
                    new_entry->CP( ).cursor_down( );
                    old_entry .CP( ).jump_to_line( line_number + 1 );
                }
            }
            old_entry.CP( ) = old_CP;

            // If the old file had block mode on, delete the block.
            if( old_state == true ) old_entry.delete_block( );

            // In any case, turn off the block mode.
            old_entry.toggle_block( );
        }

        return return_value;
    }


    void kill( )
    {
        // Only kill if there is more than one file in the list. Y gets confused if there is no
        // file to work with.
        //
        if( the_list.size( ) > 1 ) {

            // Get a pointer to the currently active file. That's the one we are killing.
            YEditFile **file = the_list.get( );

            // Save the state information for this file in case it gets reloaded.
            FileDescriptor *new_descriptor = new FileDescriptor( ( *file )->name( ) );
            ( *file )->set_descriptor( *new_descriptor );
            new_descriptor->make_inactive( );
            new_descriptor->make_deleted( );
            descriptor_list.insert( *new_descriptor );
            delete new_descriptor;

            // Trash the file object and the list node.
            delete *file;
            the_list.erase( );

            // Wrap back to the beginning if necessary.
            if( the_list.get( ) == NULL ) the_list.jump_to( 0 );
        }
    }


    unsigned count()
    {
        return static_cast< unsigned >( the_list.size( ) );
    }


    bool save_changes( )
    {
        bool return_value = true;
        YEditFile **file;
        YFileList::Iterator stepper( the_list );

        // Look for files which have changed and save those files.
        while( ( file = stepper( ) ) != NULL ) {

            if( ( *file )->changed( ) ) {
                // Try saving the file. Update records if save worked.
                bool save_worked = ( *file )->save( ( *file )->name( ) );
                if( !save_worked ) return_value = false;
                else {
                    ( *file )->set_timestamp( ( *file )->name( ) );
                    ( *file )->mark_as_unchanged( );
                }
            }
        }

        return return_value;
    }


    bool reload_files( )
    {
        YEditFile **file;
        YFileList::Iterator stepper( the_list );

        while( ( file = stepper( ) ) != NULL ) {

            // Read date and time stamp for disk versions of files.
            FileNameMatcher  stamper;
            char            *name_string;

            stamper.set_name( ( *file )->name( ) );
            if( ( name_string = stamper.next( ) ) != NULL ) {

                // Check to see if disk version is more recent. If so, reload.
                if( stamper.modify_time( ) > ( *file )->time( ) ) {

                    // Remember current point in file.
                    FilePosition point = ( *file )->CP( );

                    // Erase the file's data.
                    ( *file )->top_of_file( );
                    ( *file )->set_block_state( true );
                    ( *file )->bottom_of_file( );
                    ( *file )->delete_block( );
                    ( *file )->set_block_state( false );
                    ( *file )->top_of_file( );

                    // Get the new stuff.
                    ( *file )->load( name_string );
                    ( *file )->set_timestamp( name_string );
                    ( *file )->CP( ) = point;
                    ( *file )->mark_as_unchanged( );
                }
            }
        }

        return true;
    }


    bool no_changes( )
    {
        YEditFile **file;
        YFileList::Iterator stepper( the_list );

        while( ( file = stepper( ) ) != NULL )
            if( ( *file )->changed( ) ) return false;

        return true;
    }

    /*================================================*/
    /*           Pertaining to the bookmark           */
    /*================================================*/

    void set_bookmark( )
    {
        mark_name  = active_file( ).name( );
        mark_point = active_file( ).CP( );
    }


    void toggle_bookmark( )
    {
        if( mark_name.length( ) == 0 )
            error_message( "No bookmark defined" );

        else {
            // Get info on current position.
            EditBuffer   temp_name  = active_file( ).name( );
            FilePosition temp_point = active_file( ).CP( );

            // Change to saved current point. First see if the file is still loaded.
            if( lookup( mark_name.to_string( ).c_str( ) ) == true ) {
                active_file( ).CP( ) = mark_point;
            }

            // If not, reload (or even create!) the file.
            else if( new_file( mark_name.to_string( ).c_str( ) ) == true ) {
                active_file( ).CP( ) = mark_point;
            }

            // Hmmm...
            else {
                error_message( "Unable to restore marked position" );
            }

            // Save the old current point as the bookmark. Doing this even in the unlikely event
            // that we couldn't restore the position just redefines the bookmark to something
            // sensible. This is good.
            //
            mark_name  = temp_name;
            mark_point = temp_point;
        }
    }

}
