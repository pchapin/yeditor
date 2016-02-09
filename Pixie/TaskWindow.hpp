/*! \file    task_window.h
    \brief   Definition of the TaskWindow class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <ctime>
#include <string>
#include <vector>

#include "Date.hpp"
#include "Window.hpp"

//! Structure that holds information about a single task.
struct PixieTask {
    std::string description;       //!< Description of the task as presented to the user.
    int         priority;          //!< Range 1 .. 99
    std::time_t start_time;        //!< Zero implies that the task is not active.
    int         accumulated;       //!< Total number of minutes applied to this task.
    int         accumulated_today; //!< Total number of minutes applied to this task today.
    int         daily;             //!< Number of minutes of attention this task requires each day.
    int         accumulated_debt;  //!< Total number of minutes of attention this task requires. A credit is negative.
};


//! A window that displays and manages the task list.
/*!
 * \todo Pixie should use a model-view-controller design where the task window is only concerned
 * about display issues. The actual task management methods should be moved to a non-UI class.
 */
class TaskWindow : public scr::Window {
public:
    //! Construct a task window.
    /*!
     * The constructor associates the task window with a window manager and specifies the
     * initial location and size of the window using scr coordinates. The constructor also reads
     * the task file. If no task file exists, the error is ignored. The first save of the task
     * file will create it.
     *
     * \todo Provide a way to specify the location of the task file, otherwise all TaskWindow
     * instances will use the same file which limits flexibility.
     */
    TaskWindow( scr::Manager *my_manager, int row, int column, int width, int height );
   ~TaskWindow( );

    //! Add the specified number of minutes to the indicated task.
    void add_minutes( int task_number, int additional_minutes );

    //! Change the daily allocation of the indicated task to the new amount of minutes.
    void change_daily( int task_number, int new_daily );

    //! Change priority of the indicated task to the new priority level.
    void change_priority( int task_number, int new_priority );

    //! Create a task with the given description and priority. Other attributes are default.
    void create_task( const std::string &new_description, int initial_priority );

    //! Delete a task given it's index in the task list.
    void delete_task( int task_number );

    //! Rename the indicated task using the new description text.
    void rename( int task_number, const std::string &new_description );

    //! Save the tasks to the task list used during construction.
    void save_tasks( );

    //! Start working on the indicated task.
    void start_task( int task_number );

    //! Stop working on all active tasks.
    void stop_tasks( );

    //! Remove one day's worth of daily allocations.
    void undo_daily( );

    //! Zero accumulated times for all tasks. This also stops any active tasks.
    void zero_tasks( );

    //! Return a current image of the task window for the window manager to display.
    virtual scr::ImageBuffer *get_image( );

private:
    spica::Date today;              //!< Today's date.
    std::string task_file_name;     //!< Name of the task file.
    std::vector< PixieTask > tasks; //!< The tasks themselves.

    //! Read tasks from the task list.
    bool read_tasks( );

    //! Write tasks back to the task list.
    bool write_tasks( );
};

#endif
