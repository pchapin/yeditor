/****************************************************************************
FILE          : verylong_test.cpp
LAST REVISED  : 2003-01-07
SUBJECT       : Program to exercise class very_long
PROGRAMMER    : (C) Copyright 2003 by Peter Chapin

This program exercises class very_long. It attempts to do so as
automatically as possible. It also attempts to kill the class. The test
cases in this program that use very large integers were computed using
the Unix program 'dc' which supports arbitrary precision integer
arithmetic. :->

To Do:

+ Automate the tests more. The user should be able to run the program
  non-interactively to verify correctness of the class. (An interactive
  option might be nice too).

LICENSE

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANT-
ABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Please send comments or bug reports pertaining to this file to

     Peter C. Chapin
     Electrical and Computer Engineering Technology Department
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include <iostream>

#include "verylong.h"

using namespace std;
using namespace spica;


//
// void wait()
//
// This function waits for a newline character to be entered at stdin.
//
static void wait()
{
  char ch;
  while (cin.get(ch)) if (ch == '\n') break;
}


//
// void ConstructTest_1()
//
// This function verifies that the default constructor/destructor
//   won't cause a core dump.
//
static void ConstructTest_1()
{
  cout << "Testing default constructor..." << endl;

  very_long object;
  cout << "Default constructor executed." << endl;
}

//
// void IOTest()
//
// This function verifies that we can do basic I/O on very_long. We
//   will need that for other tests.
//
static void IOTest()
{
  very_long object;

  cout << "Testing basic very_long I/O..." << endl;
  cout << "Enter an integer with less than one long digit: " << flush;
  cin  >> object;
  cout << "I got : " << object << endl;
  cout << "Enter the integer zero: " << flush;
  cin  >> object;
  cout << "I got (over the earlier number): " << object << endl;
  cout << "Enter an integer with more than one long digit: " << flush;
  cin  >> object;
  cout << "I got (over the zero): " << object << endl;
  cout << "Enter an integer with leading zeros: " << flush;
  cin  >> object;
  cout << "I got: " << object << endl;
  cout << "Enter a negative integer with less than one long digit: " << flush;
  cin  >> object;
  cout << "I got: " << object << endl;
  cout << "Enter a negative integer with more than one long digit: " << flush;
  cin  >> object;
  cout << "I got: " << object << endl;
  cout << "Enter a negative integer with leading zeros: " << flush;
  cin  >> object;
  cout << "I got: " << object << endl;

  // Throw away the last '\n' on the input stream.
  cin.get();
}


//
// void ConstructTest_2()
//
// This function checks out the other constructors.
//
static void ConstructTest_2()
{
  string raw_digits("123456789");

  cout << "Testing all other constructors..." << endl;

  very_long object_1("12345678900000987654321");
  very_long object_2("0");
  very_long object_3(123456L);
  very_long object_4(0L);
  very_long object_5(-123456L);
  very_long object_6(object_1);
  very_long object_7(object_2);
  very_long object_8(object_5);
  very_long object_9("000000000000001234");
  very_long object_A("-1234");
  very_long object_B("-12345678900000987654321");
  very_long object_C("-0");
  very_long object_D(raw_digits);

  cout << "object_1 = " << object_1 << " (expecting 12345678900000987654321)" << endl;
  cout << "object_2 = " << object_2 << " (expecting 0)" << endl;
  cout << "object_3 = " << object_3 << " (expecting 123456)" << endl;
  cout << "object_4 = " << object_4 << " (expecting 0)" << endl;
  cout << "object_5 = " << object_5 << " (expecting -123456)" << endl;
  cout << "object_5 = " << object_6 << " (expecting 12345678900000987654321)" << endl;
  cout << "object_6 = " << object_7 << " (expecting 0)" << endl;
  cout << "object_8 = " << object_8 << " (expecting -123456)" << endl;
  cout << "object_7 = " << object_9 << " (expecting 1234)" << endl;
  cout << "object_8 = " << object_A << " (expecting -1234)" << endl;
  cout << "object_9 = " << object_B << " (expecting -12345678900000987654321)" << endl;
  cout << "object_A = " << object_C << " (expecting 0)" << endl;
  cout << "object_D = " << object_D << " (expecting 123456789)" << endl;
}


//
// void ComparisonTest()
//
// This function checks out operator==() and operator<().
//
static void ComparisonTest()
{
  cout << "Testing comparison operators..." << endl;

  very_long zero_1(0L);
  very_long zero_2(0L);
  very_long object_1("1234567890987654321");
  very_long object_2("1234567890987654321");
  very_long object_3("5678909876543211234");
  very_long object_4("-1234567890987654321");
  very_long object_5("-1234567890987654321");
  very_long object_6("-5678909876543211234");

  cout << "0 == 0 ";
  if (zero_1 == zero_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "0 == 1234567890987654321 ";
  if (zero_1 == object_1) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "1234567890987654321 == 1234567890987654321 ";
  if (object_1 == object_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 == 5678909876543211234 ";
  if (object_1 == object_3) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "-1234567890987654321 == -1234567890987654321 ";
  if (object_4 == object_5) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1234567890987654321 == 1234567890987654321 ";
  if (object_4 == object_1) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "0 < 0 ";
  if (zero_1 < zero_2) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "0 < 1234567890987654321 ";
  if (zero_1 < object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 < 0 ";
  if (object_1 < zero_1) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "1234567890987654321 < 1234567890987654321 ";
  if (object_1 < object_2) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "1234567890987654321 < 5678909876543211234 ";
  if (object_1 < object_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "5678909876543211234 < 1234567890987654321 ";
  if (object_3 < object_1) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "-1234567890987654321 < 1234567890987654321 ";
  if (object_4 < object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 < -1234567890987654321 ";
  if (object_1 < object_4) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

  cout << "-5678909876543211234 < -1234567890987654321 ";
  if (object_6 < object_4) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1234567890987654321 < -5678909876543211234 ";
  if (object_4 < object_6) cout << "(failed)" << endl;
    else cout << "(passed)" << endl;

}


//
// void AssignmentTest()
//
// This function checks out operator=()
//
static void AssignmentTest()
{
  cout << "Testing assignment operator..." << endl;

  very_long target;
  very_long source_1(0L);
  very_long source_2("1234567890987654321");
  very_long source_3("-1234567890987654321");

  cout << "target = 0 ";
  target = source_1;
  if (target == source_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "target = 1234567890987654321 ";
  target = source_2;
  if (target == source_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "target = -1234567890987654321 ";
  target = source_3;
  if (target == source_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void BitTest();
//
// This function exercises the various bit operations supported.
//
static void BitTest()
{
  cout << "Testing bit operations..." << endl;

  very_long object_1(0L);
  very_long object_2(5L);

  cout << "Checking number of bits in zero ";
  if (object_1.number_bits() == 0) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Checking number of bits in five ";
  if (object_2.number_bits() == 3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Checking specific bits in zero ";
  if (object_1.get_bit( 0) == 0 &&
      object_1.get_bit( 1) == 0 &&
      object_1.get_bit(33) == 0) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Checking specific bits in five ";
  if (object_2.get_bit( 0) == 1 &&
      object_2.get_bit( 1) == 0 &&
      object_2.get_bit( 2) == 1 &&
      object_2.get_bit( 3) == 0 &&
      object_2.get_bit(33) == 0) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Setting specific bits in zero ";
  object_1.put_bit( 0, 1);
  object_1.put_bit(33, 1);
  if (object_1.get_bit( 0) == 1 &&
      object_1.get_bit( 1) == 0 &&
      object_1.get_bit(33) == 1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Setting specific bits in five ";
  object_2.put_bit(0, 0);
  object_2.put_bit(2, 0);
  if (object_2 == very_long::zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void IncDecTest();
//
// This function tests the various operator++ and operator-- functions.
//
static void IncDecTest()
{
  cout << "Testing various increment and decrement operators..." << endl;

  very_long zero(0L);
  very_long object_1(-1L);
  very_long object_2(+1L);
  very_long object_3(-1L);
  very_long object_4(+1L);

  cout << "Testing prefix ++ ";
  if (++object_1 == zero && object_1 == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Testing prefix -- ";
  if (--object_2 == zero && object_2 == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Testing postfix ++ ";
  if (object_3++ == -1 && object_3 == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Testing postfix -- ";
  if (object_4-- == 1 && object_4 == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void UnaryminusTest();
//
// This function tests the operator-() function.
//
static void UnaryminusTest()
{
  cout << "Testing operator-()..." << endl;

  very_long zero(0L);
  very_long object_1("1234567890987654321");
  very_long object_2("-1234567890987654321");

  cout << "Testing 0 ";
  if (zero == -zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Testing 1234567890987654321 ";
  if (object_2 == -object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "Testing -1234567890987654321 ";
  if (object_1 == -object_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void PlusTest()
//
// This function checks out operator+=().
//
static void PlusTest()
{
  cout << "Testing operator+=()..." << endl;

  very_long zero(0L);
  very_long one(1L);
  very_long minus_one(-1L);
  very_long target;
  very_long object_1("1234567890987654321");
  very_long object_2("9999999999999999999");
  very_long object_3("-1234567890987654321");
  very_long object_4("65535");
  very_long object_5("4294967295");
  very_long result_1("2469135781975308642");
  very_long result_2("10000000000000000000");
  very_long result_3("1234567890987654320");
  very_long result_4("-1234567890987654322");
  very_long result_5("65536");
  very_long result_6("4294967296");

  cout << "0 += 0 ";
  target  = zero;
  target += zero;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "0 += 1234567890987654321 ";
  target  = zero;
  target += object_1;
  if (target == object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 += 0 ";
  target  = object_1;
  target += zero;
  if (target == object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 += 1234567890987654321 ";
  target  = object_1;
  target += object_1;
  if (target == result_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "9999999999999999999 += 1 ";
  target  = object_2;
  target += one;
  if (target == result_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 += -1234567890987654321 ";
  target = object_1;
  target += object_3;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 += -1 ";
  target = object_1;
  target += minus_one;
  if (target == result_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1 += 1234567890987654321 ";
  target = minus_one;
  target += object_1;
  if (target == result_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1234567890987654321 += -1 ";
  target = object_3;
  target += minus_one;
  if (target == result_4) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "65535 += 1 ";
  target = object_4;
  target += one;
  if (target == result_5) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "4294967295 += 1 ";
  target = object_5;
  target += one;
  if (target == result_6) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void MinusTest()
//
// This function checks out operator-=().
//
static void MinusTest()
{
  cout << "Testing operator-=()..." << endl;

  very_long zero(0L);
  very_long one(1L);
  very_long minus_one(-1L);
  very_long target;
  very_long object_1("1234567890987654321");
  very_long object_2("9999999999999999999");
  very_long object_3("-1234567890987654321");
  very_long object_4("4294967296");
  very_long object_5("-4294967296");
  very_long result_1("5678909876543211234");
  very_long result_2("10000000000000000000");
  very_long result_3("2469135781975308642");
  very_long result_4("1234567890987654322");
  very_long result_5("-1234567890987654322");
  very_long result_6("-1234567890987654320");
  very_long result_7("4294967295");
  very_long result_8("-4294967295");

  cout << "0 -= 0 ";
  target  = zero;
  target -= zero;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 -= 0 ";
  target  = object_1;
  target -= zero;
  if (target == object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 -= 1234567890987654321 ";
  target  = object_1;
  target -= object_1;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "5678909876543211234 -= 1234567890987654321 ";
  target  = result_1;
  target -= object_1;
  if (target == very_long("4444341985555556913")) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "10000000000000000000 -= 1 ";
  target  = result_2;
  target -= one;
  if (target == object_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 -= -1234567890987654321 ";
  target = object_1;
  target -= object_3;
  if (target == result_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 -= -1 ";
  target = object_1;
  target -= minus_one;
  if (target == result_4) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1 -= 1234567890987654321 ";
  target = minus_one;
  target -= object_1;
  if (target == result_5) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-1234567890987654321 -= -1 ";
  target = object_3;
  target -= minus_one;
  if (target == result_6) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "4294967296 -= 1 ";
  target = object_4;
  target -= one;
  if (target == result_7) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "-4294967295 += 1 ";
  target = object_5;
  target += one;
  if (target == result_8) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void MulTest()
//
// This function checks out operator*=().
//
static void MulTest()
{
  cout << "Testing operator*=()..." << endl;

  very_long zero(0L);
  very_long one(1);
  very_long target;
  very_long object_1("1234567890987654321");
  very_long object_2("5678909876543211234");
  very_long result_1("7010999789392912665121155378475842114");

  cout << "0 *= 0 ";
  target  = zero;
  target *= zero;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 *= 0 ";
  target  = object_1;
  target *= zero;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "0 *= 1234567890987654321 ";
  target  = zero;
  target *= object_1;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 *= 1 ";
  target  = object_1;
  target *= one;
  if (target == object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1 *= 1234567890987654321 ";
  target  = one;
  target *= object_1;
  if (target == object_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 *= 5678909876543211234 ";
  target  = object_1;
  target *= object_2;
  if (target == result_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void DivTest()
//
// This function checks out operator/=().
//
static void DivTest()
{
  cout << "Testing operator/=()..." << endl;

  very_long zero(0L);
  very_long one(1);
  very_long target;
  very_long object_1("1234567890987654321");
  very_long object_2("5678909876543211234");
  very_long object_3("998877665544332211998877665544332211");
  very_long object_4("1234567890987654321234567890987654321");
  very_long object_5("123456789");
  very_long result_1("4");
  very_long result_2("809090915806363692");
  very_long result_3("10000000008000000074800000597");

  cout << "0 /= 1 ";
  target  = zero;
  target /= one;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "0 /= 1234567890987654321 ";
  target  = zero;
  target /= object_1;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1 /= 1234567890987654321 ";
  target  = one;
  target /= object_1;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321 /= 1234567890987654321 ";
  target  = object_1;
  target /= object_1;
  if (target == one) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "5678909876543211234 /= 1234567890987654321 ";
  target  = object_2;
  target /= object_1;
  if (target == result_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "998877665544332211998877665544332211 /= 1234567890987654321 ";
  target  = object_3;
  target /= object_1;
  if (target == result_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321234567890987654321 /= 123456789 ";
  target  = object_4;
  target /= object_5;
  if (target == result_3) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void ModTest()
//
// This function checks out operator%=().
//
static void ModTest()
{
  cout << "Testing operator%=()..." << endl;

  very_long zero(0L);
  very_long one(1);
  very_long target;
  very_long object_1("1234567890987654321");
  very_long object_2("2469135781975308642");
  very_long object_3("5678909876543211234");
  very_long object_4("1234567890987654321234567890987654321");
  very_long object_5("123456789");
  very_long result_1("740638312592593950");
  very_long result_2("83951288");

  cout << "0 %= 1 ";
  target  = zero;
  target %= one;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1 %= 1 ";
  target  = one;
  target %= one;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1 %= 1234567890987654321 ";
  target  = one;
  target %= object_1;
  if (target == one) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "2469135781975308642 %= 1234567890987654321 (divides evenly) ";
  target  = object_2;
  target %= object_1;
  if (target == zero) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "5678909876543211234 %= 1234567890987654321 ";
  target  = object_3;
  target %= object_1;
  if (target == result_1) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;

  cout << "1234567890987654321234567890987654321 %= 123456789 ";
  target  = object_4;
  target %= object_5;
  if (target == result_2) cout << "(passed)" << endl;
    else cout << "(failed)" << endl;
}


//
// void InteractiveDivModTest()
//
// This function checks out operator%=().
//
static void InteractiveDivModTest()
{
  very_long numerator, denominator;

  cout << "Enter numerator and denominator, one on each line. Use (0, 0) to quit." << endl;
  while (1) {
    cin >> numerator;
    cin >> denominator;

    if (numerator == 0L && denominator == 0L) break;

    very_long::vldiv_t division_results;
    very_long::vldiv(numerator, denominator, &division_results);

    cout << "Division yields: " << division_results.quot << endl;
    cout << "Modulus yields:  " << division_results.rem  << endl;
  }
}


//
// Main Program
//
int main()
{
  char line_buffer[256+1];

  cout << "\nverylong_test (v2.0a) June 24, 2001" << endl;
  cout << "  This program tests class very_long.\n" << endl;

  while (1) {
    cout << "0. Exit\n"
            "1. Default constructor.\n"
            "2. I/O operations.\n"
            "3. All other constructors.\n"
            "4. Comparison operations.\n"
            "5. Assignment.\n"
            "6. Bit operations.\n"
            "7. Increment and decrement.\n"
            "8. operator-()\n"
            "9. operator+=()\n"
            "A. operator-=()\n"
            "B. operator*=()\n"
            "C. operator/=()\n"
            "D. operator%=()\n"
            "E. Interactive division and modulus testing.\n";

    cin.getline(line_buffer, 256+1);
    if (line_buffer[0] == '0') break;

    switch (line_buffer[0]) {
      case '1': ConstructTest_1(); break;
      case '2': IOTest();          break;
      case '3': ConstructTest_2(); break;
      case '4': ComparisonTest();  break;
      case '5': AssignmentTest();  break;
      case '6': BitTest();         break;
      case '7': IncDecTest();      break;
      case '8': UnaryminusTest();  break;
      case '9': PlusTest();        break;
      case 'A': MinusTest();       break;
      case 'B': MulTest();         break;
      case 'C': DivTest();         break;
      case 'D': ModTest();         break;
      case 'E': InteractiveDivModTest(); break;
    }

    cout << endl;
    wait();
  }

  return 0;
}

