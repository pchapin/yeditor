/*! \file base64.c
    \brief Base64 encoding functions.
    \author Peter C. Chapin <Peter.Chapin@vtc.vsc.edu>
    \date November 11, 2007
    \warning This code has not been extensively tested!


The functions in this file do base64 encoding and decoding as decribed in RFC-2045. The text
below has been extracted from RFC-2045 and documents the format. This implementation does not
attempt to convert the input to canonical form first (as required by the MIME standard for email
attachments). Such a conversion requires prior knowledge of the MIME type being encoded. It is
up to the caller to resolve that matter before using these functions.

-----> BEGIN excerpt from RFC-2045 <-----

6.8.  Base64 Content-Transfer-Encoding

   The Base64 Content-Transfer-Encoding is designed to represent arbitrary sequences of octets
   in a form that need not be humanly readable. The encoding and decoding algorithms are simple,
   but the encoded data are consistently only about 33 percent larger than the unencoded data.
   This encoding is virtually identical to the one used in Privacy Enhanced Mail (PEM)
   applications, as defined in RFC 1421.

   A 65-character subset of US-ASCII is used, enabling 6 bits to be represented per printable
   character. (The extra 65th character, "=", is used to signify a special processing function.)

   NOTE: This subset has the important property that it is represented identically in all
   versions of ISO 646, including US-ASCII, and all characters in the subset are also
   represented identically in all versions of EBCDIC. Other popular encodings, such as the
   encoding used by the uuencode utility, Macintosh binhex 4.0 [RFC-1741], and the base85
   encoding specified as part of Level 2 PostScript, do not share these properties, and thus do
   not fulfill the portability requirements a binary transport encoding for mail must meet.

   The encoding process represents 24-bit groups of input bits as output strings of 4 encoded
   characters. Proceeding from left to right, a 24-bit input group is formed by concatenating 3
   8bit input groups. These 24 bits are then treated as 4 concatenated 6-bit groups, each of
   which is translated into a single digit in the base64 alphabet. When encoding a bit stream
   via the base64 encoding, the bit stream must be presumed to be ordered with the
   most-significant-bit first. That is, the first bit in the stream will be the high-order bit
   in the first 8bit byte, and the eighth bit will be the low-order bit in the first 8bit byte,
   and so on.

   Each 6-bit group is used as an index into an array of 64 printable characters. The character
   referenced by the index is placed in the output string. These characters, identified in Table
   1, below, are selected so as to be universally representable, and the set excludes characters
   with particular significance to SMTP (e.g., ".", CR, LF) and to the multipart boundary
   delimiters defined in RFC 2046 (e.g., "-").

<pre>
                    Table 1: The Base64 Alphabet

     Value Encoding  Value Encoding  Value Encoding  Value Encoding
         0 A            17 R            34 i            51 z
         1 B            18 S            35 j            52 0
         2 C            19 T            36 k            53 1
         3 D            20 U            37 l            54 2
         4 E            21 V            38 m            55 3
         5 F            22 W            39 n            56 4
         6 G            23 X            40 o            57 5
         7 H            24 Y            41 p            58 6
         8 I            25 Z            42 q            59 7
         9 J            26 a            43 r            60 8
        10 K            27 b            44 s            61 9
        11 L            28 c            45 t            62 +
        12 M            29 d            46 u            63 /
        13 N            30 e            47 v
        14 O            31 f            48 w         (pad) =
        15 P            32 g            49 x
        16 Q            33 h            50 y
</pre>

   The encoded output stream must be represented in lines of no more than 76 characters each.
   All line breaks or other characters not found in Table 1 must be ignored by decoding
   software. In base64 data, characters other than those in Table 1, line breaks, and other
   white space probably indicate a transmission error, about which a warning message or even a
   message rejection might be appropriate under some circumstances.

   Special processing is performed if fewer than 24 bits are available at the end of the data
   being encoded. A full encoding quantum is always completed at the end of a body. When fewer
   than 24 input bits are available in an input group, zero bits are added (on the right) to
   form an integral number of 6-bit groups. Padding at the end of the data is performed using
   the "=" character. Since all base64 input is an integral number of octets, only the following
   cases can arise: (1) the final quantum of encoding input is an integral multiple of 24 bits;
   here, the final unit of encoded output will be an integral multiple of 4 characters with no
   "=" padding, (2) the final quantum of encoding input is exactly 8 bits; here, the final unit
   of encoded output will be two characters followed by two "=" padding characters, or (3) the
   final quantum of encoding input is exactly 16 bits; here, the final unit of encoded output
   will be three characters followed by one "=" padding character.

   Because it is used only for padding at the end of the data, the occurrence of any "="
   characters may be taken as evidence that the end of the data has been reached (without
   truncation in transit). No such assurance is possible, however, when the number of octets
   transmitted was a multiple of three and no "=" characters are present.

   Any characters outside of the base64 alphabet are to be ignored in base64-encoded data.

   Care must be taken to use the proper octets for line breaks if base64 encoding is applied
   directly to text material that has not been converted to canonical form. In particular, text
   line breaks must be converted into CRLF sequences prior to base64 encoding. The important
   thing to note is that this may be done directly by the encoder rather than in a prior
   canonicalization step in some implementations.

   NOTE: There is no need to worry about quoting potential boundary delimiters within
   base64-encoded bodies within multipart entities because no hyphen characters are used in the
   base64 encoding.

-----> END excerpt from RFC-2045 <-----
*/

