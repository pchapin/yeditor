/*! \file    DiskEditFile.cpp
 *  \brief   Implementation of class DiskEditFile
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <cstdlib>
#include <cstring>

#include "environ.hpp"

#if eOPSYS != ePOSIX
#include <dos.h>
#endif

#include "DiskEditFile.hpp"
#include "EditBuffer.hpp"
#include "FileNameMatcher.hpp"
#include "MessageWindow.hpp"
#include "scr.hpp"
#include "support.hpp"

/*=======================================*/
/*           Support Functions           */
/*=======================================*/

static bool write_line( EditBuffer *line, std::FILE *disk )
{
    int result = 0;    // Result of a disk write operation.
    int space_count;   // Number of spaces found so far.

    space_count = 0;

    // For each character on that line...
    for( std::size_t offset = 0; result != EOF  &&  offset < line->length( ); offset++ ) {

        // Don't print spaces yet.. may be nothing else on line.
        if( (*line)[offset] == ' ' ) space_count++;
        else {

            // Print any pending spaces.
            for( int i = 0; result != EOF  &&  i < space_count; i++ ) {
                result = std::putc(' ', disk);
            }

            space_count = 0;

            // Print the character.
            if( result != EOF ) {
                result = std::putc( (*line)[offset], disk );
            }
        }
    }

    // Store a '\n' at the end of each line.
    if( result != EOF ) result = std::putc( '\n', disk );

    return ( result == EOF ) ? false : true;
}


/*=======================================*/
/*           Protected Members           */
/*=======================================*/

//! Reads a file into file_data. Returns false if out of memory.
/*!
 * Reads a previously opened disk file. Lines from the file are inserted into the object's data.
 * Whatever data is already in the object is not destroyed or anyway touched.
 *
 * Notice that the name of the file is not considered. The function can handle arbitraryly long
 * lines by using a dynamic memory technique. In addition, tabs are expanded assuming 8 column
 * tab stops.
 */
bool DiskEditFile::read_disk( std::FILE *disk )
{
          int   ch;          // A character from the file.
          char *workspace;   // Pointer to the current line from file being read.
    const int   chunk_size = 128; // Memory chunk size used to handling incoming lines.
          int   chunk_count; // Number of chunks in workspace.
          int   count;       // Number of characters installed in the workspace.

    // Get some memory for the incoming line. Start with chunk_size bytes.
    workspace = static_cast< char * >( std::malloc( chunk_size * sizeof( char ) ) );
    chunk_count = 1;
    if( workspace == NULL ) return false;

    // Loop until an error occurs or the entire file is read.
    count = 0;
    bool abort = false;
    while( !abort  &&  ( ch = std::getc( disk ) ) != EOF ) {

        // Ignore non-ASCII characters (not that control characters are still processed).
        if( ch == 0 || ( ch & 0x80 ) ) continue;

        if( ch == '\n' ) {
        
            // Terminate the workspace (there will always be room) and install the line.
            workspace[count] = '\0';
            EditBuffer *new_copy = new EditBuffer( workspace );
            if( file_data.insert( new_copy ) == NULL ) abort = true;

            // Release the current workspace and grab another initial one.
            std::free( workspace );
            workspace = static_cast< char * >( std::malloc( chunk_size * sizeof( char ) ) );
            chunk_count = 1;
            if( workspace == NULL ) abort = true;

            // Reset character counter.
            count = 0;
        }
        else {

            // Install the character (there will always be room even if tabs are expanded).
            if( ch != '\t' ) workspace[count++] = static_cast< char >( ch );
            else {
                int skip_distance = 8 - ( count % 8 );
                for( int i = 0; i < skip_distance; i++ ) workspace[count++] = ' ';
            }

            // If workspace getting close to full, get a bigger space.
            if( chunk_size * chunk_count - count < 10 ) {
                workspace =
                    static_cast< char * >( std::realloc(workspace, chunk_size * ( chunk_count + 1 ) ) );
                chunk_count++;
                if( workspace == NULL ) abort = true;
            }
        }
    }

    // Install the last partial line, if there is one.
    workspace[count] = '\0';
    if( !std::ferror( disk )  &&  std::strlen( workspace ) > 0 ) {
        EditBuffer *new_copy = new EditBuffer(workspace);
        if ( file_data.insert( new_copy ) == NULL ) abort = true;
    }

    // Get rid of the current workspace.
    std::free( workspace );

    if( abort ) {
        memory_message( "Can't read entire file" );
    }

    // Tell user if it didn't work.
    if( abort || std::ferror( disk ) ) return false;
    return true;
}


