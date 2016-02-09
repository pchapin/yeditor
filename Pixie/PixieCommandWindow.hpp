/*! \file    pixie_commandw.h
    \brief   Definition of the PixieCommandWindow class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef PIXIECOMMANDW_H
#define PIXIECOMMANDW_H

#include "CommandWindow.hpp"

class TaskWindow;

class PixieCommandWindow : public scr::CommandWindow {
public:
    PixieCommandWindow( scr::Manager *my_manager, int row, int column, int width, int height, TaskWindow *task_window ) :
      scr::CommandWindow( my_manager, row, column, width, height ), main_window( task_window )
      { }

    bool process_keystroke( int &key_code );

private:
    TaskWindow *main_window;
};

#endif
