/*! \file    VeryLong.hpp
 *  \brief   Arbitrary precision integer class.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef VERYLONG_HPP
#define VERYLONG_HPP

#include <iosfwd>
#include <string>
#include <vector>

//! Arbitrary precision integers.
/*!
 * This class makes manipulating extended precision integers natural and easy. It is not
 * especially efficient although it uses "reasonable" algorithms (at least that is the intent).
 * The primary goal is to make using extended precision integers as much like using the built in
 * integer types as possible.
 *
 * VeryLong integers have as many digits as required, limited only by available memory. This
 * implementation stores integers using a signed/magnitude representation. It does not use 2's
 * complement for representing negative values. This is significant if you access a VeryLong on
 * a bit by bit basis using get_bit or put_bit. This implementation insures that zero is always
 * positive. Thus zero has only one representation.
 *
 * Before you compile this class, you should review the definitions of storage_type and
 * compute_type in VeryLong.hpp. If you are using a 64 bit machine you can improve performance
 * by redefining these types appropriately. See the comments in VeryLong.hpp for more
 * information. Note that the default settings in VeryLong.hpp will work correctly for 16, 32,
 * and 64 bit machines, even though they are sub-optimal for 64 bit machines.
 *
 * The public type size_type is an unsigned type that is intended to be able to represent the
 * size, in bits, of any VeryLong object. Actually, size_type can only count the VeryLong's
 * underlying storage units. For large values of this count, the number of bits in the VeryLong
 * will overflow size_type. On a 32 bit machine this will occur when the VeryLong has allocated
 * 268,435,456 storage units, consuming 512 MBytes of memory. In that case it will be 2^32 bits
 * long. This is unlikely to be an issue in practice (for example a 512 bit number requires only
 * 32 storage units on a 32 bit machine).
 *
 * The following statements describe the exception safety of this class.
 *
 * <ol>
 *
 * <li><p>If an exception occurs during an operation on one or more VeryLongs, the VeryLong
 * objects that were <B>not</B> being modified by the operation will retain their original value
 * after the exception.</p></li>
 *
 * <li><p>The VeryLong objects that were being modified by an operation that throws an exception
 * enter an unspecified state and should not be used in any further operations of any kind.
 * However, such objects are still destructable and the destructor will correctly recover all
 * resources that were being used by such objects.</p></li>
 *
 * </ol>
 *
 * A stronger and more desirable mode of exception safety would be for an operation to leave
 * <em>all</em> of its operands in the same state they were in before the exception was thrown.
 * VeryLong can almost make that claim, but there are a few operations for which that is not
 * true. Correcting VeryLong so that it would be exception safe in this stronger sense would
 * require making modifications to the class that would likely reduce its performance to an
 * unacceptable degree (in general).
 */

class VeryLong {

    //--------------------------------------
    //           Friend Functions
    //--------------------------------------

    //! Allows a VeryLong object to be written into an ostream.
    /*!
     * Writes the number as a sequence of digit characters with no special formatting. This
     * function currently ignores the settings in the ios part of the given stream. A leading
     * sign is printed only if the number is negative.
     */
    friend std::ostream &operator<<( std::ostream &, const VeryLong & );

    //! Allows a VeryLong object to be read from an istream.
    /*!
     * Leading white space is skipped and a leading '+' or '-' is processed as expected.
     * Conversion stops at the first non-digit character found (which is left on the stream).
     * This function currently ignores the settings on the ios part of the given stream.
     */
    friend std::istream &operator>>( std::istream &, VeryLong & );

    //! Returns true if the two VeryLong objects are equal.
    friend bool operator==( const VeryLong &, const VeryLong & );

    //! Returns true if the first VeryLong is strictly less than the second.
    friend bool operator<( const VeryLong &, const VeryLong & );

    //! Swaps VeryLongs efficiently (O(1)).
    friend void swap( VeryLong &, VeryLong & );

private:

    //-----------------------------------
    //           Private Types
    //-----------------------------------

    //! The type used to hold a single "long" digit.
    typedef unsigned short storage_type;    // Assumed to be 16 bits.

