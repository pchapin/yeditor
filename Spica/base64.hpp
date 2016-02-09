/****************************************************************************
FILE          : base64.h
LAST REVISION : 2007-11-22
SUBJECT       : Functions to do base64 encoding and decoding.
PROGRAMMER    : (C) Copyright 2007 by Peter C. Chapin

Please send comments or bug reports to

     Peter C. Chapin
     Electrical and Computer Engineering Technology
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#ifndef BASE64_HPP
#define BASE64_HPP

#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

void base64_encode(std::FILE *infile, std::FILE *outfile);
void base64_decode(std::FILE *infile, std::FILE *outfile);

#ifdef __cplusplus
}
#endif

#endif
