/*! \file    Entity.hpp
    \brief   Interface to the abstract base class Entity.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

There are 12 entity types all derived from the class defined here. They are binary, complex,
directory, float, integer, labeled, list, matrix, program, rational, string, and vector.
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iosfwd>
#include <stdexcept>
#include <string>

// This is used during serialization to I/O devices (files).
enum EntityType {
    BINARY,  COMPLEX,  DIRECTORY, FLOAT,
    INTEGER, LABELED,  LIST,      MATRIX,
    PROGRAM, RATIONAL, STRING,    VECTOR
};

class Entity {
public:

    /*!
     * The following class defines the exception type thrown by Entity operations when they
     * can't perform properly. Entity operations might also throw std::bad_alloc or
     * std::bad_cast under certain circumstances.
     */
    class Error : public std::runtime_error {
    public:
        explicit Error( const std::string &what_arg ) :
            std::runtime_error( what_arg ) {}
    };

    virtual ~Entity( );

    //! Returns an indication of this entity's runtime type.
    virtual EntityType my_type( ) const = 0;

    //! This function returns a one line representation of the object's value.
    virtual std::string display( ) const = 0;

    //! Return a pointer to a new object with the same actual type and value.
    virtual Entity *duplicate( ) const = 0;

    // Unary operations.
    //
    // These operations are only to be applied to an object allocated on the heap. They return a
    // pointer to result object; the original object is always unchanged. They throw an
    // exception if the result could not be computed.

    virtual Entity *abs              ( ) const;
    virtual Entity *acos             ( ) const;
    virtual Entity *asin             ( ) const;
    virtual Entity *atan             ( ) const;
    virtual Entity *complex_conjugate( ) const;
    virtual Entity *cos              ( ) const;
    virtual Entity *exp              ( ) const;
    virtual Entity *exp10            ( ) const;
    virtual Entity *fractional_part  ( ) const;
    virtual Entity *imaginary_part   ( ) const;
    virtual Entity *integer_part     ( ) const;
    virtual Entity *inv              ( ) const;
    virtual Entity *ln               ( ) const;
    virtual Entity *log              ( ) const;
    virtual Entity *logical_not      ( ) const;
    virtual Entity *neg              ( ) const;
    virtual Entity *real_part        ( ) const;
    virtual Entity *rotate_left      ( ) const;
    virtual Entity *rotate_right     ( ) const;
    virtual Entity *shift_left       ( ) const;
    virtual Entity *shift_right      ( ) const;
    virtual Entity *sign             ( ) const;
    virtual Entity *sin              ( ) const;
    virtual Entity *sq               ( ) const;
    virtual Entity *sqrt             ( ) const;
    virtual Entity *tan              ( ) const;
    virtual Entity *transpose        ( ) const;

    // Conversion functions.
    //
    // These operations are only to be applied to an object allocated on the heap. They return a
    // pointer to result object; the original object is always unchanged. They throw an
    // exception if the result could not be computed.

    virtual Entity *to_binary   ( ) const;
    virtual Entity *to_complex  ( ) const;
    virtual Entity *to_directory( ) const;
    virtual Entity *to_float    ( ) const;
    virtual Entity *to_integer  ( ) const;
    virtual Entity *to_labeled  ( ) const;
    virtual Entity *to_list     ( ) const;
    virtual Entity *to_matrix   ( ) const;
    virtual Entity *to_program  ( ) const;
    virtual Entity *to_rational ( ) const;
    virtual Entity *to_string   ( ) const;
    virtual Entity *to_vector   ( ) const;

    // Binary operations.
    //
    // These operations are only to be applied to objects allocated on the heap. They return a
    // pointer to result object; the original objects are always unchanged. They throw an
    // exception if the result could not be computed.
    //
    // These functions require that the right operand be an Entity* which actually points to an
    // object of the same actual type as *this. If this is not so, an exception is thrown.

    virtual Entity *cross       ( const Entity * ) const;
    virtual Entity *divide      ( const Entity * ) const;
    virtual Entity *dot         ( const Entity * ) const;
    virtual Entity *logical_and ( const Entity * ) const;
    virtual Entity *logical_or  ( const Entity * ) const;
    virtual Entity *logical_xor ( const Entity * ) const;
    virtual Entity *minus       ( const Entity * ) const;
    virtual Entity *modulo      ( const Entity * ) const;
    virtual Entity *multiply    ( const Entity * ) const;
    virtual Entity *plus        ( const Entity * ) const;
    virtual Entity *power       ( const Entity * ) const;

    // Relational operations.
    virtual Entity *is_equal          ( const Entity * ) const;
    virtual Entity *is_notequal       ( const Entity * ) const;
    virtual Entity *is_less           ( const Entity * ) const;
    virtual Entity *is_lessorequal    ( const Entity * ) const;
    virtual Entity *is_greater        ( const Entity * ) const;
    virtual Entity *is_greaterorequal ( const Entity * ) const;

    // File handling operations.
    //
    // These functions allow objects to be written and read from files. The file_size function
    // returns the number of bytes that will be required to write the entity to a file. The
    // write function appends a record on the end of the given binary file. It stores type
    // information as well as value information and throws an exception if it encounters an
    // error. The read function reads the given binary file and returns a pointer to an object
    // of the appropriate type that has been allocated on the heap. It throws an exception if it
    // can't read a proper object. Neither function closes the file, instead they leave the file
    // ready for the next operation.

    virtual long    file_size( ) const;
    virtual void    write( std::ofstream & ) const;
    static  Entity *read( std::ifstream & );
  };

#endif
