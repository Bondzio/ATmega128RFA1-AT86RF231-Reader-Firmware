/* Copyright (c) 2008  ATMEL Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/*
  $Id: rum_types.h,v 1.1.1.1 2010/03/17 16:16:29 bleverett Exp $
*/
#ifndef RUM_TYPES_H
#define RUM_TYPES_H

#include <stdint.h>

// Make shorter names for standard types; I hate typing ;-)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 uchar;

#if __AVR32__
#define htons_16(x)                             \
    ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define htons_32(x)                                                     \
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |          \
     (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define htons_64(x)                                                     \
    ((((x)   & 0xff00000000000000ull) >> 56)                              \
     | (((x) & 0x00ff000000000000ull) >> 40)                            \
     | (((x) & 0x0000ff0000000000ull) >> 24)                            \
     | (((x) & 0x000000ff00000000ull) >> 8)                             \
     | (((x) & 0x00000000ff000000ull) << 8)                             \
     | (((x) & 0x0000000000ff0000ull) << 24)                            \
     | (((x) & 0x000000000000ff00ull) << 40)                            \
     | (((x) & 0x00000000000000ffull) << 56))
#else // ARM and AVR
#define htons_16(x) (x)
#define htons_32(x) (x)
#define htons_64(x) (x)
#endif

#define ntohs_16(a) htons_16(a)
#define ntohs_32(a) htons_32(a)
#define ntohs_64(a) htons_64(a)

#if __AVR32__
#define __attribute__(packed)
#endif

#endif //RUM_TYPES_H