//! Save file_data to a file. Returns false if disk write fails, but no message is printed.
/*!
 * Writes the data in the YEditFile to the previously opened file. The entire file is written.
 * It returns false if there is an error with the write or true otherwise.
 */
bool DiskEditFile::write_disk( std::FILE *disk )
{
    EditBuffer *line;          // Refers to the currently active line.
    int         result = 0;    // =EOF if error encountered during write.

    // Jump out if the file is empty.
    if( file_data.size( ) == 0L ) return true;

    // For each line in the EditFile object...
    file_data.jump_to(0);
    while( result != EOF && ( line = file_data.next( ) ) != NULL ) {
        result = write_line( line, disk );
    }

    return static_cast< bool >( result != EOF );
}


//! Save current block to a file. Returns false if disk write fails, but no message is printed.
bool DiskEditFile::write_disk_block( std::FILE *disk )
{
    EditBuffer *line;
    int         result = 0;

    // Learn about block extent.
    long top;
    long bottom;
    block_limits(top, bottom);

    // If block is off the end of the file, return at once.
    if( top > file_data.size( ) ) return true;

    // For each line in the block...
    file_data.jump_to( top );
    while ( result != EOF    &&
            top++ <= bottom  &&
            ( line = file_data.next( ) ) != NULL ) {
        result = write_line( line, disk );
    }

    return static_cast< bool >( result != EOF );
}


/*====================================*/
/*           Public Members           */
/*====================================*/

/*!
 * Sets the date and time stamp for a file to match that given by the file with the specified
 * name. If the file with the specified name does not exist, the date and time stamp of the
 * current file is unchanged.
 */
void DiskEditFile::set_timestamp( const char *name )
{
    FileNameMatcher stamper;

    stamper.set_name( name );
    if( stamper.next( ) != 0 ) {
        file_time = stamper.modify_time( );
    }
}


/*!
 * Allows the client programs to change the status of is_changed. Normally is_changed is not
 * avaible to clients. It makes sense to let clients who need file I/O abilities to control the
 * status of is_changed, since it's existance has mostly to do with when (and if) files should
 * be saved at the next opportunity.
 *
 * For example, when a file is renamed, it should be marked as changed so that the high level
 * saving operations know to save that file.
 */
void DiskEditFile::mark_as_changed( )
{
    is_changed = true;
    return;
}


/*!
 * Allows the client programs to change the status of is_changed. Normally is_changed is not
 * avaible to clients. It makes sense to let clients who need file I/O abilities to control the
 * status of is_changed, since it's existance has mostly to do with when (and if) files should
 * be saved at the next opportunity.
 *
 * For example, when a file is renamed, it should be marked as changed so that the high level
 * saving operations know to save that file.
 */
void DiskEditFile::mark_as_unchanged( )
{
    is_changed = false;
    return;
}


/*!
 * Tries to load the named file into the object. Since it uses read_file() from above, this load
 * will insert the named file into whatever is currently in the object. By making sure the
 * object is initially empty, this function can do complete loads as well as insertions.
 */
bool DiskEditFile::load( const char *the_name )
{
    // Assume all will work.
    bool result;

    // Synchronize the list with the cursor, extend the list if needed. NOTE: the (-1) below is
    // to prevent the extend operation from, effectively, inserting an extra line in the file.
    //
    if( !extend_to_line( current_point.cursor_line( ) - 1 ) ) return false;
    file_data.jump_to( current_point.cursor_line( ) );

    // Try to open the file.
    std::FILE *disk;
    if( ( disk = std::fopen( the_name, "r" ) ) == NULL ) {
        error_message( "Can't open %s for reading", the_name );
        return false;
    }

    // Inform the user that we're reading a file.
    std::string buffer( "Reading " );
    buffer.append( the_name );
    buffer.append( "..." );
    scr::MessageWindow Teaser( buffer.c_str( ), scr::MESSAGE_WINDOW_MESSAGE );
    scr::refresh( );

    // Do the dirty work and record result for after Teaser window is gone.
    result = read_disk( disk );
    std::fclose( disk );

    Teaser.close( );

    if( result == false ) {
        warning_message( "Problems reading %s. File may be incomplete", the_name );
    }

    return result;
}


