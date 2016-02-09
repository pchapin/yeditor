/*! \file    BitFile.hpp
 *  \brief   A class that allows I/O of a file on a bit-by-bit basis.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef BITFILE_HPP
#define BITFILE_HPP

#include <cstdio>

namespace spica {

    enum class BitFileMode { In, Out };

    //! Class for doing I/O of a file on a bit-by-bit basis.
    /*!
     *  Certain applications need to read or write files on a bit by bit basis. This class
     *  facilitates such activities. The precise order in which the bits are stored into each byte
     *  of the bit file (ie from bit 0 toward bit 7 or the other way around) is not specified in
     *  this interface. However, files written with put_bit( ) will produce the bits in the same
     *  order when read with get_bit( ).
     */
    class BitFile {
    public:
        int         open_bit( char *the_name, BitFileMode the_mode = BitFileMode::In );
        std::size_t read_bitheader( void *buffer, std::size_t number_of_bytes );
        int         get_bit( );
        std::size_t write_bitheader( void *buffer, std::size_t number_of_bytes );
        int         put_bit( int bit_value );
        int         close_bit( );

    private:
        std::FILE *the_file;     //!< The actual file.
        int   bit_number;        //!< The current position in workspace.
        int   workspace;         //!< One partially filled byte.
        int   workspace_active;  //!< =1 if there are interesting bits in workspace.
        BitFileMode mode;        //!< Is the file an input or an output file?
    };

}

#endif