#include <cstdio>
#include <cstring>
#include "base64.hpp"

static const char code_table[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


//! Base64 encode
/*!
 * This function does a base64 encoding of a previously opened input file and writes the result
 * to a previously opened output file. The files should be both opened in binary mode, however
 * the output file can be opened in append mode if desired. This function does not close either
 * file. There is no error indication.
 *
 * \param infile The previously opened input file to encode.
 * \param outfile The previously opened output file.
 */
void base64_encode( std::FILE *infile, std::FILE *outfile )
{
        std::size_t count;         // Number of bytes read from the input.
             int    line_length;   // Used to count size of each output line.
    unsigned char   in_buffer[3];  // Buffer for the decoded data.
    unsigned char   out_buffer[4]; // Buffer for the encoded data.
    unsigned int    index;         // Index into the code table.

    // Initialize.
    line_length = 0;
    std::memset( in_buffer, 0, 3 );
    std::memset( out_buffer, '=', 4 );

    // Process the input file three bytes at a time.
    while( ( count = std::fread( in_buffer, 1, 3, infile ) ) != 0 ) {

        // Handle the first input byte.
        index  = ( in_buffer[0] & 0xFC ) >> 2;
        out_buffer[0] = code_table[index];
        index  = ( in_buffer[0] & 0x03 ) << 4;
        index |= ( in_buffer[1] & 0xF0 ) >> 4;
        out_buffer[1] = code_table[index];

        if( count > 1 ) {

            // Finish handling the second input byte.
            index  = ( in_buffer[1] & 0x0F ) << 2;
            index |= ( in_buffer[2] & 0xC0 ) >> 6;
            out_buffer[2] = code_table[index];

            if( count > 2 ) {

                // Finish handling the last input byte.
                index = in_buffer[2] & 0x3F;
                out_buffer[3] = code_table[index];
            }
        }

        // Write the encoded data. If it fills out a line, then go to the next.
        std::fwrite( out_buffer, 1, 4, outfile );
        line_length += 4;
        if( line_length >= 72 ) {
            std::fprintf( outfile, "\r\n" );
            line_length = 0;
        }

        // Reset the buffers to their default state.
        std::memset( in_buffer, 0, 3 );
        std::memset( out_buffer, '=', 4 );
    }
}


//! Base64 decode
/*!
 * This function does a base64 decoding of a previously opened input file and writes the result
 * to a previously opened output file. The files should be both opened in binary mode, however
 * the output file can be opened in append mode if desired. This function does not close either
 * file. There is no error indication.
 *
 * \param infile The previously opened input file to decode.
 * \param outfile The previously opened output file.
 */
void base64_decode( std::FILE *, std::FILE * )
{
    std::fprintf( stderr, "base64_decode: Sorry, not implemented!\n" );
}



#ifdef NEVER
// I wrote the following C++ implementation for CourseSmart. It shouldn't be too hard to
// translate it into C.
//

// Helper function for read_string (below).
static bool isBase64( int Ch )
{
    if (std::isalpha(Ch) || std::isdigit(Ch) ||
        Ch == '+' || Ch == '/' || Ch == '=') return true;
    return false;
}

// Helper function for FileSystem::decode (below).
static int read_string(
  unsigned char *buffer, std::string::size_type desired, std::string &contents)
{
    std::string::size_type count = 0;
    std::string::size_type index = 0;

    while (count < desired && index < contents.size()) {
        if (isBase64(contents[index])) {
            *buffer++ = contents[index];
            ++count;
        }
        ++index;
    }
    contents.erase(contents.begin(), contents.begin() + index);
    return count;
}

void FileSystem::decode(const std::string &name, const std::string &contents)
{
    // Creating a temp here is probably not great. What if the file is large? However,
    // read_string modifies the string as it reads it (to avoid having to keep track of any
    // internal state) so I need to copy the constant string given as an argument.
    //
    std::string temp(contents);

    std::string path(normalize_path(name));
    std::FILE *outfile = std::fopen(path.c_str(), "wb");
    if (outfile == NULL) return;

    unsigned char in_buffer[4];
    unsigned char out_buffer[3];
    std::string::size_type count;

    while ((count = read_string(in_buffer, 4, temp)) != 0) {
        // Input base64 is corrupt.
        if (count != 4 || in_buffer[0] == '=' || in_buffer[1] == '=') break;

        const char *p;
        int   value;

        p = std::find(code_table, code_table + sizeof(code_table), in_buffer[0]);
        value = p - code_table;
        out_buffer[0] = value << 2;

        p = std::find(code_table, code_table + sizeof(code_table), in_buffer[1]);
        value = p - code_table;
        out_buffer[0] |= (value & 0x30) >> 4;
        out_buffer[1]  = (value & 0x0F) << 4;
        count = 1;

        if (in_buffer[2] != '=') {

            p = std::find(code_table, code_table + sizeof(code_table), in_buffer[2]);
            value = p - code_table;
            out_buffer[1] |= (value & 0x3C) >> 2;
            out_buffer[2]  = (value & 0x03) << 6;
            count = 2;

            if (in_buffer[3] != '=') {

                p = std::find(code_table, code_table + sizeof(code_table), in_buffer[3]);
                value = p - code_table;
                out_buffer[2] |= value;
                count = 3;
            }
        }
        std::fwrite(out_buffer, sizeof(unsigned char), count, outfile);
    }

    std::fclose(outfile);
}
#endif
