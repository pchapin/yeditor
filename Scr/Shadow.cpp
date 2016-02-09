/*! \file    Shadow.cpp
    \brief   Implementation of class Shadow.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstring>

#include "scr.hpp"
#include "Shadow.hpp"

namespace scr {

    //! Creates a shadow.
    /*!
     * Before changing the attributes, this function saves the data in the region. Although only
     * attributes need to be saved, this function saves both text and attributes as a
     * convenience. When the shadow is erased, the text and attributes are written back. It is
     * assumed that the shaded material is not changed while the shadow is on it.
     *
     * \param row The row coordinate of the region's upper left corner.
     * \param column The column coordinate of the region's upper left corner.
     * \param width The width of the region.
     * \param height The height of the region.
     */
    void Shadow::open( int row, int column, int width, int height )
    {
        char *workspace;

        // Ignore call if shadow already open.
        if( background != NULL ) return;

        // Adjust size so that shadow does not overlap the boundary of the screen.
        if( row    + height > number_of_rows( ) + 1 ) height = number_of_rows( ) + 1 - row;
        if( column + width  > number_of_columns( ) + 1 ) width  = number_of_columns( ) + 1 - column;

        // Get memory to hold the true image. Only attributes are really necessary.
        background = new char[2 * width * height];
        if( background != NULL ) {

            // Copy true image into storage.
            read( row, column, width, height, background );
            top_row       = row;
            left_column   = column;
            shadow_width  = width;
            shadow_height = height;
    
            // Get workspace.
            workspace = new char[2 * width * height];
            if( workspace != NULL ) {

                // Get another copy of the image.
                std::memcpy( workspace, background, 2 * width * height );

                // Set all attributes to shadow color.
                for( int i = 1; i < 2 * width * height; i += 2 ) {
                    workspace[i] = BRIGHT | BLACK | REV_BLACK;
                }

                // Write shadow back onto screen and erase workspace.
                write( row, column, width, height, workspace );
                delete workspace;
            }
        }
    }


    //! Restores the orginal text and attributes.
    void Shadow::close( )
    {
        // Do something only if there exists a saved background.
        if( background != NULL ) {

            // Write the original attributes (and text) back onto the screen.
            write( top_row, left_column, shadow_width, shadow_height, background );
            delete background;
            background = NULL;
        }
    }

}
