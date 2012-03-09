/**************************************************************************/
/*! 
    @file     bmp.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#ifndef __BMP_H__
#define __BMP_H__

#include "projectconfig.h"

/**************************************************************************
    Windows Bitmap File Format
    -----------------------------------------------------------------------
    Windows bitmap images are relatively easy to work with because the
    format is basic and requires limited overhead to work with (in
    practice, image data is very rarely compressed).  Bitmap files have
    the following structure:

    --------------------------
    |          Header        |        14 bytes
    |-------------------------
    |        Info Header     |        40 bytes
    -----------------------
    |    Palette (optional)  |
    |-------------------------
    |       Image Data       |
    --------------------------

    For more information on the bitmap file format, see:
    http://local.wasp.uwa.edu.au/~pbourke/dataformats/bmp/

 **************************************************************************/


/**************************************************************************/
/*!
    @brief  14-byte Windows bitmap header
*/
/**************************************************************************/
typedef struct 
{
  uint16_t type;                      /* Magic identifier            */
  uint32_t size;                      /* File size in bytes          */
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;                    /* Offset to image data, bytes */
} bmp_header_t;

/**************************************************************************/
/*!
    @brief  40-byte Windows bitmap info header
*/
/**************************************************************************/
typedef struct 
{
  uint32_t size;                      /* Header size in bytes      */
  int32_t  width;                     /* Width of the image        */
  int32_t  height;                    /* Height of image           */
  uint16_t planes;                    /* Number of colour planes   */
  uint16_t bits;                      /* Bits per pixel            */
  uint32_t compression;               /* Compression type          */
  uint32_t imagesize;                 /* Image size in bytes       */
  int32_t  xresolution;               /* Pixels per meter          */
  int32_t  yresolution;               /* Pixels per meter          */
  uint32_t ncolours;                  /* Number of colours         */
  uint32_t importantcolours;          /* Important colours         */
} bmp_infoheader_t;

/**************************************************************************/
/*!
    @brief  Describes the different compression methods available in
            Windows bitmap images, though compression is not currently
            supported by this code.
*/
/**************************************************************************/
typedef enum
{
  BMP_COMPRESSION_NONE = 0,
  BMP_COMPRESSION_RLE8 = 1,
  BMP_COMPRESSION_RLE4 = 2,
  BMP_COMPRESSION_RGBMASK = 3
} bmp_compression_t;

/**************************************************************************/
/*!
    @brief  24-bit pixel data
*/
/**************************************************************************/
typedef struct
{
  uint8_t rgbBlue;                    /* Blue value */
  uint8_t rgbGreen;                   /* Green value */
  uint8_t rgbRed;                     /* Red value */
} bmp_rgbdata_t;

/**************************************************************************/
/*!
    @brief  Error return codes when processing bitmap images
*/
/**************************************************************************/
typedef enum
{
  BMP_ERROR_NONE = 0,
  BMP_ERROR_SDINITFAIL = 1,
  BMP_ERROR_FILENOTFOUND = 2,
  BMP_ERROR_UNABLETOCREATEFILE = 3,
  BMP_ERROR_NOTABITMAP = 10,          /* First two bytes of the image not 'BM' */
  BMP_ERROR_INVALIDBITDEPTH = 11,     /* Image is not 24-bits */
  BMP_ERROR_COMPRESSEDDATA = 12,      /* Image contains compressed data (not supported) */
  BMP_ERROR_INVALIDDIMENSIONS = 13,   /* Image is > CFG_TFTLCD_WIDTH pixels wide or > CFG_TFTLCD_HEIGHT pixels high */
  BMP_ERROR_PREMATUREEOF = 14         /* EOF reached unexpectedly in pixel data */
} bmp_error_t;

bmp_error_t bmpDrawBitmap(uint16_t x, uint16_t y, const char* filename);

#if defined CFG_SDCARD_READONLY && CFG_SDCARD_READONLY == 0
bmp_error_t bmpSaveScreenshot(const char* filename);
#endif

#endif
