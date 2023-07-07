/*! \file    EditBuffer.cpp
 *  \brief   Implementation of class EditBuffer
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <algorithm>
#include <cstring>
#include "EditBuffer.hpp"

using namespace std;

const int initial_capacity = 8;

//! Find a power of two greater than a given amount.
/*!
 * This function is used to find the necessary capacity to hold a string of the provided size in
 * the buffer. The capacity returned includes extra space for a null byte.
 *
 * \param required The number of non-null charcters that need to be held.
 * \return A power of two (at least 8) that is strictly greater than the requested size.
 */
static size_t round_up( const size_t required )
{
    size_t result = initial_capacity;
    while( result <= required ) {
        result <<= 1;
    }
    return( result );
}

//-------------------------------------------------
//           Constructors and destructor
//-------------------------------------------------

//! Default constructor
/*!
 * Creates an initially empty EditBuffer object.
 *
 * \throws std::bad_alloc if insufficient memory available.
 */
EditBuffer::EditBuffer( ) :
    workspace( new char[initial_capacity] ),
    capacity ( initial_capacity ),
    size     ( 0 )
{
    workspace[0] = '\0';
    return;
}


//! Builds an EditBuffer from a c-style string
/*!
 * Copies the given string into the EditBuffer. If the input parameter is NULL, this constructor
 * behaves identically to the default constructor.
 *
 * \param str Pointer to a null terminated array of characters.
 * \throws std::bad_alloc if insufficient memory available.
 */
EditBuffer::EditBuffer( const char *const str ) :
    workspace( NULL ),
    capacity ( 0 ),
    size     ( 0 )
{
    if( str == NULL ) {
        capacity  = initial_capacity;
        workspace = new char[capacity];
        size      = 0;
        workspace[0] = '\0';
    }
    else {
        const size_t incoming_length = strlen( str );
        capacity = round_up( incoming_length );
        workspace = new char[capacity];
        memcpy( workspace, str, incoming_length + 1 );
        size = incoming_length;
    }
}

//! Copy constructor
/*!
 * The target object is given a capacity related to the length of the string and not necessarily
 * the same capacity as the source object.
 *
 * \param existing The EditBuffer to copy.
 * \throws std::bad_alloc if there is insufficient memory.
 */
EditBuffer::EditBuffer( const EditBuffer &existing ) :
    workspace( NULL ),
    capacity ( 0 ),
    size     ( 0 )
{
    capacity = round_up( existing.size );
    workspace = new char[capacity];
    memcpy( workspace, existing.workspace, existing.size + 1 );
    size = existing.size;
    return;
}

//! Copy assignment operator
/*!
 * This method is similar to a destruction followed by a copy construction except that it is
 * exception safe. The capacity of the target object is adjusted to reflect the length of the
 * incoming string. The target object is left unchanged if an exception occurs.
 *
 * \param existing The source EditBuffer.
 * \throws std::bad_alloc if there is insufficient memory.
 */
EditBuffer &EditBuffer::operator=( const EditBuffer &existing )
{
    if( this != &existing ) {
        const size_t new_capacity = round_up( existing.size );
        char  *const new_workspace = new char[new_capacity];
        delete [] workspace;
        capacity = new_capacity;
        workspace = new_workspace;
        memcpy( workspace, existing.workspace, existing.size + 1 );
        size = existing.size;
    }
    return( *this );
}

#ifdef NEVER
//! Move constructor
EditBuffer::EditBuffer( EditBuffer &&existing) :
    workspace( existing.workspace ),
    capacity ( existing.capacity  ),
    size     ( existing.size      )
{
    existing.workspace = nullptr;
    existing.capacity  = 0;
    existing.size      = 0;
}


//! Move assignment operator
EditBuffer &EditBuffer::operator=( EditBuffer &&existing )
{
    if( this != &existing ) {
        delete [] workspace;
        workspace = existing.workspace;
        capacity  = existing.capacity;
        size      = existing.size;

        existing.workspace = nullptr;
        existing.capacity  = 0;
        existing.size      = 0;
    }
    return( *this );
}
#endif


//-----------------------------------
//           Manipulation
//-----------------------------------

