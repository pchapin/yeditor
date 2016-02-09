/*! \file    StatusLine.cpp
    \brief   Implementation of class StatusLine.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "scr.hpp"
#include "StatusLine.hpp"

static const char *default_status( )
{
    return "Uninitialized scr::Status_Line object used";
}


namespace scr {

    //! Constructor.
    StatusLine::StatusLine( )
    {
        make_line = default_status;
    }


    //! Opens and prints the StatusLine.
    /*!
     * This method creates the status line and prints into it the string returned from the line
     * generation function. Note that status lines always have a height of 1 and no border.
     *
     * \param row The screen row on which the line appears.
     * \param column The column where the line starts.
     * \param width The width of the space where the line is drawn.
     * \param attribute The color attribute used for the line.
     */
    bool StatusLine::open( int row, int column, int width, int attribute )
    {
        bool return_value =
            SimpleWindow::open( row, column, width, 1, attribute, NO_BORDER, attribute );

        if( return_value == true )
            print_text( row, column, width, make_line( ) );

        return return_value;
    }


    //! Shows and updates a StatusLine
    /*!
     * This method shows and updates a hidden status line. Because showing a window that is
     * already visible has no effect on that window, this method can also be used to update a
     * visible status line.
     */
    void StatusLine::show( )
    {
        if( is_defined ) {
            SimpleWindow::show( );
            print_text( row( ), column( ), width( ), make_line( ) );
        }
    }

}
