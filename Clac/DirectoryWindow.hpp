/*! \file    DirectoryWindow.hpp
 *  \brief   Definition of the DirectoryWindow class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * Objects of this class manage the display of a DirectoryEntity.
 */

#ifndef DIRECTORYWINDOW_HPP
#define DIRECTORYWINDOW_HPP

// The Scr library.
#include "ImageBuffer.hpp"
#include "Manager.hpp"
#include "Window.hpp"

#include "DirectoryEntity.hpp"

class DirectoryWindow : public scr::Window {
public:
    // Be sure there is no association initially.
    DirectoryWindow( scr::Manager &M, int R, int C, int W, int H ) :
        scr::Window( &M, R, C, W, H ), the_directory( nullptr ) { }

    // Associate this window with a DirectoryEntity object.
    void associate( DirectoryEntity *object ) { the_directory = object; }

    virtual scr::ImageBuffer *get_image( ) override;

private:
    DirectoryEntity *the_directory;
};

#endif


