/****************************************************************************
FILE          : fvector_test.cpp
LAST REVISED  : June 24, 2001
SUBJECT       : Test program to exercise fvectors.
PROGRAMMER    : (C) Copyright 2001 by Peter Chapin

{Comment}


REVISION HISTORY

+ Oct 28, 2000: First official release.


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

     Peter Chapin
     P.O. Box 317
     Randolph Center, VT 05061
     pchapin@sover.net
****************************************************************************/

#include "environ.hpp"

#include <algorithm>
#include <iostream>
#include "FileVector.hpp"

//
// Function to test operator[]()
//
static void access_test()
{
  int i;

  std::cout << "Testing operator[]()...";
  spica::FileVector<int> my_file("test.dat", 1024);
  for (i = 0; i < 1024; ++i) {
    my_file[i] = i;
  }

  for (i = 0; i < 1024; ++i) {
    if (my_file[i] != i) {
      std::cout << "FAILED!\n";
      break;
    }
  }
  if (i == 1024) std::cout << "passed\n";
}


//
// Function to test push_back()
//
static void push_back_test()
{
  int i;

  std::cout << "Testing push_back()...";
  spica::FileVector<int> my_file("test.dat");
  if (my_file.size() != 1024) {
    std::cout << "FAILED! size() not as expected!\n";
    return;
  }

  for (i = 0; i < 1024; ++i) {
    my_file.push_back(2*i);
  }

  for (i = 1024; i < 2048; ++i) {
    if (my_file[i] != 2*(i - 1024)) {
      std::cout << "FAILED!\n";
      break;
    }
  }
  if (i == 2048) std::cout << "passed\n";
}


int main(int, char **)
{
  try {
    access_test();
    push_back_test();
  }
  catch(std::bad_alloc) {
    std::cout << "Out of memory!" << "\n";
  }
  return 0;
}
