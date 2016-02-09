/*! \file    VeryLong.cpp
    \brief   Implementation of an arbitrary precision integer class.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

VeryLong integers are implemented as vectors of "long digits". Each long digit contains several
(generally 16 or 32) bits; the exact number depends on the capacity of the system's primitive
types. The intent of this design is to improve performance by allowing VeryLongs to calculate
several bits at once and to decrease memory requirements by packing as much information as
possible into a primitive type. I/O operations are somewhat inefficient since the conversion
between a large base 10 number and its binary representation is awkward.

This implementation uses a "signed magnitude" representation and not a two's complement
representation. This is significant when individual bits are accessed, but should not be
noticable for other, higher level operations. This implementation ensures that there is no
negative zero. Thus two zero values generated in different ways will compare equal.

TODO:

+ The I/O operations should be modified so that they don't depend on ASCII and they should honor
  formatting flags in the stream objects they are given. Also the handling of white space on
  input should be done by the standard 'eatwhite' manipulator. Support for I/O in bases other
  than 10 should be added.

+ The multiplication code runs in O(n^2) where n is the number of digits in the two numbers.
  This can be improved.

+ The technique used to do division and modulus is a hack I worked up myself. It is probably
  buggy and inefficient. I should probably implement "Algorithm D" in Knuth.

+ The code should be reviewed for thread safety.

+ Support should be added for overloaded bit manipulation operators.

*/

#include <algorithm>
#include <cctype>
#include <iostream>

#include "VeryLong.hpp"

using namespace std;

//-------------------------------------------------------
//           Internally Linked Helper Functions
//-------------------------------------------------------

//
// int is_white( char )
//
// This function defines what "white space" means when doing I/O on VeryLong objects. Actually,
// to insure consistency with the rest of the iostreams library, this issue should be handled
// with the istream manipulator 'eatwhite'.
//
static int is_white( char ch )
{
    if( ch == ' '  || ch == '\t' || ch == '\n' ||
        ch == '\f' || ch == '\r' || ch == '\v'    )
        return 1;

    return 0;
}


//---------------------------------
//           Static Data
//---------------------------------

// The following static constants are public. They exist as a convenience to the user.
const VeryLong VeryLong::negative_one( -1L );
const VeryLong VeryLong::zero( 0L );
const VeryLong VeryLong::one(  1L );
const VeryLong VeryLong::two(  2L );
const VeryLong VeryLong::ten( 10L );


//-------------------------------------
//           Private Methods
//-------------------------------------

//
// void VeryLong::initialize( const char * )
//
// This method does the grunt work of initializing a VeryLong from a string of digit characters.
//
void VeryLong::initialize( const char *digit_string )
{
    int final_sign = +1;

    // First order of business is to make *this a zero. If we find letters in the input string
    // right away, then we will treat that as a zero input.
    //
    digits.resize( 0 );
    sign_flag = +1;

    // Skip over leading white space.
    while( *digit_string && is_white( *digit_string ) ) digit_string++;
    if( *digit_string == '\0' ) return;

    // If we came to a +/- sign, handle it.
    if( *digit_string == '+' || *digit_string == '-' ) {
        final_sign = ( *digit_string == '+' ) ? +1 : -1;
        digit_string++;
        while( *digit_string && is_white( *digit_string ) ) digit_string++;
    }

    // Now process characters as long as we keep getting digits.
    while( *digit_string && isdigit( *digit_string ) ) {

        *this *= ten;
        *this += *digit_string - '0';

        // Get the next character.
        digit_string++;
    }

    // Now apply the proper sign (I couldn't do this earlier, because negative values would get
    // lost in the first multiplication (when *this is zero).
    // 
    sign_flag = final_sign;

    // This handles the case where we got a '-' on the input, but then encountered an
    // end-of-string before we saw any digits.
    //
    if( digits.empty( ) ) {
        sign_flag = +1;
    }
}


