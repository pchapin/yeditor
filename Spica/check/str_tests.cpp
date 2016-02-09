/****************************************************************************
FILE          : str_test.cpp
LAST REVISED  : 2006-02-10
SUBJECT       : Test program to exercise class pcc::String.
PROGRAMMER    : (C) Copyright 2006 by Peter Chapin

This program does not do exhaustive testing, but it does allow the user
to exercise the basic functionality of the String class. If all tests
pass, you can be somewhat confident that the class works.


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

#include "environ.hpp"

// I need ctype.h for strupr(). I provide my own strupr() on Unix systems.
#if eOPSYS == ePOSIX
#include <cctype>
#endif

#include <iostream>
#include <cstring>

#include "str.hpp"

using namespace spica;

//--------------------------------------
//           Helper Functions
//--------------------------------------

//
// void wait()
//
// This function waits for the user to type a line (return).
//
static void wait()
{
  char ch;
  while (std::cin.get(ch)) if (ch == '\n') break;
}


//
// void print_header(char *)
//
// This function prints an attractive header at the start of each test.
//
static void print_header(const char *message)
{
  std::cout << "+++++" << std::endl;
  std::cout << message << std::endl;
  std::cout << "+++++" << std::endl;
}


//
// void print_string(char *caption, String &S)
//
// This function prints out a string with delimiter bars so that it will
// be easy to see where the string starts and stops. This function
// assumes that the string operator<<() works.
//
static void print_string(const char *caption, String &S)
{
  std::cout << caption << ": |" << S << "|" << std::endl;
}


#if eOPSYS == ePOSIX
//
// void strupr(char *text)
//
// Unix systems don't seem to have this function in their libraries.
// Really that's rather odd.
//
static void strupr(char *text)
{
  while (*text) {
    *text = std::toupper(*text);
     text++;
  }
}
#endif


//------------------------------------
//           Test Functions
//------------------------------------

//
// void Constructor_Test()
//
// This function constructs a few strings. The destructor is also
// tested. When this function return the strings are destroyed. Although
// there is no check for proper release of memory resources, the lack of
// a core dump would be a good sign that things went well.
//
void Constructor_Test()
{
  print_header("Constructor tests");
  String object_1;
  String object_2("Hello");
  String object_3(object_2);

  print_string("Result of String::String(). Expecting \"\"", object_1);
  print_string(
    "Result of String::String(const char *). Expecting \"Hello\"", object_2);
  print_string(
    "Result of String::String(const String &). Expecting \"Hello\"", object_3);
}


//
// void IO_Test()
//
// This function tests the iostream operations on strings.
//
void IO_Test()
{
  print_header("I/O tests");

  String object_1;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  print_string("I got", object_1);
}


//
// void Append_Test()
//
// This function tests the various append functions.
//
void Append_Test()
{
  print_header("Append tests");

  String junk("Junk");
  String buffer;
  int    counter;
  int    i;

  std::cout
    << "How many times should I concatenate Junk to the buffer? "
    << std::flush;
  std::cin  >> counter;
  std::cin.get();

  // Try appending one string onto another (several times).
  for (i = 0; i < counter; i++) {
    buffer.append(junk);
  }
  print_string("Result of String::append(const String &)", buffer);

  buffer.erase();
  print_string("After erasing", buffer);

  // Now try appending an "old style" string.
  for (i = 0; i < counter; i++) {
    buffer.append("Junk");
  }
  print_string("Result of String::append(const char *)", buffer);

  buffer.erase();
  print_string("After erasing", buffer);

  // And finally append single characters.
  for (i = 0; i < counter; i++) {
    buffer.append('J');
  }
  print_string("Result of String::append(char)", buffer);

  buffer.erase();
  print_string("After erasing", buffer);
}


//
// void Assignment_Test()
//
// This function checks out operator=()
//
void Assignment_Test()
{
  print_header("Assignment tests");

  const  int BUFFER_SIZE = 256;
  String one;
  String two;
  char   line[BUFFER_SIZE + 1];

  std::cout << "Enter a string: " << std::flush;
  std::cin.getline(line, BUFFER_SIZE + 1);
  one = line;
  two = one;
  print_string("After assignment of a char *", one);
  print_string("After assignment of a String", two);
}


//
// void Length_Test()
//
// Can we measure the length of a string normally?
//
void Length_Test()
{
  print_header("Length test");

  String one;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> one;
  std::cout << "Length = " << one.length() << std::endl;
}


//
// void LeftRight_Test()
//
// This function checks out the Left/Right operation.
//
void LeftRight_Test()
{
  print_header("Left/Right test");

  String object_1;
  String object_2;
  int    count;

  std::cout << "Enter a test string: " << std::flush;
  std::cin  >> object_1;
  object_2 = object_1;

  std::cout << "How many? " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 = object_1.left(count, '-');
  print_string("After left(count, '-')", object_1);

  object_2 = object_2.right(count, '-');
  print_string("After right(count, '-')", object_2);
}


//
// void Center_Test()
//
// This function tests the centering feature.
//
void Center_Test()
{
  print_header("Center test");

  String object_1;
  int    width;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a width: " << std::flush;
  std::cin  >> width;
  std::cin.get();

  object_1 = object_1.center(width, '-');
  print_string("Result after center(width, '-')", object_1);
}


//
// void Copy_Test()
//
// This function tests the copy feature.
//
void Copy_Test()
{
  print_header("Copy test");

  String object_1;
  int    count;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a count: " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 = object_1.copy(count);
  print_string("Result after copy(count)", object_1);
}


//
// void Erase_Test()
//
// This function tests the erase substring feature.
//
void Erase_Test()
{
  print_header("Erase test");

  String object_1;
  int    offset;
  int    count;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter an offset: " << std::flush;
  std::cin  >> offset;
  std::cin.get();
  std::cout << "Enter a count: " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 = object_1.erase(offset, count);
  print_string("Result after erase(offset, count)", object_1);
}


//
// void Insert_Test()
//
// This function tests the insert substring feature.
//
void Insert_Test()
{
  print_header("Insert test");

  String object_1;
  String object_2;
  int    offset;
  int    count;

  std::cout << "Enter a string to use as a base: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a string to insert into the base: " << std::flush;
  std::cin  >> object_2;
  std::cout << "Enter an offset: " << std::flush;
  std::cin  >> offset;
  std::cin.get();
  std::cout << "Enter a count: " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 = object_1.insert(object_2, offset, count);
  print_string("Result after insert(object_2, offset, count)", object_1);
}


//
// void Pos_Test()
//
// This function tests the insert pos functions.
//
void Pos_Test()
{
  print_header("Position test");

  String object_1;
  int    offset;
  char   needle_1;
  String needle_2;

  std::cout << "Enter a string to search: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a needle (char): " << std::flush;
  std::cin  >> needle_1;
  std::cin.get();
  std::cout << "Enter a needle (string): " << std::flush;
  std::cin  >> needle_2;
  std::cout << "Enter an offset: " << std::flush;
  std::cin  >> offset;
  std::cin.get();

  int pos = object_1.pos(needle_1, offset);
  std::cout
    << "I found '" << needle_1 << "' at Position: " << pos
    << std::endl;
  int last_pos = object_1.last_pos(needle_1, offset);
  std::cout
    << "I found '" << needle_1 << "' at Last Position: " << last_pos
    << std::endl;
  int string_pos = object_1.pos(needle_2, offset);
  std::cout
    << "I found '" << needle_2 << "' at Position: " << string_pos
    << std::endl;
}


//
// void Strip_Test()
//
// This function tests the strip function.
//
void Strip_Test()
{
  print_header("Strip test");

  String object_1;
  char   kill_char;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a kill char: " << std::flush;
  std::cin  >> kill_char;
  std::cin.get();

  object_1 = object_1.strip('B', kill_char);
  print_string("Result after strip('B', kill_char)", object_1);
}


//
// void Substr_Test()
//
// This function tests the Substr function.
//
void Substr_Test()
{
  print_header("Substr test");

  String object_1;
  int    offset;
  int    count;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter an offset: " << std::flush;
  std::cin  >> offset;
  std::cin.get();
  std::cout << "Enter a count: " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 = object_1.substr(offset, count);
  print_string("Result after substr(offset, count)", object_1);
}


//
// void Words_Test()
//
// This function tests the Words function.
//
void Words_Test()
{
  print_header("Words test");

  String object_1;
  String delimiters;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a delimiter string: " << std::flush;
  std::cin  >> delimiters;

  int count =
    object_1.words(
      delimiters.length() == 0 ? 0 : static_cast<const char *>(delimiters));
  std::cout << "I found " << count << " words in that string." << std::endl;
}


//
// void Subword_Test()
//
// This function tests the Subword function.
//
void Subword_Test()
{
  print_header("Subword test");

  String object_1;
  String delimiters;
  int    offset;
  int    count;

  std::cout << "Enter a string: " << std::flush;
  std::cin  >> object_1;
  std::cout << "Enter a delimiter string: " << std::flush;
  std::cin  >> delimiters;
  std::cout << "Enter an offset: " << std::flush;
  std::cin  >> offset;
  std::cin.get();
  std::cout << "Enter a count: " << std::flush;
  std::cin  >> count;
  std::cin.get();

  object_1 =
    object_1.subword(
      offset, count,
      delimiters.length() == 0 ? 0 : static_cast<const char *>(delimiters));
  print_string("Result after subword(offset, count, delimiters)", object_1);
}


//----------------------------------
//           Main Program
//----------------------------------

bool str_tests()
{
  const int BUFFER_SIZE = 256;
  char line_buffer[BUFFER_SIZE+1];

  std::cout << "str_test: Program to exercise class pcc::String.\n"
            << "(C) Copyright 2001 by Peter Chapin.\n"
            << std::endl;

  while (1) {
    std::cout << "0. Exit\n"
              << "1. Constructor Test\n"
              << "2. I/O Test\n"
              << "3. Append Test\n"
              << "4. Assignment Test\n"
              << "5. Length Test\n"
              << "6. Left/Right Test\n"
              << "7. Center Test\n"
              << "8. Copy Test\n"
              << "9. Erase Test\n"
              << "A. Insert Test\n"
              << "B. Position Test\n"
              << "C. Strip Test\n"
              << "D. Substr Test\n"
              << "E. Words Test\n"
              << "F. Subword Test\n"
              << std::endl;

    std::cin.getline(line_buffer, BUFFER_SIZE+1);
    strupr(line_buffer);
    if (line_buffer[0] == '0') break;

    switch (line_buffer[0]) {
      case '1': Constructor_Test(); break;
      case '2': IO_Test();          break;
      case '3': Append_Test();      break;
      case '4': Assignment_Test();  break;
      case '5': Length_Test();      break;
      case '6': LeftRight_Test();   break;
      case '7': Center_Test();      break;
      case '8': Copy_Test();        break;
      case '9': Erase_Test();       break;
      case 'A': Insert_Test();      break;
      case 'B': Pos_Test();         break;
      case 'C': Strip_Test();       break;
      case 'D': Substr_Test();      break;
      case 'E': Words_Test();       break;
      case 'F': Subword_Test();     break;
    }

    std::cout << std::endl;
    wait();
  }

  return true;
}
