/*! \file    StackWindow.cpp
 *  \brief   Implementation of the StackWindow class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <cstdio>
#include <cstdlib>

#include "scr.hpp"
#include "StackWindow.hpp"

using namespace std;

scr::ImageBuffer *StackWindow::get_image( )
{
    char number_buffer[16];

    image.clear( );
    for( int i = 0; i < image.get_height( ); i++ ) {
        int item_number = image.get_height( ) - (i + 1);

        // Get the right stack level number and print it into the window.
        int count = sprintf( number_buffer, "%2d: ", item_number );
        image.copy( number_buffer, i + 1, 1, image.get_width( ), scr::WHITE );

        Entity *thing = the_stack->get( item_number );

        // If the object exists, print it in the window.
        if( thing != NULL ) {
            image.copy( thing->display( ).c_str( ),
                        i + 1,
                        count + 1,
                        image.get_width( ) - count,
                        scr::WHITE );
        }
    }
    return &image;
}
