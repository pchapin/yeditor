/*! \file BitFile.cpp
 *  \brief Declarations of the bit_file methods.
 *  \author Peter C. Chapin <Peter.Chapin@vtc.vsc.edu>
 *  \date November 11, 2007
 *  \warning This code has not been extensively tested!
 */

#include <cstdio>
#include "BitFile.hpp"

namespace spica {

    //! Opens a bit file.
    /*!
     * It is an error to open a bit file that is already opened without first closing it.
     *
     * \param the_name Pointer to the file name.
     * \param the_mode Either BIT_IN for input files or BIT_OUT for output files.
     *
     * \return Zero if the file failed to open; non-zero otherwise.
     */
    int BitFile::open_bit( char *the_name, BitFileMode the_mode )
    {
        mode             = the_mode;
        bit_number       = 0;
        workspace        = 0;
        workspace_active = 0;

        if( the_mode == BitFileMode::In )
            the_file = std::fopen( the_name, "rb" );
        else
            the_file = std::fopen( the_name, "wb" );

        return ( the_file == NULL ) ? 0 : 1;
    }


    //! Read a header of raw bytes out of a bit file.
    /*!
     * This function reads a specified number of bytes out of a bit file and puts them into the
     * specified buffer. It returns the actual number of bytes read. This function is intended
     * to be used to read a header out of a bit file before processing the bits themselves. It
     * should not be used on a bit_file opened as BIT_OUT. It should not be used once get_bit( )
     * has been called.
     *
     * \param buffer Pointer to buffer to receive the header.
     * \param nmbr_bytes The number of bytes to read.
     *
     * \return The number of bytes actually read. If this is less than the number of bytes
     * requested then either an error occurred or the end of the file was encountered.
     */
    std::size_t BitFile::read_bitheader( void *buffer, std::size_t nmbr_bytes )
    {
        if( the_file == NULL || mode == BitFileMode::Out ) return 0;
        return std::fread( buffer, 1, nmbr_bytes, the_file );
    }


    //! Read a single bit from a bit_file.
    /*!
     * This function returns the next bit from a bit file.
     *
     * \return The next bit or EOF on end-of-file or error.
     */
    int BitFile::get_bit( )
    {
        int result;

        if( the_file == NULL || mode == BitFileMode::Out ) return EOF;

        if( workspace_active == 0 ) {
            workspace        = std::getc( the_file );
            workspace_active = 1;
            bit_number       = 0;
        }
        if( workspace == EOF ) return EOF;
        result = workspace  &  ( 1 << bit_number );
        if( ++bit_number > 7 )
            workspace_active = 0;

        return result != 0;
    }


    //! Write a header into a bit file.
    /*!
     * This function writes a specified number of bytes out of a buffer and puts them into the
     * specified bit file. It returns the actual number of bytes written. This function is
     * intended to be used to write a header into a bit file before processing the bits
     * themselves. It should not be used on BitFile objects opened as BitFileMode::In. It should
     * not be used once put_bit( ) has been called.
     *
     * \param buffer Pointer to the buffer containing the material to write.
     * \param nmbr_bytes The number of bytes to write.
     *
     * \return The number of bytes actually written. If this is less than the number of bytes
     * requested then an error occurred.
     */
    std::size_t BitFile::write_bitheader( void *buffer, std::size_t nmbr_bytes )
    {
        if( the_file == NULL || mode == BitFileMode::In ) return 0;
        return std::fwrite( buffer, 1, nmbr_bytes, the_file );
    }


    //! Write a single bit to a bit file.
    /*!
     * This function writes a bit into a bit file.
     *
     * \param bit_value The bit to write.
     *
     * \return Zero if the write failed, non-zero otherwise.
     */
    int BitFile::put_bit( int bit_value )
    {
        int worked = 1;

        if( the_file == NULL || mode == BitFileMode::In ) return 0;
        
        if( bit_value == 0 )
            workspace &= ~( 1 << bit_number );
        else
            workspace |=  ( 1 << bit_number );

        if( ++bit_number > 7 ) {
            if( std::putc( workspace, the_file ) == EOF ) worked = 0;
            workspace  = 0;
            bit_number = 0;
        }

        return worked;
    }


    //! Close a bit file.
    /*!
     * This function flushes the last partially filled byte to the bit file (for files open as
     * BitFileMode::Out) and closes the file. Bits in the last byte that were not written via
     * put_bit( ) are given the value zero.
     *
     * \param object The bit file to close.
     * \return Zero if the close failed, non-zero otherwise.
     */
    int BitFile::close_bit( )
    {
        int worked = 1;

        if( mode == BitFileMode::In )
            return ( std::fclose( the_file ) == 0 );

        if( bit_number > 0 ) {
            if( std::putc( workspace, the_file ) == EOF ) worked = 0;
        }
        if( std::fclose( the_file ) != 0 ) worked = 0;

        return worked;
    }

}