    //! Used for computations on long digits.
    /*! Must have twice as many bits as storage_type. */
    typedef unsigned long compute_type;     // Assumed to be 32 bits.

    //! One more than the largest number used for a long digit.
    static const compute_type DIGIT_RANGE = 65536UL;

    //! Number of bits in a long digit.
    static const int BITS_PER_LONGDIGIT = 16;

    //! Masks to use when computing carry during long digit calculations.
    static const compute_type DIGIT_MASK = 0xFFFF;

    // // These values are more efficient on a 64 bit machine.
    // typedef unsigned int storage_type;      // Assumed to be 32 bits.
    // typedef unsigned long compute_type;     // Assumed to be 64 bits.
    // static const compute_type DIGIT_RANGE = 4294967296UL;
    // static const int BITS_PER_LONGDIGIT = 32;
    // static const compute_type DIGIT_MASK = 0xFFFFFFFF;

public:

    //! Unsigned type to represent the number of long digits and bits.
    typedef std::vector< storage_type >::size_type size_type;

    //------------------------------------
    //           Public Methods
    //------------------------------------

    //! Default constructor creates a VeryLong with a value of zero.
    VeryLong( );

    //! Constructs a VeryLong with the given value.
    VeryLong( long );

    //! Constructs a VeryLong from a string of digit characters.
    /*!
     * For example if the argument is "123456", this constructor would initialize the VeryLong
     * with the value 123,456. The string can contain an arbitrarly large number of digits.
     * Leading white space is skipped and a leading '+' or '-' sign is handled appropriately.
     * Currently only base 10 numbers are supported.
     *
     * This constructor stops at the first non-digit character (including spaces) found in the
     * string. For example: "123a" would initialize the VeryLong to 123. The strings "abc" and
     * "" would both initialize the VeryLong to zero.
     */
    explicit VeryLong( const std::string &digit_string )
      { initialize( digit_string.c_str( ) ); }

    //! Construct a VeryLong from a string of digits.
    explicit VeryLong( const char *digit_string )
      { initialize( digit_string ); }

    //! Returns a long integer version of this VeryLong object.
    /*!
     * If the VeryLong object has a value that is too large to represent in a long, the result
     * is undefined.
     */
    long to_long( ) const;

    //
    // Compiler generated destructor, assignment operator, and copy constructor are appropriate.
    //
    // ~VeryLong( );
    //  VeryLong &operator=( const VeryLong & );
    //  VeryLong( const VeryLong & );
    //

    //! Returns number of bits in the current number.
    /*!
     * Leading zeros are not counted. Thus the value 0 has zero bits.
     */
    size_type number_bits( ) const;

    //! Returns the bit at the indicated position.
    /*!
     * The least significant bit is bit 0. Non-existent bits are zero.
     **/
    int get_bit( size_type ) const;

    //! Sets the bit specified by the first parameter to 1 or 0.
    /*!
     * If the bit does not exist, the number is extended so that it does.
     */
    void put_bit( size_type, int );

    //! Returns the negative of the current object.
    VeryLong operator-( ) const;

    //! Prefix increment.
    VeryLong &operator++( );

    //! Prefix decrement.
    VeryLong &operator--( );

    //! Postfix increment.
    VeryLong  operator++( int );

    //! Postfix decrement.
    VeryLong  operator--( int );

    //! Adds the given VeryLong to the current object.
    void operator+=( const VeryLong & );

    //! Subtracts the give VeryLong from the current object.
    void operator-=( const VeryLong & );

    //! Multiplies the given VeryLong into the current object.
    void operator*=( const VeryLong & );

    //! Divides the current object by the given VeryLong.
    /*!
     * This method leaves the quotient in the current object. The result is always truncated
     * toward zero. Division by zero is undefined.
     */
    void operator/=( const VeryLong & );

    //! Divides the current object by the given VeryLong.
    /*!
     * This method leaves the remainder in the current object. The sign of the remainder is the
     * sign on *this (the left operand). Division by zero is undefined.
     */
    void operator%=( const VeryLong & );

    //------------------------------------
    //           Static Methods
    //------------------------------------