//! Inserts a character into the EditBuffer.
/*!
 * If an attempt is made to insert a character off the end of the buffer, the buffer is extended
 * with spaces. If an exception occurs during the insert operation there is no effect.
 *
 * \param letter The character to insert.
 * \param offset The position in the buffer where the insertion is to take place. The character
 * orginally at this position (and all characters to the right of this position) are moved.
 * \throws std::bad_alloc if there is insufficient memory.
 */
void EditBuffer::insert( const char letter, const std::size_t offset )
{
    // Are we inserting into the existing data?
    if( offset <= size ) {

        // Is there sufficient capacity?
        if( size + 1 < capacity ) {
            memmove( &workspace[offset + 1], &workspace[offset], ( size + 1 ) - offset );
            workspace[offset] = letter;
            ++size;
        }
        // We need to create a new buffer.
        else {
            const size_t new_capacity = round_up( size + 1 );
            char *const new_workspace = new char[new_capacity];
            memcpy( new_workspace, workspace, offset );
            new_workspace[offset] = letter;
            memcpy( &new_workspace[offset + 1], &workspace[offset], ( size + 1 ) - offset );
            delete [] workspace;
            capacity = new_capacity;
            workspace = new_workspace;
            ++size;
        }
    }

    // We are inserting off the end of the buffer.
    else {

        // Is there sufficient capacity? (We need extra space for the null character).
        if( offset + 2 < capacity ) {
            memset( &workspace[size], ' ', offset - size );
            workspace[offset] = letter;
            workspace[offset+1] = '\0';
            size = offset + 1;
        }

        // We need to create a new buffer.
        else {
            const size_t new_capacity = round_up( offset + 1 );
            char *const new_workspace = new char[new_capacity];
            memcpy( new_workspace, workspace, size );
            memset( &new_workspace[size], ' ', offset - size );
            new_workspace[offset] = letter;
            new_workspace[offset+1] = '\0';
            delete [] workspace;
            capacity = new_capacity;
            workspace = new_workspace;
            size = offset + 1;
        }
    }
}


//! Replace the character at the specified offset with the specified character.
/*!
 * This method overwrites the character at the specified position with a new character. If the
 * offset is off the end of the buffer, the buffer is extended with spaces. If an exception
 * occurs during this operation there is no effect.
 *
 * \param letter The new character.
 * \param offset The location in the buffer of where the character should go.
 * \throws std::bad_alloc if there is insufficient memory.
 */
void EditBuffer::replace( const char letter, const std::size_t offset )
{
    if( offset >= size ) insert( letter, offset );
    else {
        workspace[offset] = letter;
    }
}


//! Erase character at a specific position.
/*!
 * It is not an error to attempt to erase a character off the end of the data. In that case
 * there is no effect. This method collapses the data but does not reduce the capacity of the
 * buffer.
 *
 * \param offset The location of the character to erase.
 * \return The character that was erased or the null character if there was no actual data
 * removed.
 */
char EditBuffer::erase( const size_t offset )
{
    char return_value;
    if( offset >= size ) return_value = '\0';
    else {
        return_value = workspace[offset];
        memmove( &workspace[offset], &workspace[offset + 1], size - offset );
        --size;
    }

    return return_value;
}


//! Erases the entire buffer.
/*!
 * Removes the data in the buffer and reinitializes the buffer. This method will thus reduce the
 * capacity of a large buffer. If an exception is thrown during the execution of this method,
 * there is no effect on the original buffer.
 *
 * \throws std::bad_alloc if there is insufficient memory to reinitialize.
 */
void EditBuffer::erase( )
{
    char *const new_workspace = new char[initial_capacity];
    delete [] workspace;
    workspace = new_workspace;
    capacity  = initial_capacity;
    size      = 0;
    workspace[0] = '\0';
}


//! Appends a single character.
/*!
 * If an exception is thrown during the execution of this method, there is no effect on the
 * original object.
 *
 * \param letter The character to append.
 * \throws std::bad_alloc if there is insufficient memory.
 */
void EditBuffer::append( const char letter )
{
    if( size + 1 >= capacity ) {
        const size_t new_capacity = round_up( size + 1 );
        char *const new_workspace = new char[new_capacity];
        memcpy( new_workspace, workspace, size );
        delete [] workspace;
        capacity = new_capacity;
        workspace = new_workspace;
    }
    workspace[size] = letter;
    workspace[size + 1] = '\0';
    ++size;
}