//
// void VeryLong::trim_zeros( )
//
// This method removes any leading zero digits from the VeryLong object's internal
// representation. It correctly handles the case when the number is all zero digits.
//
void VeryLong::trim_zeros( )
{
    // If there are no digits, we are done.
    if( digits.empty( ) ) return;

    // Count the number of leading zeros. Note the careful handling of the condition. The type
    // size_type is unsigned so i >= 0 won't do.
    //
    size_type leading_zeros = 0;
    size_type i;
    for( i = digits.size( ) - 1; i > 0; i-- ) {
        if( digits[i] != 0 ) break;
        leading_zeros++;
    }
    if( i == 0 && digits[0] == 0 ) leading_zeros++;

    // Now resize the array of digits.
    digits.resize( digits.size( ) - leading_zeros );

    // If this number is zero, be sure that it is positive zero.
    if( digits.empty( ) ) sign_flag = +1;
}


//
// void VeryLong::shift_left( size_type )
//
// This method shifts the long digits in the VeryLong to the left by the indicated number of
// digit positions. It will shift zero correctly (creating a number with leading zeros).
//
void VeryLong::shift_left( size_type count )
{
    // Resize the number to allow for the new digits.
    size_type old_size = digits.size( );
    digits.resize( digits.size( ) + count );

    // Loop over the existing digits, moving them.
    size_type index;
    size_type loop_counter;
    for( loop_counter = 0, index = old_size - 1;
         loop_counter < old_size;
         loop_counter++, index-- ) {
        digits[index + count] = digits[index];
    }

    // Fill in the "new" places with zeros.
    for( index = 0; index < count; index++ ) {
        digits[index] = 0;
    }
}


//
// bool VeryLong::smaller_than( const VeryLong & ) const
//
// This method returns true if *this has a smaller absolute magnitude than the given object. It
// is used in the comparison function and to order the operands of subtraction.
//
bool VeryLong::smaller_than( const VeryLong &other ) const
{
    // check the number of digits first.
    if( other.digits.size( ) > digits.size( ) ) return true;
    if( other.digits.size( ) < digits.size( ) ) return false;

    // They have the same number of digits.
    size_type index;
    size_type loop_counter;
    for( loop_counter = 0, index = digits.size( ) - 1;
         loop_counter < digits.size( );
         loop_counter++, index-- ) {
        if( other.digits[index] > digits[index] ) return true;
        if( other.digits[index] < digits[index] ) return false;
    }

    // They are equal.
    return false;
}


//-------------------------------------
//           Friend Functions
//-------------------------------------

//
// istream &operator>>( istream &, VeryLong & )
//
// Used to read a VeryLong object from an input stream. This function skips leading white space.
// It then processes all the digits it encounters. It leaves the first non-digit it finds on the
// input stream.
//
istream &operator>>( istream &input, VeryLong &number )
{
    // Initialize to something sensible in case the stream is at EOF right away.
    char ch = ' ';

    string digit_string;

    // Skip over leading white space.
    while( input.get( ch ) && is_white( ch ) ) ;
    if( !input ) return input;

    // If we came to a +/- sign, handle it.
    if( ch == '+' || ch == '-' ) {
        digit_string.push_back( ch );
        while( input.get( ch ) && is_white( ch ) ) ;
    }

    // Now process characters as long as we keep getting digits.
    while( input && isdigit( ch ) ) {
        digit_string.push_back( ch );

        // Get the next character.
        input.get( ch );
    }

    // We got something that wasn't a digit. Put it back.
    if( input ) input.putback( ch );

    // Now install this digit string into number.
    number.initialize( digit_string.c_str( ) );

    return input;
}


//
// ostream &operator<<( ostream &, const VeryLong & )
//
// This function allows a VeryLong object to be printed to an output stream of some kind. Really
// to do this right, I should pay attention to the formatting flags inside the given ostream and
// honor them. This version does not do this.
//
ostream &operator<<( ostream &output, const VeryLong &number )
{
    // Handle zero as a special case.
    if( number.digits.empty( ) ) {
        output << '0';
    }

    // Non-zero numbers get ugly.
    else {
        VeryLong          current( number );
        string             result;
        VeryLong::vldiv_t division_results;

        // If the number is negative, deal with that.
        if( current.sign_flag < 0 ) {
            output << '-';
            current.sign_flag = +1;
        }

        // Keep dividing by 10 until there is no number left. Pack the remainders into a string.
        // The least significant digit is on the left side of the string.
        //
        while( current != VeryLong::zero ) {
            VeryLong::vldiv( current, VeryLong::ten, &division_results );

            // Handle the case of a zero remainder in a special way.
            if( division_results.rem.digits.empty( ) ) {
                result.push_back( '0' );
            }
            else {
                result.push_back( static_cast<char>( division_results.rem.digits[0] + '0' ) );
            }
            current = division_results.quot;
        }

        // Reverse the string (so the least significant digit is to the right) and output the
        // result.
        //
        reverse( result.begin( ), result.end( ) );
        output << result;
    }
    return output;
}