/*!
 * Saves the data to the named file. Depending on save_mode either the whole file is saved or
 * just the active block is saved. This function is complicated by the need to check the result
 * of std::fclose(). For writes, std::fclose() dumps the last part of the file. To truely know
 * if the file was all written correctly, we must check the result of std::fclose(). The return
 * value from write_file() is not enough.
 *
 * Although this is kind of a pain, it leaves write_file() generic. For example, write_file()
 * could be used several times to write different chunks of data to the same file (although Y
 * currently does not do this).
 */
bool DiskEditFile::save( const char *the_name, Mode save_mode )
{
    // We don't attempt to deal with read-only files intelligently on POSIX.
    #if eOPSYS != ePOSIX
    bool read_only = false;

    // If the file exists and has the read-only attribute, remember it.
    unsigned file_attributes = 0;
    #if eOPSYS == eWIN32
    if( ( file_attributes = GetFileAttributes( the_name ) ) != INVALID_FILE_ATTRIBUTES &&
        ( file_attributes & FILE_ATTRIBUTE_READONLY ) ) {
    #else
    if( _dos_getfileattr( the_name, &file_attributes ) == 0 &&
        ( file_attributes & _A_RDONLY ) ) {
    #endif
        read_only = true;

        // Tell the user that it's read-only. Ask for permission to continue.
        EditBuffer working_buffer( the_name );
        working_buffer.append( " is read only. Write anyway? y/[n] " );
        if( confirm_message( working_buffer.to_string( ).c_str( ), 'y', true ) == true )
            return false;
        else {
            // Force the file to non read-only mode. If we can't, exit.
            #if eOPSYS == eWIN32
            if( SetFileAttributes(the_name, file_attributes & ~FILE_ATTRIBUTE_READONLY ) == FALSE ) {
            #else
            if( _dos_setfileattr(the_name, file_attributes & ~_A_RDONLY) != 0 ) {
            #endif
                error_message( "Can't remove read-only attribute on %s", the_name );
                return false;
            }
        }
    }
    #endif

    // Try to open the file.
    std::FILE *disk;
    if( ( disk = std::fopen( the_name, "w" ) ) == NULL ) {
        error_message( "Can't open %s for output", the_name );

        #if eOPSYS != ePOSIX
        // If the file exists and we converted its attributes, let's set them back here. This
        // may be the case in a networked environment where the user has rights to modify a
        // file's attributes, but not write to the file.
        //
        if( read_only ) {
            #if eOPSYS == eWIN32
            SetFileAttributes( the_name, file_attributes );
            #else
            _dos_setfileattr( the_name, file_attributes );
            #endif
        }
        #endif
        return false;
    }

    // Tell user we're working on this file.
    std::string buffer( "Writing " );
    buffer.append( the_name );
    buffer.append( "..." );
    scr::MessageWindow teaser( buffer.c_str( ), scr::MESSAGE_WINDOW_MESSAGE );
    scr::refresh( );

    // Do the bulk of the work.
    bool result1;
    if( save_mode == ALL ) result1 = write_disk( disk );
    else result1 = write_disk_block( disk );

    bool result2 = static_cast< bool >( std::fclose( disk ) == 0 );

    // Close teaser window after std::fclose() since std::fclose() does writes too.
    teaser.close( );

    // result == true only if both Write_Disk() and std::fclose() worked.
    bool result = static_cast< bool >( result1 == true  &&  result2 == true );

    // Tell user if there are problems.
    if( result == false ) {
        warning_message( "Problems writing %s. File may have been incompletely saved", the_name );
    }

    #if eOPSYS != ePOSIX
    // If we converted this file's attributes, set them back. We can assume this will work... we
    // must have successfully changed the file's attributes above in order to be here! Note that
    // by doing this conditionally, we reduce the number of OS system calls.
    //
    if (read_only) {
        #if eOPSYS == eWIN32
        SetFileAttributes( the_name, file_attributes );
        #else
        _dos_setfileattr( the_name, file_attributes );
        #endif
    }
    #endif
    return result;
}
