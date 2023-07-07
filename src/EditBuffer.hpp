/*! \file    EditBuffer.hpp
 *  \brief   Interface to class EditBuffer
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef EDITBUFFER_HPP
#define EDITBUFFER_HPP

#include <cstddef>
#include <string>

//! String-like class offering basic editing features.
/*!
 * EditBuffer objects allow the client to perform simple editing operations on strings of text
 * that may be arbitrarily long.
 *
 * It is possible to manipulate the stored text at offsets that are beyond the true end of the
 * text. In that case the object "behaves normally." For example, deleting a large offset does
 * nothing while inserting or replacing at a large offset will extend the text as necessary with
 * spaces. In effect the text in an EditBuffer contains an "infinite" number of trailing spaces
 * that are managed automatically.
 *
 * The interface to this class is designed so implementations could reasonably store the text of
 * an EditBuffer in an external storage area such as a file or on a server. In particular, no
 * references or pointers to the internal state can be obtained via this interface. This design
 * is intentional to allow future flexibility; do not make changes that remove this flexibility
 * without appropriate consideration.
 */
class EditBuffer {
public:
    // Constructors and destructor.
    EditBuffer( );
    EditBuffer( const char * );
    EditBuffer( const EditBuffer & );
    EditBuffer &operator=( const EditBuffer & );
    //EditBuffer( EditBuffer && );
    //EditBuffer &operator=( EditBuffer && );
   ~EditBuffer( );

    // Access.
    char operator[]( std::size_t offset ) const;
    std::size_t length( ) const;
    std::string to_string( ) const;

    // Manipulation.
    void insert( char letter, std::size_t offset );
    void replace( char letter, std::size_t offset );
    char erase( std::size_t offset );
    void erase( );
    void append( char );
    void append( const char * );
    void append( const EditBuffer & );
    EditBuffer subbuffer( std::size_t start_offset, std::size_t end_offset ) const;
    void trim( std::size_t offset );

private:
    char       *workspace; //!< Pointer to buffer data.
    std::size_t capacity;  //!< Size of the raw buffer.
    std::size_t size;      //!< Number of bytes in buffer, not including null.

    // Invariant: If workspace == NULL, capacity and size are zero; otherwise capacity > size.
    // The buffer's contents are null terminated. The capacity must always contain space for
    // the null byte.

    // TODO: Remove the null termination requirement in the invariant.

    // The requirement to keep the buffer contents null terminated is a throw back to when
    // EditBuffer allowed clients to obtain a pointer to the contained text. This requirement
    // meant that such a pointer could be safely passed to functions expecting a C style string.
    // However the current design of EditBuffer no longer provides a way to get such a pointer.
    // Thus the requirement to keep the text internally null terminated is now pointless.

    // TODO: Remove the workspace == NULL condition in the invariant.

    // When Y was first written operator new returned NULL on failure. This lead to the
    // possibility of an EditBuffer with a NULL workspace. However, modern C++ throws an
    // exception if operator new fails. Thus I believe it is no longer possible for a fully
    // constructed EditBuffer to have workspace == NULL. The invariant should be reworded
    // accordingly and the implementation adjusted to take advantage of the modern rules.
};

// ==============
// Inline Methods
// ==============


inline
EditBuffer::~EditBuffer( )
{
    delete [] workspace;
    return;
}


/*!
 * If an out of bounds offset is used, the effect is undefined.
 *
 * \param offset The index of the desired character.
 */
inline
char EditBuffer::operator[]( const std::size_t offset ) const
{
    return( workspace[offset] );
}


/*!
 * This operation completes in constant time.
 */
inline
std::size_t EditBuffer::length( ) const
{
    return( size );
}


/*!
 * Returns a string containing the content of this EditBuffer. The behavior is undefined if the
 * EditBuffer contains more text than a std::string can accommodate.
 *
 * \todo This method probably should be removed at some point. It is likely only being used in
 * places where std::string should be used in the first place (the EditBuffers should probably
 * be removed from those places).
 */
inline
std::string EditBuffer::to_string( ) const
{
    return( std::string( workspace ) );
}


// ==============
// Free Functions
// ==============

bool operator==( const EditBuffer &left, const EditBuffer &right );

inline
bool operator!=( const EditBuffer &left, const EditBuffer &right )
{
    return !( left == right );
}

#endif