//
// bool operator==( const VeryLong &, const VeryLong & )
//
// This function returns true if both operands have the same value. Otherwise it returns false.
//
bool operator==( const VeryLong &left, const VeryLong &right )
{
    // If they have different signs, they are not equal (no +/- zero).
    if( left.sign_flag != right.sign_flag ) return false;

    // If they have a different number of digits they are not equal (no leading zeros).
    //
    if( left.digits.size( ) != right.digits.size( ) ) return false;

    // They have the same number of digits. Compare them. If both numbers are zero, the loop
    // below will work out.
    //
    for( VeryLong::size_type i = 0; i < left.digits.size( ); i++ ) {
        if( left.digits[i] != right.digits[i] ) return false;
    }

    // If we get here, the numbers were equal.
    return true;
}


//
// bool operator<( const VeryLong &, const VeryLong & )
//
// The following function returns true if the left operand is less than the right operand.
// Otherwise it returns false.
//
bool operator<( const VeryLong &left, const VeryLong &right )
{
    // Deal with sign flags.
    if( left.sign_flag < 0 && right.sign_flag > 0 ) return true;
    if( left.sign_flag > 0 && right.sign_flag < 0 ) return false;

    // The sign flags are the same.
    if( left.sign_flag < 0 ) {
        if( right.smaller_than( left ) ) return true;
    }
    else {
        if( left.smaller_than( right ) ) return true;
    }

    return false;
}


//
// void swap( VeryLong &, VeryLong & )
//
// This swap function makes use of vector's fast swap to implement a fast, O(1), swap for
// VeryLong.
//
void swap( VeryLong &left, VeryLong &right )
{
    std::swap( left.digits, right.digits );
    std::swap( left.sign_flag, right.sign_flag );
}


//------------------------------------
//           Public Methods
//------------------------------------

//
// VeryLong::VeryLong( )
//
// This method sets the number to zero. The digits vector will be constructed as an empty vector
// by default. This is the correct way to represent zero.
//
VeryLong::VeryLong( )
{
    sign_flag = +1;
}


//
// VeryLong::VeryLong( long )
//
// This method initializes a VeryLong from an ordinary integer. It is reasonably efficient.
//
VeryLong::VeryLong( long number )
{
    // Figure out the sign flag and then "normalize" number to a positive value.
    sign_flag = +1;
    if( number < 0 ) {
        sign_flag = -1;
        number = -number;
    }

    // Compute the digits.
    while( number != 0 ) {
        digits.push_back( static_cast< storage_type >( number & DIGIT_MASK ) );
        number >>= BITS_PER_LONGDIGIT;
    }
}


//
// long VeryLong::to_long( ) const
//
// Conversions have to be explicit to prevent surprises. This is especially important
// considering that this method can loose significant digits.
//
long VeryLong::to_long( ) const
{
    // Handle the number zero as a special case.
    if( digits.empty( ) ) return 0;

    return sign_flag * digits[0];
}


//
// VeryLong::size_type VeryLong::number_bits( ) const
//
// The following method returns the number of bits in the number. If the number has a value of
// zero, it has zero bits.
//
VeryLong::size_type VeryLong::number_bits( ) const
{
    // Handle the number zero as a special case.
    if( digits.empty( ) ) return 0;

    // Compute the number of bits.
    size_type count = BITS_PER_LONGDIGIT * ( digits.size( ) - 1 );
    storage_type last_digit = digits[digits.size( ) - 1];

    while( last_digit != 0 ) {
        last_digit >>= 1;
        count++;
    }

    return count;
}


//
// int VeryLong::get_bit( size_type ) const
//
// This method returns the value of the bit at the specified index. The least significant bit is
// bit 0. This method returns either 1 or 0 as appropriate. If the bit does not exist, it
// returns zero. The sign of the VeryLong is not considered.
//
int VeryLong::get_bit( size_type bit_index ) const
{
    size_type digit_number = bit_index / BITS_PER_LONGDIGIT;
    size_type bit_number   = bit_index % BITS_PER_LONGDIGIT;

    if( digit_number >= digits.size( ) ) return 0;
    storage_type digit = digits[digit_number];
    return ( ( digit & ( static_cast< storage_type >( 0x1 ) << bit_number ) ) ? 1 : 0 );
}


