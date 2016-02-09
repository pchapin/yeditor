/*! \file    ansiscr.cpp
    \brief   Implementation of simple ANSI escape handling functions.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

This file contains the implementation of a simple screen handling module. The module uses ANSI
escape sequences. It is therefore highly reliable and portable, but slow.
*/

#include <iostream>
#include "ansiscr.hpp"

namespace scr {

    namespace ansi {

        const char ESC = '\033';

        void clear_screen ( )           { std::cout << ESC << "[2J"; }
        void clear_to_eol ( )           { std::cout << ESC << "[K" ; }
        void set_color    ( int color ) { std::cout << ESC << "[" << color << "m";  }
        void bold_on      ( )           { std::cout << ESC << "[1m"; }
        void blink_on     ( )           { std::cout << ESC << "[5m"; }
        void reverse_on   ( )           { std::cout << ESC << "[7m"; }
        void reset_screen ( )           { std::cout << ESC << "[0m"; }

        void position_cursor( int row, int Column )
            { std::cout << ESC << "[" << row << ";" << Column << "H"; }

        void cursor_up       ( int count )   { std::cout << ESC << "[" << count << "A"; }
        void cursor_down     ( int count )   { std::cout << ESC << "[" << count << "B"; }
        void cursor_forward  ( int count )   { std::cout << ESC << "[" << count << "C"; }
        void cursor_backward ( int count )   { std::cout << ESC << "[" << count << "D"; }

        void save_cursor_position   ( ) { std::cout << ESC << "[s"; }
        void restore_cursor_position( ) { std::cout << ESC << "[u"; }


        //! Draw a double line border around the specified region.
        /*!
         * This function does not check the validity of its bounds.
         *
         * \param row The row coordinate of the top of the box.
         * \param column The column coordinate of the left side of the box.
         * \param width The width of the box.
         * \param height The height of the box.
         */
        void draw_border( int row, int column, int width, int height )
        {
            int i, j;

            // Draw the corners first. Looks interesting to watch.
            position_cursor( row, column );
            std::cout << "\xC9";
            position_cursor( row + height - 1, column + width - 1 );
            std::cout << "\xBC";
            position_cursor( row, column + width - 1 );
            std::cout << "\xBB";
            position_cursor( row + height - 1, column );
            std::cout << "\xC8";

            // Draw top and bottom of the border.
            i = column + 1;
            position_cursor( row, i );
            for( ; i < column + width - 1; i++ ) std::cout << "\xCD";

            i = column + 1;
            position_cursor( row + height - 1, i );
            for( ; i < column + width - 1; i++ ) std::cout << "\xCD";
            
            // Draw the sides of the border.
            j = row + 1;
            for( ; j < row + height - 1; j++ ) {
                position_cursor( j, column );
                std::cout << "\xBA";
                position_cursor( j, column + width - 1 );
                std::cout << "\xBA";
            }
        }


        //! Erase the specified region by filling with spaces.
        /*!
         * This function does not check the validity of its bounds. The color used is the
         * currently selected color.
         *
         * \param row The row coordinate of the top of the box.
         * \param column The column coordinate of the left side of the box.
         * \param width The width of the box.
         * \param height The height of the box.
         */
        void fill_box( int row, int column, int width, int height )
        {
            int i, j;

            // Nested loops do the trick slowly, but correctly.
            for( j = row; j <= row + height - 1; j++) {
                position_cursor( j, column );
                for (i = column; i <= column + width - 1; i++) {
                    std::cout << " ";
                }
            }
        }


        //! Draws a shadowed box.
        /*!
         * The following function works similarly to fill_box except that it also draws a black
         * "shadow" across the bottom of the box and down the right side of the box. This shadow
         * is one row high and two columns wide. This function assumes it will fit.
         *
         * \param row The row coordinate of the top of the box.
         * \param column The column coordinate of the left side of the box.
         * \param width The width of the box.
         * \param height The height of the box.
         */
        void fill_shadowed_box( int row, int column, int width, int height )
        {
            int index;

            // Fill the main region in the current color.
            fill_box( row, column, width, height );

            // Draw the shadow along the bottom. Assume it fits.
            set_color( B_BLACK );
            position_cursor( row + height, column + width + 1 );
            for( index = column + 2; index <= column + width + 1; index++ ) std::cout << " ";

            // Draw the shadow down the right side. Assume it fits.
            for( index = row + 1; index <= row + height; index++ ) {
                position_cursor( index, column + width );
                std::cout << "  ";
            }
        }

    }
}
