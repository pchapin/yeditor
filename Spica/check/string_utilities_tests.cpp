/**************************************************************************
FILE          : test_strsup.cpp
PROGRAMMER    : Peter Chapin
LAST REVISION : July 16, 2000

(C) Copyright 2000 by Peter Chapin

This file contains a short program to exercise the string_sup functions.

Please send comments or bug reports to

     Peter Chapin
     P.O. Box 317
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
**************************************************************************/

#include <iostream>
#include "string_utilities.hpp"

int string_utilities_tests()
{
  std::string Result;
  std::string Search = "aabab";

  spica::create_long_string(Result, Search, 1024*1024, 2, true);
  spica::display_partial_matches(Result, Search);
  return 0;
}
