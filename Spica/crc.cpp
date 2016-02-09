/*****************************************************************************
FILE          : crc.c
LAST REVISION : 2004-05-09
SUBJECT       : CRC checksum calculating functions.
PROGRAMMER    : Peter C. Chapin

This file contains a number of CRC calulating functions. These functions
came from the public domain.

I welcome comments and bug reports. I can be reached as in:

        Peter C. Chapin
        Vermont Technical College
        Randolph Center, VT 05061
        pchapin@ecet.vtc.edu
*****************************************************************************/

#include "crc.hpp"

/*
 *   CRC_Clear
 */
unsigned short CRC_Clear()
{
  return 0;
}


/*
 *   CRC_Update
 */
unsigned short CRC_Update(unsigned short CRC, unsigned char CRC_Char)
{
  unsigned long  x;
  unsigned short i;

  /* The variable "x" will contain the character to be processed in bits
     0-7 and the CRC in bits 8-23. Bit 24 will be used to test for
     overflow, and then cleared to prevent the sign bit of "x" from
     being set to 1. Bits 25-31 are not used. ("x" is treated as though
     it is a 32 bit register). */

  /* Get the CRC and the character. */
  x = ((unsigned long)CRC << 8) + CRC_Char;

  /* For each bit in the character... */
  for(i = 0; i < 8; i++) {

    /* Shift the high-order bit of the character into the low-order bit
       of the CRC, and shift the high-order bit of the CRC into bit
       24. */

    x <<= 1;

    /* Test to see if the old high-order bit of the CRC was a 1. */
    if(x & 0x01000000UL)

      /* If the old high-order bit of the CRC was a 1, exclusive-or it
         with a one to set it to 0, and exclusive-or the CRC with hex
         1021 to produce the CCITT-recommended CRC generator of: X**16 +
         X**12 + X**5 + 1. To produce the CRC generator of: X**16 +
         X**15 + X**2 + 1, change the constant from 0x01102100 to
         0x01800500. This will exclusive-or the CRC with hex 8005 and
         produce the same CRC that IBM uses for their synchronous
         transmission protocols. */

      x ^= 0x01102100UL;
  }

  /* Return the CRC we have so far. */
  return (unsigned short)((x & 0x00FFFF00UL) >> 8);
}


/*
 *   CRC_Finish
 */
unsigned short CRC_Finish(unsigned short CRC)
{
  /* Call crc_update twice, passing it a character of hex 00 each time,
     to flush out the last 16 bits from the CRC calculation, and return
     the result as the value of this function. */

  return CRC_Update(CRC_Update(CRC,'\0'),'\0');
}
