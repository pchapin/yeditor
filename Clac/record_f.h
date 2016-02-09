/***************************************************************************
FILE          : record_f.h
LAST REVISION : 2005-12-22
SUBJECT       : Interface to a "record file" class.

(C) Copyright 2006 by Peter Chapin and Peter Nikolaidis

--**--
[Put comments here]
--**--

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

Please send comments and bug reports to

     Peter C. Chapin
     c/o Electrical and Computer Engineering Technology Department
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
***************************************************************************/
#ifndef RECORD_F_H
#define RECORD_F_H

#include <fstream>

class Record_File {

  public:
    Record_File(            // Opens file and allocates space for buffer.
      char *File_Name,
      int   Search_Method,
      int   Length,
      char  Comment,
      char *Delimit);
   ~Record_File();         // Closes file and frees buffer.

    char  **Get_Line();    // Reads a line and breaks it into fields.
    int     Get_Length();  // Returns the number of fields.
    bool    Is_Ok;         // =true if constructor works.
    bool    Too_Long;      // =false if too many fields in line.

    enum { DEFAULT };      // Search methods.

  private:
    std::ifstream The_File;   // Refers to actual file.
    char     Comment_Char;    // Defines comment character in file.
    int      Line_Length;     // Longest allowed line length in file.
    char    *Buffer;          // Points at buffer to hold each raw line.
    char    *Delimiters;      // String of delimiters for fields in the file.
    char    *Parts[16];       // Points at each field (note fixed size).
    int      Nmbr_Of_Parts;   // Number of fields.

  };

#endif
