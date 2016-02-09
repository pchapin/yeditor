/****************************************************************************
FILE          : str_mttest.cpp
LAST REVISED  : 2003-05-18
SUBJECT       : Test program to exercise class pcc::String.
PROGRAMMER    : (C) Copyright 2003 by Peter Chapin

This program exercises the multithreaded support of pcc::String. The
testing done here is certainly not exhaustive but it is a start.
Hopefully it is better than nothing.

REVISION HISTORY

+ 2003-05-18: First official release.


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
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include "environ.hpp"

#include <iostream>

#include <process.h>
#include <windows.h>
#include "str.hpp"

spica::String *A;
spica::String *B;

unsigned __stdcall assigner(void *)
{
  while (1) {
    *A = *B;
  }
  return 0;
}

unsigned __stdcall appender(void *)
{
  while (1) {
    A->append("x");
  }
  return 0;
}

int str_testsMT()
{
  unsigned ass_thread;
  unsigned app_thread;

  A = new spica::String;
  B = new spica::String;
  *B = "Hello!";

  uintptr_t ass_handle =
    _beginthreadex(NULL, 0, assigner, NULL, 0, &ass_thread);
  if (ass_handle == 0) {
    std::cout << "Unable to start the assignment thread.\n";
    return 1;
  }

  uintptr_t app_handle =
    _beginthreadex(NULL, 0, appender, NULL, 0, &app_thread);
  if (app_handle == 0) {
    std::cout << "Unable to start the append thread.\n";
    // Kill the other thread?
    return 1;
  }

  WaitForSingleObject(reinterpret_cast<HANDLE>(ass_handle), INFINITE);
  WaitForSingleObject(reinterpret_cast<HANDLE>(app_handle), INFINITE);

  delete A;
  delete B;
  return 0;
}
