/******************************************************************************
 * Fast DXT - a realtime DXT compression tool
 *
 * Author : Luc Renambot
 *
 * Copyright (C) 2007 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about SAGE to http://www.evl.uic.edu/cavern/forum/
 *
 *****************************************************************************/

// From:
//    Real-Time DXT Compression
//    May 20th 2006 J.M.P. van Waveren
//    (c) 2006, Id Software, Inc.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

#define C565_5_MASK 0xF8 // 0xFF minus last three bits
#define C565_6_MASK 0xFC // 0xFF minus last two bits

#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )

#if !defined(MAX_INT)
#define       MAX_INT         2147483647      /* max value for an int 32 */
#define       MIN_INT         (-2147483647-1) /* min value for an int 32 */
#endif


#if defined(__GNUC__)
#define   ALIGN16(_x)   _x __attribute((aligned(16)))
#else
#define   ALIGN16( x ) __declspec(align(16)) x
#endif

#define R_DXT5FileSize( width, height ) width * height

// Compress to DXT1 format
void CompressImageDXT1( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes );

// Compress to DXT5 format
void CompressImageDXT5( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes );

// Compress to DXT5 format, first convert to YCoCg color space
void CompressImageDXT5YCoCg( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes );

// Compute error between two images
double ComputeError( const byte *original, const byte *dxt, int width, int height);