//
// void VeryLong::put_bit(size_type, int)
//
// This method installs a new bit value at the specified index. The least significant bit is bit
// 0. If the bit does not exist, the number is extended (with zeros) until it does.
//
void VeryLong::put_bit( size_type bit_index, int new_value )
{
    size_type digit_number = bit_index / BITS_PER_LONGDIGIT;
    size_type bit_number   = bit_index % BITS_PER_LONGDIGIT;

    // If the number isn't big enough, extend it.
    if( digit_number >= digits.size( ) ) {
        size_type old_size = digits.size( );
        digits.resize( digit_number + 1 );
        for( size_type i = old_size; i < digits.size(); i++ ) {
            digits[i] = 0;
        }
    }

    // Now perform the necessary manipulations.
    if( new_value != 0 ) {
        digits[digit_number] |= static_cast< storage_type >(  ( 0x1 << bit_number ) );
    }
    else {
        digits[digit_number] &= static_cast< storage_type >( ~( 0x1 << bit_number ) );
        trim_zeros( );
    }
}


//
// VeryLong VeryLong::operator-( ) const
//
// This method just returns a VeryLong with the opposite sign as the implicit object. It has to
// be careful about zero since zero is always positive.
//
VeryLong VeryLong::operator-( ) const
{
    VeryLong result = *this;
    if( !result.digits.empty( ) ) result.sign_flag = -result.sign_flag;
    return result;
}


//
// VeryLong &VeryLong::operator++( );
// VeryLong &VeryLong::operator--( );
// VeryLong  VeryLong::operator++( int );
// VeryLong  VeryLong::operator--( int );
//
// The following methods implement the usual increment and decrement operators. They are not
// especially efficient and someday could (should) be made so. However, they do get the job done
// with the right semantics so they will be satisfactory for now.
//

VeryLong &VeryLong::operator++( )
{
    *this += 1;
    return *this;
}

VeryLong &VeryLong::operator--( )
{
    *this -= 1;
    return *this;
}

VeryLong VeryLong::operator++( int )
{
    VeryLong old = *this;
    *this += 1;
    return old;
}

VeryLong VeryLong::operator--( int )
{
    VeryLong old = *this;
    *this -= 1;
    return old;
}


//
// void VeryLong::operator+=( const VeryLong & )
//
// Because of the possibility of positive and negative numbers, we have to deal with both the
// addition and subtraction operations here.
//
void VeryLong::operator+=( const VeryLong &other )
{
    // Let's dispense with the easy stuff first.
    if( other.digits.empty(  )) return;
    if( digits.empty( ) ) {
        *this = other;
        return;
    }

    // Neither number is zero...
    vector< storage_type > result;

    // Which number is longer? Figure out the maximum size.
    size_type max_size = digits.size( );
    if( other.digits.size( ) > max_size ) max_size = other.digits.size( );

    // Are we adding or subtracting? If the sign flags are the same we just add the magnitudes.
    if( sign_flag == other.sign_flag ) {

        // Now scan down the numbers adding corresponding digits as we go.
        compute_type carry = 0;
        for( size_type i = 0; i < max_size; i++ ) {
            compute_type sum = carry;

            // Make sure each number has a digit before adding it in. If no digit, use zero.
            if( i < digits.size( ) ) sum += digits[i];
            if( i < other.digits.size( ) ) sum += other.digits[i];

            // Break the sum of two digits into a new digit and a carry.
            result.push_back( static_cast< storage_type >( sum & DIGIT_MASK ) );
            carry = sum >> BITS_PER_LONGDIGIT;
        }
        if( carry ) result.push_back( static_cast< storage_type >( carry ) );

        // Put the computed sum where we want it.
        swap( digits, result );
    }

    // Otherwise we are subtracting.
    else {

        // Point 'small' and 'large' at the objects with the smaller (or larger) absolute
        // magnitude.
        //
        const VeryLong *small;
        const VeryLong *large;
        if( smaller_than( other ) ) {
            small = this;
            large = &other;
        }
        else {
            small = &other;
            large = this;
        }

        // The sign of the result is the sign on the operand with the largest absolute value.
        sign_flag = large->sign_flag;

        // Now do the subtraction of large - small. This will not go negative (although it might
        // be zero if large and small are the same).
        //
        compute_type borrow = 0;
        for( size_type i = 0; i < max_size; i++ ) {
            compute_type next_borrow = 0;

            // Get the digits we will be working with. Handle the possibility that we have used
            // up all of the digits from the small number already.
            //
            compute_type large_digit = large->digits[i];
            compute_type small_digit = 0;
            if( i < small->digits.size( ) ) small_digit = small->digits[i];

            // Do we have to borrow? If so, do it.
            if( large_digit < small_digit + borrow ) {
                large_digit += DIGIT_RANGE;
                next_borrow = 1;
            }

            // Compute and install the new digit.
            compute_type result_digit = large_digit - small_digit - borrow;
            result.push_back( static_cast< storage_type >( result_digit ) );

            // Remember the amount we borrowed for the next loop pass.
            borrow = next_borrow;
        }

        // Install the answer.
        swap( digits, result );
        trim_zeros( );
    }
}


