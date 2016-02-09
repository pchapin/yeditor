/***************************************************************************
FILE     : test/test_verylong.cpp
SUBJECT  : Unit tests for class very_long.
AUTHOR   : (C) Copyright 2008 by Vermont Technical College

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

TODO

  + ...

Please send comments and bug reports to

     Summer of Software Engineering
     Vermont Technical College
     201 Lawrence Place
     Williston, VT 05495
     sosebugs@summerofsoftware.org (http://www.summerofsoftware.org)
***************************************************************************/
#include <string>
#include <sstream>
#include "test.h"
#include "UnitTest++.h"
#include "../verylong.h"

using namespace std;

/*%
    As it's name implies, the very_long class is a very, very long number.  The VTank game
    itself doesn't have large enough numbers to need this class, but the crypto class utilizes
    it.
*/
SUITE(Very_Long_Tests)
{
    TEST(Constructor)
    {
        // Ensure that the constructor is behaving as expected.
        very_long object;
        CHECK_EQUAL(object.number_bits(), 0u);
        CHECK(object == very_long::zero);
        CHECK(object == 0);
    }

    TEST(IO)
    {
        string raw_digits("123456789");

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

// The helper macro just makes the tests a bit less tedious to type.
#define CHECK_FORMAT(num, result) \
        ostringstream formatter_ ## num; \
        formatter_ ## num << object_ ## num; \
        CHECK(formatter_ ## num.str() == result)

        CHECK_FORMAT(1, "12345678900000987654321");
        CHECK_FORMAT(2, "0");
        CHECK_FORMAT(3, "123456");
        CHECK_FORMAT(4, "0");
        CHECK_FORMAT(5, "-123456");
        CHECK_FORMAT(6, "12345678900000987654321");
        CHECK_FORMAT(7, "0");
        CHECK_FORMAT(8, "-123456");
        CHECK_FORMAT(9, "1234");
        CHECK_FORMAT(A, "-1234");
        CHECK_FORMAT(B, "-12345678900000987654321");
        CHECK_FORMAT(C, "0");
        CHECK_FORMAT(D, "123456789");

        // No input testing is done. That should be fixed eventually.
    }

    TEST(Relational)
    {
        very_long zero_1(0L);
        very_long zero_2(0L);
        very_long object_1( "1234567890987654321");
        very_long object_2( "1234567890987654321");
        very_long object_3( "5678909876543211234");
        very_long object_4("-1234567890987654321");
        very_long object_5("-1234567890987654321");
        very_long object_6("-5678909876543211234");

        CHECK(  zero_1 == zero_2);
        CHECK(!(zero_1 == object_1));
        CHECK(!(zero_1   < zero_2));
        CHECK(  zero_1   < object_1);
        CHECK(!(object_1 < zero_1));

        CHECK(  object_1 == object_2);
        CHECK(!(object_1 == object_3));
        CHECK(  object_4 == object_5);
        CHECK(!(object_4 == object_1));
        CHECK(!(object_1 < object_2));
        CHECK(  object_1 < object_3);
        CHECK(!(object_3 < object_1));
        CHECK(  object_4 < object_1);
        CHECK(!(object_1 < object_4));
        CHECK(  object_6 < object_4);
        CHECK(!(object_4 < object_6));
    }

    TEST(Assignment)
    {
        very_long target;
        very_long source_1(0L);
        very_long source_2( "1234567890987654321");
        very_long source_3("-1234567890987654321");

        target = source_1; CHECK(target == source_1);
        target = source_2; CHECK(target == source_2);
        target = source_3; CHECK(target == source_3);
    }

    TEST(BitManipulation)
    {
        very_long object_1(0L);
        very_long object_2(5L);

        CHECK(object_1.number_bits() == 0);
        CHECK(object_2.number_bits() == 3);

        CHECK(object_1.get_bit( 0) == 0 &&
              object_1.get_bit( 1) == 0 &&
              object_1.get_bit(33) == 0);
        CHECK(object_2.get_bit( 0) == 1 &&
              object_2.get_bit( 1) == 0 &&
              object_2.get_bit( 2) == 1 &&
              object_2.get_bit( 3) == 0 &&
              object_2.get_bit(33) == 0);

        object_1.put_bit( 0, 1);
        object_1.put_bit(33, 1);

        CHECK(object_1.get_bit( 0) == 1 &&
              object_1.get_bit( 1) == 0 &&
              object_1.get_bit(33) == 1);

        object_2.put_bit(0, 0);
        object_2.put_bit(2, 0);
        CHECK(object_2 == very_long::zero);
    }

    TEST(IncDec)
    {
        very_long zero(0L);
        very_long object_1(-1L);
        very_long object_2(+1L);
        very_long object_3(-1L);
        very_long object_4(+1L);

        CHECK(++object_1 == zero && object_1 == zero);
        CHECK(--object_2 == zero && object_2 == zero);
        CHECK(object_3++ == -1 && object_3 == zero);
        CHECK(object_4-- ==  1 && object_4 == zero);
    }

    TEST(UnaryMinus)
    {
        very_long zero(0L);
        very_long object_1("1234567890987654321");
        very_long object_2("-1234567890987654321");

        CHECK(zero == -zero);
        CHECK(object_2 == -object_1);
        CHECK(object_1 == -object_2);
    }

    TEST(Plus)
    {
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

        target = zero;      target += zero;      CHECK(target == zero);
        target = zero;      target += object_1;  CHECK(target == object_1);
        target = object_1;  target += zero;      CHECK(target == object_1);
        target = object_1;  target += object_1;  CHECK(target == result_1);
        target = object_2;  target += one;       CHECK(target == result_2);
        target = object_1;  target += object_3;  CHECK(target == zero);
        target = object_1;  target += minus_one; CHECK(target == result_3);
        target = minus_one; target += object_1;  CHECK(target == result_3);
        target = object_3;  target += minus_one; CHECK(target == result_4);
        target = object_4;  target += one;       CHECK(target == result_5);
        target = object_5;  target += one;       CHECK(target == result_6);
    }

    TEST(Minus)
    {
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

        target = zero;      target -= zero;      CHECK(target == zero);
        target = object_1;  target -= zero;      CHECK(target == object_1);
        target = object_1;  target -= object_1;  CHECK(target == zero);
        target = result_1;  target -= object_1;  CHECK(target == very_long("4444341985555556913"));
        target = result_2;  target -= one;       CHECK(target == object_2);
        target = object_1;  target -= object_3;  CHECK(target == result_3);
        target = object_1;  target -= minus_one; CHECK(target == result_4);
        target = minus_one; target -= object_1;  CHECK(target == result_5);
        target = object_3;  target -= minus_one; CHECK(target == result_6);
        target = object_4;  target -= one;       CHECK(target == result_7);
        target = object_5;  target += one;       CHECK(target == result_8);
    }

    TEST(Multiply)
    {
        very_long zero(0L);
        very_long one(1);
        very_long target;
        very_long object_1("1234567890987654321");
        very_long object_2("5678909876543211234");
        very_long result_1("7010999789392912665121155378475842114");

        target = zero;     target *= zero;     CHECK(target == zero);
        target = object_1; target *= zero;     CHECK(target == zero);
        target = zero;     target *= object_1; CHECK(target == zero);
        target = object_1; target *= one;      CHECK(target == object_1);
        target = one;      target *= object_1; CHECK(target == object_1);
        target = object_1; target *= object_2; CHECK(target == result_1);
    }

    TEST(Divide)
    {
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

        target = zero;     target /= one;      CHECK(target == zero);
        target = zero;     target /= object_1; CHECK(target == zero);
        target = one;      target /= object_1; CHECK(target == zero);
        target = object_1; target /= object_1; CHECK(target == one);
        target = object_2; target /= object_1; CHECK(target == result_1);
        target = object_3; target /= object_1; CHECK(target == result_2);
        target = object_4; target /= object_5; CHECK(target == result_3);
    }

    TEST(Modulus)
    {
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

        target = zero;     target %= one;      CHECK(target == zero);
        target = one;      target %= one;      CHECK(target == zero);
        target = one;      target %= object_1; CHECK(target == one);
        target = object_2; target %= object_1; CHECK(target == zero);
        target = object_3; target %= object_1; CHECK(target == result_1);
        target = object_4; target %= object_5; CHECK(target == result_2);
    }
}

