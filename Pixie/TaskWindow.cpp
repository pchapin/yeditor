/*! \file    task_window.cpp
    \brief   Implementation of the TaskWindow class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "debug.hpp"
#include "TaskWindow.hpp"

using namespace std;

//! Sorts pixie tasks in the task window.
/*!
 * This function implements Pixie's task priority logic.
 *
 * \todo The way in which Pixie sorts tasks should be configurable.
 */
static bool compare_tasks( const PixieTask &left, const PixieTask &right )
{
    bool left_hot  = ( left.daily  != 0 && left.accumulated_debt  > 0 );
    bool right_hot = ( right.daily != 0 && right.accumulated_debt > 0 );

    if( left_hot && right_hot ) {
        if( left.priority == right.priority )
            return left.accumulated_debt > right.accumulated_debt;
        else
            return left.priority > right.priority;
    }

    if( left_hot  && !right_hot ) return true;
    if( right_hot && !left_hot  ) return false;

    return ( right.priority * left.accumulated ) < ( left.priority * right.accumulated );
}


//! Parse a line from the task file.
/*!
 * This function locates the start time, accumulated time, priority, and task description in
 * that line and uses that information to load up the given task object. This function returns
 * true if it is successful, and false otherwise.
 *
 * \todo This function is a horrible hack. However, don't waste time fixing it. Eventually the
 * input file will be XML and read using an XML parser.
 */
static bool parse_line( const string &line, PixieTask &new_task )
{
    // Skip leading white space to locate the start of the first word.
    std::size_t word1_start = line.find_first_not_of( " \t", 0 );
    if( word1_start == string::npos ) return false;

    // Find the end of this word.
    std::size_t word1_end = line.find_first_of( " \t", word1_start );
    if( word1_end == string::npos ) return false;

    string word1 = line.substr( word1_start, word1_end - word1_start );

    // Similarly for the second word...
    std::size_t word2_start = line.find_first_not_of( " \t", word1_end );
    if( word2_start == string::npos ) return false;

    std::size_t word2_end = line.find_first_of( " \t", word2_start );
    if( word2_end == string::npos ) return false;

    string word2 = line.substr( word2_start, word2_end - word2_start );

    // Similarly for the third word...
    std::size_t word3_start = line.find_first_not_of( " \t", word2_end );
    if( word3_start == string::npos ) return false;

    std::size_t word3_end = line.find_first_of( " \t", word3_start );
    if( word3_end == string::npos ) return false;

    string word3 = line.substr( word3_start, word3_end - word3_start );

    // Similarly for the fourth word...
    std::size_t word4_start = line.find_first_not_of( " \t", word3_end );
    if( word4_start == string::npos ) return false;

    std::size_t word4_end = line.find_first_of( " \t", word4_start );
    if( word4_end == string::npos ) return false;

    string word4 = line.substr( word4_start, word4_end - word4_start );

    // Similarly for the fifth word...
    std::size_t word5_start = line.find_first_not_of( " \t", word4_end );
    if( word5_start == string::npos ) return false;

    std::size_t word5_end = line.find_first_of( " \t", word5_start );
    if( word5_end == string::npos ) return false;

    string word5 = line.substr( word5_start, word5_end - word5_start );

    // Similarly for the sixth word...
    std::size_t word6_start = line.find_first_not_of( " \t", word5_end );
    if( word6_start == string::npos ) return false;

    std::size_t word6_end = line.find_first_of( " \t", word6_start );
    if( word6_end == string::npos ) return false;

    string word6 = line.substr( word6_start, word6_end - word6_start );

    // Locate the start of the description.
    std::size_t word7_start = line.find_first_not_of( " \t", word6_end );
    if( word7_start == string::npos ) return false;

    // Load up the result.
    new_task.start_time        = atoi( word1.c_str( ) );
    new_task.accumulated       = atoi( word2.c_str( ) );
    new_task.accumulated_today = atoi( word3.c_str( ) );
    new_task.daily             = atoi( word4.c_str( ) );
    new_task.priority          = atoi( word5.c_str( ) );
    new_task.accumulated_debt  = atoi( word6.c_str( ) );
    new_task.description       = line.substr( word7_start );
    return true;
}