    struct vldiv_t;

    //! Divides two VeryLong objects and gets both quotient and remainder.
    /*!
     * This is faster than doing the divide and modulus operations separately in cases where
     * both results are needed. The function acts like the standard library functions with
     * similar names.
     *
     * In order to avoid copying VeryLongs unnecessarily this function takes a pointer to the
     * place where the results should be put. The quotient always truncates toward zero and has
     * the sign of the result. The remainder has the sign of the left operand. This is so a =
     * (a / b) * b + a % b for all values of 'a' and 'b.' Division by zero is undefined.
     */
    static void vldiv( const VeryLong &, const VeryLong &, vldiv_t * );

    //----------------------------------------
    //           Public Static Data
    //----------------------------------------

    //! A preconstructed VeryLong -1.
    static const VeryLong negative_one;

    //! A preconstructed VeryLong 0.
    static const VeryLong zero;

    //! A preconstructed VeryLong 1.
    static const VeryLong one;

    //! A preconstructed VeryLong 2.
    static const VeryLong two;

    //! A preconstructed VeryLong 10.
    static const VeryLong ten;

private:

    //----------------------------------
    //           Private Data
    //----------------------------------

    // Holds the long digits in memory with the LSD first. There are no leading zero digits
    // stored in memory. The value zero is represented by an empty Digits vector.
    // 
    std::vector<storage_type> digits;

    // Set to +/- 1 to indicate the sign of the number. Zero is always positive.
    // 
    int sign_flag;

    //-------------------------------------
    //           Private Methods
    //-------------------------------------

    // Initializes a VeryLong from the null terminated string of digit characters. Leading white
    // space is skipped and a leading + or - sign is honored. Only base 10 is (currently)
    // supported.
    // 
    void initialize( const char *digit_string );

    // Erase the leading zeros in *this. This is used to ensure that any leading zeros given or
    // computed are removed.
    // 
    void trim_zeros( );

    // Shifts *this left the given number of long digits.
    void shift_left( size_type count );

    // Compares the absolute magnitude of *this with that of the given object. Returns true if
    // *this has a smaller magnitude.
    // 
    bool smaller_than( const VeryLong & ) const;
};


//! Used by the VeryLong::vldiv function.
//
// It is defined here because it contains VeryLong objects and the compiler rejects attempts to
// define it that way inside class VeryLong.
//
struct VeryLong::vldiv_t {
    VeryLong quot;
    VeryLong rem;
};


//------------------------------------------------
//           Inline Non-Member Functions
//------------------------------------------------

//! Returns true if the two arguments are unequal.
inline bool operator!=( const VeryLong &left, const VeryLong &right )
    { return !( left == right ); }

//! Returns true if the left argument is greater than the right.
inline bool operator>( const VeryLong &left, const VeryLong &right )
    { return right < left; }

//! Returns true if the left argument is greater than or equal to the right.
inline bool operator>=( const VeryLong &left, const VeryLong &right )
    { return !( left < right ); }

//! Returns true if the left argument is less than or equal to the right.
inline bool operator<=( const VeryLong &left, const VeryLong &right )
    { return !( left > right ); }


//! Infix add returns the sum of two VeryLongs.
inline VeryLong operator+( const VeryLong &left, const VeryLong &right )
    { VeryLong temp( left ); temp += right; return temp; }

//! Infix subtract returns the difference of two VeryLongs.
inline VeryLong operator-( const VeryLong &left, const VeryLong &right )
    { VeryLong temp( left ); temp -= right; return temp; }

//! Infix multiply returns the produce of two VeryLongs.
inline VeryLong operator*( const VeryLong &left, const VeryLong &right )
    { VeryLong temp( left ); temp *= right; return temp; }

//! Infix divide returns the quotient of two VeryLongs.
inline VeryLong operator/( const VeryLong &left, const VeryLong &right )
    { VeryLong temp( left ); temp /= right; return temp; }

//! Infix modulus returns the remainder after division of two VeryLongs.
inline VeryLong operator%( const VeryLong &left, const VeryLong &right )
    { VeryLong temp( left ); temp %= right; return temp; }

#endif
