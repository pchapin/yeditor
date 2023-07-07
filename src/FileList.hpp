/*! \file    FileList.hpp
 *  \brief   Interface to the FileList abstract object
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef FILELIST_HPP
#define FILELIST_HPP

class YEditFile;

//! Encloses functions that manipulate the file list abstract object.
/*!
 * The file list is Y's list of currently open files. This namespace contains functions for
 * manipulating and controlling that list. Note that the file list is polymorphic. Different
 * file types provide specialized editing experiences as appropriate. Y stores these different
 * types in a common file list and uses virtual functions to handle the specialized editing
 * operations.
 *
 * The FileList maintains a notion of "currently active file." This is the file with which the
 * user is interacting. Clients of FileList get a handle to the currently active file using
 * FileList::active_file( ). Several of the functions in FileList change the active file and
 * thus invalidate any previously obtained handles. Clients need to be aware of this.

 * Operations defined for the FileList are the sort that require knowledge about more than one
 * file. Operations that only affect one file are defined in YEditFile's interface. A future
 * version of FileList may support multiple file lists (if needed) and may support a somewhat
 * cleaner way of keeping track of active file handles. In addition, some of the functionality
 * in YEditFile might get moved here eventually. In particular, YEditFile::display( ) knows
 * nothing about the other files. In a multi-windowed version of Y that maintained images of
 * several different files on the screen at once, display( ) might have to be in FileList.
 */
namespace FileList {

    //! Returns a reference to the active YEditFile.
    YEditFile &active_file( );

    //! Returns the number of files currently in the list.
    unsigned count( );

    //! Inserts active file into specified file.
    /*!
     * This is a somewhat strange function. Is there a better (more general) way to handle the
     * problem this function solves?
     */
    bool insert_active( const char *name );

    //! Removes active file from list and marks next file as active.
    void kill( );

    //! Activates a currently loaded file with the given name.
    bool lookup( const char *name );

    //! Makes the next file in the list the focus of user interaction (active).
    void next( );

    //! Creates a new YEditFile and tries to load it from disk.
    bool new_file( const char *name );

    //! Returns TRUE if no files are changed.
    bool no_changes( );

    //! Makes the previous file in the list the focus of user interaction (active).
    void previous( );

    //! Read more recent files from disk.
    bool reload_files( );

    //! Save all files that have changed and resets their changed flag.
    bool save_changes( );

    //! Remembers current file and position.
    void set_bookmark( );

   //! Exchanges bookmark and current point.
    void toggle_bookmark( );
};

#endif