//=========================
//     Private Methods
//=========================

bool TaskWindow::read_tasks( )
{
    scr::Tracer( "TaskWindow::read_tasks", 3 );
    spica::Date file_date;
    string      line;

    // Open the file. If it doesn't exist, that's not an error. That way a user without a project
    // file can use the program to create his/her project list.
    //
    ifstream task_file( task_file_name.c_str( ) );
    if( !task_file ) {
        return true;
    }

    tasks.clear( );

    // Read the date at the start of the file.
    task_file >> file_date;
    getline( task_file, line );  // Eat the rest of the first line.

    // Read the project file one line at a time.
    while( getline( task_file, line)) {
        PixieTask new_task;
        if( !parse_line( line, new_task ) ) {
#ifdef NEVER
            cerr << "Error in project file! Line: "
                 << line_counter << ", File: " << file_name << endl;
#endif
            return false;
        }
        if( file_date != today ) {
            new_task.accumulated_today = 0;
            if( new_task.daily != 0 )
                new_task.accumulated_debt += (workday_difference(today, file_date)) * new_task.daily;
        }
        tasks.push_back( new_task );
    }
    return true;
}


bool TaskWindow::write_tasks( )
{
    scr::Tracer( "TaskWindow::write_tasks", 3 );
    // Open the output file.
    ofstream task_file( task_file_name.c_str( ) );
    if( !task_file ) {
#ifdef NEVER
        cerr << "Can't open project file: " << file_name << endl;
#endif
        return false;
    }

    task_file << today << "\n";

    // Step down the vector and output one line for each project.
    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); i++ ) {
        task_file << tasks[i].start_time        << " "
                  << tasks[i].accumulated       << " "
                  << tasks[i].accumulated_today << " "
                  << tasks[i].daily             << " "
                  << tasks[i].priority          << " "
                  << tasks[i].accumulated_debt  << " "
                  << tasks[i].description       << endl;
    }
    return true;
}

//========================
//     Public Methods
//========================

TaskWindow::TaskWindow( scr::Manager *my_manager, int row, int column, int width, int height ) :
    scr::Window( my_manager, row, column, width, height )
{
    scr::Tracer( "TaskWindow::TaskWindow", 1 );

    // Figure out today's date and store it.
    time_t now = time( NULL );
    struct tm *cooked_time = localtime( &now );
    today.set(cooked_time->tm_year + 1900, cooked_time->tm_mon + 1, cooked_time->tm_mday );

    // Read the task file.
    task_file_name =  getenv( "HOME" );
    task_file_name += '/';
    task_file_name += ".pixie-tasks";
    read_tasks( );
}


TaskWindow::~TaskWindow( )
{
    scr::Tracer( "TaskWindow::~TaskWindow", 1 );
    save_tasks( );
}