//
// void VeryLong::operator-=( const VeryLong & )
//
// This method implements subtraction by just adding a negative.
//
void VeryLong::operator-=( const VeryLong &other )
{
    // Handle zero special so we don't accidently send a -0 to operator+=().
    if( other.digits.empty( ) ) return;
    if( digits.empty( ) ) {
        *this = other;
        sign_flag = -sign_flag;
        return;
    }

    // Here I'm using the fact that (x - y) is -(-x + y). In this way I avoid having to make a
    // copy of the 'other' object just to negate it.
    //
    sign_flag = -sign_flag;
    *this += other;
    sign_flag = -sign_flag;

    // Protect against creating -0.
    if( digits.empty( ) ) sign_flag = +1;
}


//
// void VeryLong::operator*=( const VeryLong & )
//
// Multiplies two VeryLong integers. This code follows the "classical algorithm" for
// multiplication as described in Knuth's "The Art of Computer Programming, Volume 2:
// Seminumerical Algorithms" (third edition, published by Addison-Wesley, copyright 1998, page
// 268- 270).
//
void VeryLong::operator*=( const VeryLong &other )
{
    // Handle zero cases.
    if( digits.empty( ) ) return;
    if( other.digits.empty( ) ) {
        digits.resize( 0 );
        sign_flag = +1;
        return;
    }

    // Neither number is zero. Compute the result's sign flag.
    sign_flag *= other.sign_flag;

    // Allocate a workspace that is large enough to hold the result.
    size_type m = digits.size( );
    size_type n = other.digits.size( );
    std::vector< storage_type > workspace( m + n, 0 );

    // Do the multiplication.
    for( size_type j = 0; j < n; ++j ) {
        compute_type carry = 0;
        for( size_type i = 0; i < m; ++i ) {
            compute_type temp = carry + 
                static_cast< compute_type >( digits[i] ) * other.digits[j]  + workspace[i + j];
            workspace[i + j] = static_cast< storage_type >( temp & DIGIT_MASK );
            carry = temp >> BITS_PER_LONGDIGIT;
        }
        workspace[j + m] = static_cast< storage_type >( carry );
    }
    
    // Install new digits.
    swap( digits, workspace );
    trim_zeros( );
}


//
// void VeryLong::operator/=( const VeryLong & )
//
// This method computes the quotient after division.
//
void VeryLong::operator/=( const VeryLong &other )
{
    // Let me handle zero as a special case.
    if( digits.empty( ) ) return;

    vldiv_t division_results;
    vldiv( *this, other, &division_results );

    // Put the results where we want them.
    swap( *this, division_results.quot );
}


//
// void VeryLong::operator%=( const VeryLong & )
//
// This method computes the remainder after division.
//
void VeryLong::operator%=( const VeryLong &other )
{
    // Let me handle zero as a special case.
    if( digits.empty( ) ) return;

    vldiv_t division_results;
    vldiv( *this, other, &division_results );

    // Put the results where we want them.
    swap( *this, division_results.rem );
}