//! Appends a c-style string.
/*!
 * If an exception is thrown during the execution of this method, there is no effect on the
 * original object.
 *
 * \param additional The null terminated c-style string to append. If NULL there is no effect.
 * \throws std::bad_alloc if there is insufficient memory.
 */
void EditBuffer::append( const char *const additional )
{
    if( additional == NULL ) return;
    const size_t additional_size = strlen( additional );

    if( size + additional_size >= capacity ) {
        const size_t new_capacity = round_up( size + additional_size );
        char *const new_workspace = new char[new_capacity];
        memcpy( new_workspace, workspace, size );
        delete [] workspace;
        capacity = new_capacity;
        workspace = new_workspace;
    }
    memcpy( &workspace[size], additional, additional_size + 1 );
    size += additional_size;
}


//! Appends a EditBuffer.
/*!
 * If an exception is thrown during the execution of this method, there is no effect on the
 * original object.
 *
 * \param other The EditBuffer to append.
 * \throws std::bad_alloc if there is insufficient memory.
 */
void EditBuffer::append( const EditBuffer &other )
{
    if( size + other.size >= capacity ) {
        const size_t new_capacity = round_up( size + other.size );
        char *const new_workspace = new char[new_capacity];
        memcpy( new_workspace, workspace, size );
        delete [] workspace;
        capacity = new_capacity;
        workspace = new_workspace;
    }
    memcpy( &workspace[size], other.workspace, other.size + 1 );
    size += other.size;
}


//! Returns a substring of this EditBuffer.
/*!
 * It is not an error for the start_offset and end_offset to be outside the data of this
 * EditBuffer. In that case an EditBuffer containing a suitable number of trailing spaces is
 * returned. If end_offset <= start_offset then an empty EditBuffer is returned.
 *
 * \param start_offset The offset of the first character to be part of the subbuffer.
 * \param end_offset The offset just past the last character that is part of the subbuffer.
 * \returns An EditBuffer containing a copy of the designated text. The source EditBuffer is not
 * changed.
 * \throws std::bad_alloc if there is insufficient memory to copy the designated text.
 */
EditBuffer EditBuffer::subbuffer( const size_t start_offset, const size_t end_offset ) const
{
    EditBuffer result;

    // Only do work if there is work to do.
    if( end_offset > start_offset ) {

        // Copy the designated text to a new workspace. Deal with adding trailing spaces.
        const size_t result_size = end_offset - start_offset;
        const size_t result_capacity = round_up( result_size );
        char *const result_workspace = new char[result_capacity];

        const size_t letters =
            (start_offset < size) ? min(size - start_offset, result_size) : 0;
        const size_t spaces  = result_size - letters;
        memcpy( result_workspace, workspace + start_offset, letters );
        memset( result_workspace + letters, ' ', spaces );
        result_workspace[result_size] = '\0';

        // Replace the guts of the result object.
        delete [] result.workspace;
        result.workspace = result_workspace;
        result.capacity  = result_capacity;
        result.size      = result_size;
    }
    return result;
}


//! Release the tail end of an EditBuffer.
/*!
 * It is not an error to trim an offset that is off the end of the data. In that case, there is
 * no effect. In particular, any excess capacity that might exist is not released (this could be
 * considered a bug).
 *
 * \param offset The offset where the release begins. Space at this offset and beyond is
 * returned to the memory pool.
 * \throws std::bad_alloc if there is insufficient memory to make a fresh copy of the preserved
 * data.
 */
void EditBuffer::trim( const std::size_t offset )
{
    if( offset >= size ) return;

    const size_t new_capacity = round_up( offset );
    char *const new_workspace = new char[new_capacity];
    memcpy( new_workspace, workspace, offset );
    delete [] workspace;
    capacity = new_capacity;
    workspace = new_workspace;
    workspace[offset] = '\0';
    size = offset;
}


//! Return true if the given EditBuffer objects contain the same text.
/*!
 * The full size of the EditBuffers are considered including any real trailing spaces that are
 * currently present. It might be better to do this comparison without regard to trailing spaces.
 */
bool operator==( const EditBuffer &left, const EditBuffer &right )
{
    if( left.length( ) != right.length( ) ) return false;
    for( size_t offset = 0; offset < left.length( ); ++offset ) {
        if( left[offset] != right[offset] ) return false;
    }
    return true;
}
