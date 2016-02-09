/*****************************************************************************
FILE          : crc.h
LAST REVISION : 2004-05-09
SUBJECT       : CRC checksum calculating functions.
PROGRAMMER    : Peter C. Chapin

This file contains prototypes for CRC related functions. The functions
currently in the package were obtained from the public domain. They
produce a 16 bit (short) CRC checksum using the CCITT polynomial. See
crc.c for more information.

    Peter Chapin
    Vermont Technical College
    Randolph Center, VT 05061
    pchapin@ecet.vtc.edu
*****************************************************************************/

#ifndef CRC_HPP
#define CRC_HPP

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: It is important that the type "unsigned short" be 16 bits and that
         the type "unsigned long" be 32 bits.  */

/*
 *   CRC_Clear:
 *       This function clears the CRC to zero. It should be called prior to
 *       the start of the processing of a block for both received messages,
 *       and messages to be transmitted.
 *
 *       Calling sequence:
 *
 *       unsigned short crc;
 *       crc = CRC_Clear();
 */
unsigned short CRC_Clear(void);


/*
 *   CRC_Update:
 *       this function must be called once for each character which is
 *       to be included in the CRC message transmitted. This function is
 *       called once for each character which is included in the CRC of
 *       a received message, AND once for each of the two CRC at the end
 *       of the received message. If the resulting CRC is zero, then the
 *       message has been correctly received.
 *
 *   Calling sequence:
 *
 *       crc = CRC_Update(crc, Next_Char);
 */
unsigned short CRC_Update(unsigned short crc, unsigned char Next_Char);


/*
 *   CRC_Finish:
 *       This function must be called once after all the characters in a
 *       block have been processed for a message which is to be TRANSMITTED.
 *       It returns the calculated CRC bytes, which should be transmitted
 *       as the two characters following the block. The first of these 2
 *       bytes must be taken from the high-order byte of the CRC, and the
 *       second must be taken from the low-order byte of the CRC. This
 *       routine is NOT called for a message which has been RECEIVED.
 *
 *   Calling sequence:
 *
 *       crc = CRC_Finish(crc);
 */
unsigned short CRC_Finish(unsigned short crc);

#ifdef __cplusplus
}
#endif

#endif