//
// void VeryLong::vldiv( const VeryLong &, const VeryLong &, vldiv_t * )
//
// This is where I actually do the grunt work of dividing. I compute both the quotient and
// remainder at the same time and load them into the given vldiv_t object. Cool!
//
void VeryLong::vldiv( const VeryLong &left, const VeryLong &right, vldiv_t *result )
{
    // If they are trying to divide by zero, they are bad people. What we do doesn't matter. The
    // documentation says the effect is "undefined".
    //
    if( right.digits.empty( ) ) return;

    // Set the sign flags.
    result->quot.sign_flag = left.sign_flag / right.sign_flag;
    result->rem.sign_flag  = left.sign_flag;

    // Handle some of the easy special cases.
    if( left.digits.empty( ) ) {
        result->quot.sign_flag = +1;
        result->quot.digits.resize( 0 );
        result->rem.digits.resize( 0 );
        return;
    }
    if( left.smaller_than( right ) ) {
        result->quot.sign_flag = +1;
        result->quot.digits.resize( 0 );
        result->rem.digits = left.digits;
        return;
    }

    // Neither left nor right are zero and abs(left) >= abs(right). Now I have to work.

    VeryLong remainder   = left;   remainder.sign_flag   = +1;
    VeryLong denominator = right;  denominator.sign_flag = +1;
    VeryLong subtractor;
    VeryLong quotient;
    size_type shift_count = remainder.digits.size( ) - denominator.digits.size( );

    // Keep looping until we have our answer.
    while( 1 ) {

        // Create a new digit for the quotient.
        quotient.shift_left( 1 );
        quotient.digits[0] = 0;

        // We have to keep looping, making positive and negative corrections as necessary, until
        // the remainder is positive and yet less than the shifted subtractor.
        //
        while( 1 ) {

            // Line up the subtractor below the remainder.
            subtractor = denominator;
            subtractor.shift_left( shift_count );

            // If we are done thinking about this digit, leave the loop.
            if( remainder < subtractor ) break;

            // The remainder is >= the subtractor.

            // Compute the trial quotient.
            compute_type sub_digit = subtractor.digits[subtractor.digits.size( ) - 1];
            compute_type rem_digit = remainder.digits[subtractor.digits.size( ) - 1];
            if( remainder.digits.size( ) > subtractor.digits.size( ) ) {
                rem_digit +=
                    static_cast< compute_type >( remainder.digits[subtractor.digits.size( )] )
                        << BITS_PER_LONGDIGIT;
            }
            compute_type trial_qdigit = rem_digit / sub_digit;

            // Install the quotient digit.
            quotient.digits[0] += static_cast< storage_type >( trial_qdigit );

            subtractor *= static_cast< long >( trial_qdigit );
            remainder  -= subtractor;

            // If the remainder is negative, our last operation was too much. We need to correct
            // it. Sometimes the trial quotient digit we calculate here will not be enough
            // correction. In that case, the loop will continue to execute. Sometimes it will be
            // too much correction. In that case, the outer loop will continue and another
            // "chunk" will be subtracted from the remainder.
            //
            while( remainder < 0L ) {

                // Line the subtractor up the way it was for the previous operation.
                subtractor = denominator;
                subtractor.shift_left( shift_count );

                // Compute a trial quotient digit.
                compute_type sub_digit = subtractor.digits[subtractor.digits.size( ) - 1];
                compute_type rem_digit = 0;
                if( remainder.digits.size( ) >= subtractor.digits.size( ) )
                    rem_digit = remainder.digits[subtractor.digits.size( ) - 1];
                compute_type trial_qdigit = rem_digit / sub_digit;

                // A small negative remainder still needs correction.
                if( trial_qdigit == 0 ) trial_qdigit = 1;

                // Correct the current quotient digit.
                quotient.digits[0] -= static_cast< storage_type >( trial_qdigit );

                // Scale the subtractor and add it to the remainder to compensate for the
                // previous excess.
                //
                subtractor *= static_cast< long >( trial_qdigit );
                remainder  += subtractor;
            }
        }

        // If this was the last digit, we are done.
        if( shift_count == 0 ) break;

        // We have completely handled this digit.
        shift_count--;
    }

    quotient.trim_zeros( );
    std::swap( result->quot.digits, quotient.digits );
    std::swap( result->rem.digits, remainder.digits );
}