void TaskWindow::add_minutes( int task_number, int additional_minutes)
{
    scr::Tracer( "TaskWindow::add_minutes", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;

    tasks[task_number].accumulated += additional_minutes;
    tasks[task_number].accumulated_today += additional_minutes;
    if( tasks[task_number].daily != 0 ) {
        tasks[task_number].accumulated_debt -= additional_minutes;
    }
}


void TaskWindow::change_daily( int task_number, int new_daily )
{
    scr::Tracer( "TaskWindow::change_daily", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;
    if( new_daily < 0 ) return;

    if( new_daily == 0 ) {
        tasks[task_number].daily = 0;
        tasks[task_number].accumulated_debt = 0;
    }
    else {
        int debt_adjustment = new_daily - tasks[task_number].daily;
        tasks[task_number].daily = new_daily;
        tasks[task_number].accumulated_debt += debt_adjustment;
    }
}


void TaskWindow::change_priority( int task_number, int new_priority )
{
    scr::Tracer( "TaskWindow::change_priority", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;
    if( new_priority < 1 || new_priority >= 100 ) return;

    tasks[task_number].priority = new_priority;
}


// Why doesn't PixieTask have a constructor?
void TaskWindow::create_task( const string &new_description, int initial_priority )
{
    scr::Tracer( "TaskWindow::create_task", 2 );
    PixieTask new_task;

    new_task.description       = new_description;
    new_task.priority          = initial_priority;
    new_task.start_time        = 0;
    new_task.accumulated       = 0;
    new_task.accumulated_today = 0;
    new_task.daily             = 0;
    new_task.accumulated_debt  = 0;
    tasks.push_back( new_task );
}


void TaskWindow::delete_task( int task_number )
{
    scr::Tracer( "TaskWindow::delete_task", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;

    vector< PixieTask >::iterator delete_me = tasks.begin( ) + task_number;
    tasks.erase( delete_me );
}


void TaskWindow::rename( int task_number, const std::string &new_description )
{
    scr::Tracer( "TaskWindow::rename", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;

    tasks[task_number].description = new_description;
}


void TaskWindow::save_tasks( )
{
    scr::Tracer( "TaskWindow::save_tasks", 2 );
    write_tasks( );
}


void TaskWindow::start_task( int task_number )
{
    scr::Tracer( "TaskWindow::start_task", 2 );
    task_number--;
    if( task_number < 0 || static_cast< unsigned >( task_number ) >= tasks.size( ) ) return;

    time_t raw_time = time( 0 );
    tasks[task_number].start_time = raw_time;
}


void TaskWindow::stop_tasks( )
{
    scr::Tracer( "TaskWindow::stop_tasks", 2 );
    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); i++ ) {
        if( tasks[i].start_time != 0 ) {
            int minutes = static_cast< int >( time( 0 ) - tasks[i].start_time ) / 60;
            tasks[i].accumulated       += minutes;
            tasks[i].accumulated_today += minutes;
            if( tasks[i].daily != 0 ) {
                tasks[i].accumulated_debt -= minutes;
            }
            tasks[i].start_time = 0;
        }
    }
}


void TaskWindow::undo_daily( )
{
    scr::Tracer( "TaskWindow::undo_daily", 2 );
    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); i++ ) {
        tasks[i].accumulated_debt -= tasks[i].daily;
    }
}


void TaskWindow::zero_tasks( )
{
    scr::Tracer( "TaskWindow::zero_tasks", 2 );
    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); i++ ) {
        tasks[i].start_time        = 0;
        tasks[i].accumulated       = 0;
        tasks[i].accumulated_today = 0;
        if( tasks[i].daily != 0 ) {
            tasks[i].accumulated_debt = tasks[i].daily;
        }
    }
}


scr::ImageBuffer *TaskWindow::get_image( )
{
    int total_priority_value = 0;

    scr::Tracer( "TaskWindow::get_image", 4 );
    stable_sort( tasks.begin( ), tasks.end( ), compare_tasks );

    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); ++i ) {
        total_priority_value += tasks[i].priority;
    }

    image.clear( );
    for( vector< PixieTask >::size_type i = 0; i < tasks.size( ); ++i ) {
        bool is_hot = ( tasks[i].daily && tasks[i].accumulated_debt > 0 );
        int color;
        ostringstream formatter;

        formatter << setw(2) << i + 1 << ") ";
        if( tasks[i].start_time != 0 ) {
            formatter << "* ";
            color = scr::BRIGHT | scr::WHITE | ( is_hot ? scr::REV_RED : scr::REV_BLACK );
        }
        else {
            formatter << "  ";
            color = scr::WHITE | ( is_hot ? scr::REV_RED : scr::REV_BLACK );
        }
        formatter << "time="  << setw(4) << tasks[i].accumulated << "/"
                              << setw(4) << tasks[i].accumulated_debt << ", ";    // Could be negative.
        formatter << "today=" << setw(3) << tasks[i].accumulated_today << "/"
                              << setw(3) << tasks[i].daily << ", ";
        formatter << "prio="  << setw(2) << tasks[i].priority << " ("
                              << setw(5) << fixed << setprecision(1) << 100 * static_cast<double>( tasks[i].priority ) / total_priority_value << "%), ";
        formatter << tasks[i].description;
        image.copy( formatter.str( ).c_str( ), i + 1, 1, image.get_width( ), color );
    }
    return &image;
}
