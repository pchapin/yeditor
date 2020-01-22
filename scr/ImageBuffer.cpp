/*! \file    ImageBuffer.cpp
    \brief   Implementation of class ImageBuffer.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cstring>
#include "ImageBuffer.hpp"
#include "scr.hpp"

static
void check_region( int row, int column, int width, int height )
{
    if( row    < 1 ||
        column < 1 ||
        row + height - 1   > scr::number_of_rows( ) ||
        column + width - 1 > scr::number_of_columns( ) )
            throw scr::BadRegion( row, column, width, height );
}


namespace scr {

    //! Constructs an Image_Buffer
    /*!
     * This constructor allocates memory to hold an image with the specified dimensions. The
     * image is initialized with all character cells using the specified character and using the
     * specified color attribute. An image remembers its dimensions. Note that images must
     * contain at least one row and one column. If the provided width and height values are less
     * than one, a size of one is used instead.
     *
     * \param width The width of the image in column positions.
     * \param height The height of the image in row positions.
     * \param color The color attribute used to initialize each character cell.
     * \param letter The character used to initialize each character cell.
     *
     * \throws std::bad_alloc If there is insufficient memory to create the image.
     */
    ImageBuffer::ImageBuffer( int width, int height, int color, char letter )
    {
        if( width < 1 ) width = 1;
        if( height < 1 ) height = 1;

        this->width  = width;
        this->height = height;
        buffer = new char[ 2 * width * height ];
        clear( color, letter );
    }


    //! Copies an ImageBuffer
    /*!
     * This constructor copies the given image.
     *
     * \param other The existing image to be copied.
     * \throws std::bad_alloc If there is insufficient memory to copy the existing image.
     */
    ImageBuffer::ImageBuffer( const ImageBuffer &other )
    {
        width  = other.width;
        height = other.height;
        buffer = new char[ 2 * width * height ];
        std::memcpy( buffer, other.buffer, 2 * width * height );
    }


    //! Destroys an ImageBuffer
    ImageBuffer::~ImageBuffer( )
    {
        delete [] buffer;
    }


    //! Assigns an ImageBuffer
    /*!
     * This method assigns one image to another. If there is insufficient memory to create the
     * copy, the destination image is not changed.
     *
     * \param other The existing image to be copied.
     * \throws std::bad_alloc If there is insufficient memory to copy the existing image.
     */
    ImageBuffer &ImageBuffer::operator=( const ImageBuffer &other )
    {
        char *temp_buffer = new char[ 2 * other.width * other.height ];

        width  = other.width;
        height = other.height;
        delete [] buffer;
        buffer = temp_buffer;
        std::memcpy( buffer, other.buffer, 2 * width * height );
        return *this;
    }


    //! Clears an ImageBuffer
    /*!
     * This method sets the image to repeated occurences of the specified character using the
     * specified color attribute.
     *
     * \param color The color attribute to used to initialize each character cell.
     * \param letter The character used to initialized each character cell.
     */
    void ImageBuffer::clear( int color, char letter )
    {
        for( int i = 0; i < width * height; ++i ) {
            buffer[ 2*i ] = letter;
            buffer[ 2*i + 1 ] = static_cast< char >( color );
        }
    }


    //! Copies a C style string into an ImageBuffer
    /*!
     * This method copies the characters in the given C style (null terminated) string into an
     * image, starting at the specified position in the image and using the specified color
     * attribute for each character. It is acceptable for the C string to be longer than the
     * length of a row. In that case, the string is wrapped to the next row in the image. If the
     * C string is longer than the space in the image can hold, the extra characters are
     * truncated.
     *
     * \param source Pointer to the C style (null terminated) string to copy.
     * \param row The starting row where the copy will be placed (first row is 1).
     * \param column The starting column where the copy will be placed (first column is 1).
     * \param extent The maximum number of characters to copy.
     * \param color The color attribute to use for each character copied into the image.
     */
    void ImageBuffer::copy( const char *source, int row, int column, std::size_t extent, int color )
    {
        // Make sure starting position is in range.
        if( row    < 1 ) row = 1;
        if( column < 1 ) column = 1;

        // Compute the number of character positions left in the region.
        int offset = ( row - 1 ) * width + ( column - 1 );
        int space_left = width * height - offset;

        // Truncate the string at the end of the region.
        if( space_left <= 0 ) return;
        if( extent > static_cast<std::size_t>( space_left ) ) extent = space_left;

        // Copy the characters to the region.
        char *p = buffer + 2 * offset;
        while( *source && extent != 0 ) {
            *( p + 0 ) = *source;
            *( p + 1 ) =  color;
            p += 2;
            ++source;
            --extent;
        }
    }


    //! Reads an ImageBuffer from the screen.
    /*!
     * This method reads the characters (and their color attributes) from the screen and places
     * them into the image. The row and column parameters specify the upper left corner of the
     * screen region that is read. The image's width and height specify the other dimensions of
     * the region. If this method is used to read a region that overlaps the screen's
     * boundaries, a Bad_Region exception is thrown.
     *
     * \param row The upper left corner of the screen region that is read (first row is 1).
     * \param column The upper left corner of the screen region that is read (first column is 1).
     * \throws Bad_Region if the region being read overlaps or is outside the screen boundary.
     */
    void ImageBuffer::read( int row, int column )
    {
        check_region( row, column, width, height );
        scr::read( row, column, width, height, buffer );
    }


    //! Writes an ImageBuffer to the screen.
    /*!
     * This method writes the characters (and their color attributes) from the image to the
     * screen. The row and column parameters specify the upper left corner of the screen region
     * that is written. The image's width and height specify the other dimensions of the region.
     * If this method is used to write a region that overlaps the screen's boundaries, a
     * BadRegion exception is thrown.
     *
     * \param row The upper left corner of the screen region that is written (first is 1).
     * \param column The upper left corner of the screen region that is written (first is 1).
     * \throws Bad_Region if the region being written overlaps or is outside the screen boundary.
     */
    void ImageBuffer::write( int row, int column )
    {
        check_region( row, column, width, height );
        scr::write( row, column, width, height, buffer );
    }


    //! Resizes an ImageBuffer
    /*!
     * This method resizes an existing image. If the number of rows or the number of columns are
     * made smaller, the data in the removed rows or columns is lost. If the number of rows or
     * the number of colums is made larger, the new space is initialized with the specified
     * character and the specified color attribute. Note that images must contain at least one
     * row and one column. If the provided width and height values are less than one, a size of
     * one is used instead. Note also that this method might throw a std::bad_alloc exception
     * even if the image is being reduced in size.
     *
     * \param new_width The desired new width of the region.
     * \param new_height The desired new height of the region.
     * \param color The color attribute used to initialize new character cells (if any).
     * \param letter The character used to initialize new character cells (if any).
     * \throws std::bad_alloc If there is insufficient memory to create the new image.
     */
    void ImageBuffer::resize( int new_width, int new_height, int color, char letter )
    {
        if( new_width  < 1 ) new_width  = 1;
        if( new_height < 1 ) new_height = 1;

        char *temp = new char[ 2 * new_width * new_height ];

        // Copy old data one row at a time.
        for( int i = 1; i <= new_height; ++i ) {

            // Fill row i of the new region with given letter and color.
            for( int j = 1; j <= new_width; ++j ) {
                int new_offset = ( i - 1 ) * new_width + ( j - 1 );
                temp[ 2 * new_offset ] = letter;
                temp[ 2 * new_offset + 1 ] = color;
            }

            // If we are not off the end of the old region...
            if( i <= height ) {
                int new_offset = ( i - 1 ) * new_width;
                int old_offset = ( i - 1 ) * width;
                int copy_width = ( new_width < width ) ? new_width : width;
                std::memcpy( temp + ( 2 * new_offset ), buffer + ( 2 * old_offset ), copy_width );
            }
        }

        // Update current object.
        delete [] buffer;
        buffer = temp;
        width  = new_width;
        height = new_height;
    }

}
