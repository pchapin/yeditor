/*! \file    ClacCommandWindow.hpp
 *  \brief   Definition of the ClacCommandWindow class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef CLACCOMMANDWINDOW_H
#define CLACCOMMANDWINDOW_H

#include "CommandWindow.hpp"

class ClacCommandWindow : public scr::CommandWindow {
public:
    ClacCommandWindow( scr::Manager *my_manager, int row, int column, int width, int height ) :
      scr::CommandWindow( my_manager, row, column, width, height )
      { }

    bool process_keystroke( int &key_code ) override;
};

#endif
