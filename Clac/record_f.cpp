/***************************************************************************
FILE          : record_f.cpp
LAST REVISION : 2005-12-22
SUBJECT       : Implementation of a "record file" class.

(C) Copyright 2006 by Peter Chapin and Peter Nikolaidis

--**--
This file contains the implementation of class Record_File. Objects from this
class support easy reading of text files which are organized as several lines
where each line contains a record consisting of several fields. The record
files supported can have a variable number of fields per line and can contain
comments.
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
#include <cstring>

using namespace std;

#include "record_f.h"

static bool Split_String(
  char *Buffer,             /* String to be split.                          */
  char *Delimiters,         /* String of delimiter characters.              */
  char *Strings[],          /* Array to be filled with pntrs to substrings. */
  int   Max_Nmbr_Of_Strings,  /* Maximum number of substrings allowed.      */
  int  *Actual_Nmbr_Of_Strings  /* Actual number of substrings found.       */
  )
  {
    char *Buffer_Pointer;         /* Temporary pointer into buffer.            */
    bool  In_String = false;      /* =false when *buf_ptr not pointing at str. */
    bool  Return_Code = true;     /* Returns false if too many substrings.     */

    /* Initialize result and check for out of bounds input. */
    *Actual_Nmbr_Of_Strings = 0;
    if (Max_Nmbr_Of_Strings < 0) Return_Code = false;
    else {

      /* Sweep down string until null byte reached. */
      Buffer_Pointer = Buffer;
      while (*Buffer_Pointer != '\0') {

        /* Try to process the current character only if no error so far. */
        if (Return_Code == true) {

          /* If delimiter found while scanning an argument... */
          if ((strchr(Delimiters, *Buffer_Pointer) != NULL) &&
              (In_String == true)) {
            *Buffer_Pointer = '\0';
            In_String = false;
          }

          /* If non delimiter found while scanning "white" space... */
          else if ((strchr(Delimiters, *Buffer_Pointer) == NULL) &&
                   (In_String == false)) {

            /* Install only if space. If no space left set error flag. */
            if (*Actual_Nmbr_Of_Strings < Max_Nmbr_Of_Strings) {
              Strings[*Actual_Nmbr_Of_Strings] = Buffer_Pointer;
              (*Actual_Nmbr_Of_Strings)++;
              In_String = true;
            }
            else Return_Code = false;
          }
        }
        Buffer_Pointer++;
      } /* End of while loop. */
    }   /* End of overall if... else... */

    return Return_Code;
  }


/*----------------------------------------------------------------------------
The constructor opens the file and allocates memory for a working buffer. The
search method specified tell the object where the record file might be
found. Currently only DEFAULT is supported; record files must be in the
default directory.

The comment character is the character used to mark the beginning of an
end-of-line style comment in the record file. All text after the first
occurance of this character is ignored. It can be '\0' if there are to be no
comments in the record file.
----------------------------------------------------------------------------*/

Record_File::Record_File(
  char *File_Name,           // File name. Can contain paths, etc.
  int   /* Search_Method*/,  // Where to find file. Must be DEFAULT (for now).
  int   Length,              // Maximum length of line in record file.
  char  Comment,             // Character which starts end-of-line comments.
  char *Delimit)             // String of characters which serve to delimit fields.
  {
    // Initialize important members.
    Buffer        = NULL;
    Nmbr_Of_Parts = 0;

    // Open the file and allocate space.
    The_File.open(File_Name);
    if (!The_File) {
      Is_Ok = false;
    }
    else if ((Buffer = new char[Length + 2]) == NULL) {
      Is_Ok = false;
    }
    else {

      // Save desired specs.
      Delimiters   = Delimit;
      Comment_Char = Comment;
      Line_Length  = Length;
      Is_Ok        = true;
    }
  }

/*----------------------------------------------------------------------------
The destructor closes the file and frees the buffer.
----------------------------------------------------------------------------*/

Record_File::~Record_File()
  {
    if (Buffer != NULL) delete Buffer;
    return;
  }

/*----------------------------------------------------------------------------
The following function returns an array of character pointers. Each pointer in
the array points to a field from the next line of the record file. To determin
the number of fields, see the next function. Comments in the record file and
trailing white space (regardless of the delimiter characters) are already
stripped. The function returns NULL if there is nothing else in the file.
----------------------------------------------------------------------------*/

char **Record_File::Get_Line()
  {
    if (!Is_Ok) return NULL;

    char **Return_Value = NULL;
    char  *End_Pntr;

    // Try to read a line from the file.
    if (The_File.getline(Buffer, Line_Length + 1)) {

      // Kill comments.
      if ((End_Pntr = strchr(Buffer, Comment_Char)) != NULL) *End_Pntr = '\0';

      // Remove trailing white space.
      for (End_Pntr = Buffer; *End_Pntr; End_Pntr++) /* Null */ ;
      for (--End_Pntr;
           End_Pntr >= Buffer && (*End_Pntr == ' ' || *End_Pntr == '\t');
           --End_Pntr) /* Null */ ;
      *++End_Pntr = '\0';

      // Split the string into fields.
      Too_Long = Split_String(Buffer, Delimiters, Parts, 16, &Nmbr_Of_Parts);
      Return_Value = Parts;
    }

    // Return pointer to fields or NULL.
    return Return_Value;
  }

/*----------------------------------------------------------------------------
The following function returns the number of fields in the last line read from
the file.
----------------------------------------------------------------------------*/

int Record_File::Get_Length()
  {
    if (!Is_Ok) return 0;
    return Nmbr_Of_Parts;
  }
