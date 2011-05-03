/**************************************************************************/
/*! 
    @file     bmp.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Loads uncomprssed 24-bit windows bitmaps images

    Based on the information available at:
    http://local.wasp.uwa.edu.au/~pbourke/dataformats/bmp/
    and some sample code written by Michael Sweet
	
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
#include <string.h>

#include "bmp.h"

#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/lcd.h"

// Only include read support if CFG_SDCARD is defined
#ifdef CFG_SDCARD
  #include "drivers/fatfs/diskio.h"
  #include "drivers/fatfs/ff.h"
  static FATFS Fatfs[1];
  #if defined CFG_SDCARD_READONLY && CFG_SDCARD_READONLY == 0
	static FILINFO Finfo;
	static FIL bmpSDFile;
  #endif

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/
bmp_error_t bmpParseBitmap(uint16_t x, uint16_t y, FIL file)
{
  UINT              bytesRead;
  bmp_header_t      header;
  bmp_infoheader_t  infoHeader;

  // Read the file header
  // ToDo: Optimise this to read buffer once and parse it
  f_read(&file, &header.type, sizeof(header.type), &bytesRead);
  f_read(&file, &header.size, sizeof(header.size), &bytesRead);
  f_read(&file, &header.reserved1, sizeof(header.reserved1), &bytesRead);
  f_read(&file, &header.reserved2, sizeof(header.reserved2), &bytesRead);
  f_read(&file, &header.offset, sizeof(header.offset), &bytesRead);

  // Make sure this is a bitmap (first two bytes = 'BM' or 0x4D42 on little-endian systems)
  if (header.type != 0x4D42) return BMP_ERROR_NOTABITMAP;

  // Read the info header
  // ToDo: Optimise this to read buffer once and parse it
  f_read(&file, &infoHeader.size, sizeof(infoHeader.size), &bytesRead);
  f_read(&file, &infoHeader.width, sizeof(infoHeader.width), &bytesRead);
  f_read(&file, &infoHeader.height, sizeof(infoHeader.height), &bytesRead);
  f_read(&file, &infoHeader.planes, sizeof(infoHeader.planes), &bytesRead);
  f_read(&file, &infoHeader.bits, sizeof(infoHeader.bits), &bytesRead);
  f_read(&file, &infoHeader.compression, sizeof(infoHeader.compression), &bytesRead);
  f_read(&file, &infoHeader.imagesize, sizeof(infoHeader.imagesize), &bytesRead);
  f_read(&file, &infoHeader.xresolution, sizeof(infoHeader.xresolution), &bytesRead);
  f_read(&file, &infoHeader.yresolution, sizeof(infoHeader.yresolution), &bytesRead);
  f_read(&file, &infoHeader.ncolours, sizeof(infoHeader.ncolours), &bytesRead);
  f_read(&file, &infoHeader.importantcolours, sizeof(infoHeader.importantcolours), &bytesRead);

  // Make sure that this is a 24-bit image
  if (infoHeader.bits != 24) 
    return BMP_ERROR_INVALIDBITDEPTH;

  // Check image dimensions
  if ((infoHeader.width > lcdGetWidth()) | (infoHeader.height > lcdGetHeight()))
    return BMP_ERROR_INVALIDDIMENSIONS;

  // Make sure image is not compressed
  if (infoHeader.compression != BMP_COMPRESSION_NONE) 
    return BMP_ERROR_COMPRESSEDDATA;

  // Read 24-bit image data
  uint32_t px, py;
  FRESULT res;
  uint8_t buffer[infoHeader.width * 3];
  for (py = infoHeader.height; py > 0; py--)
  {
    // Read one row at a time
    res = f_read(&file, &buffer, infoHeader.width * 3, &bytesRead);
    if (res || bytesRead == 0)
    {
      // Error or EOF
      return BMP_ERROR_PREMATUREEOF;
    }
    for (px = 0; px < infoHeader.width; px++)
    {
      // Render pixel
      // ToDo: This is a brutally slow way of rendering bitmaps ...
      //        update to pass one row of data at a time
      drawPixel(x + px, y + py - 1, drawRGB24toRGB565(buffer[(px * 3) + 2], buffer[(px * 3) + 1], buffer[(px * 3)]));
    }
  }

  return BMP_ERROR_NONE;
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Loads a 24-bit Windows bitmap image from an SD card and
            renders it

    @section Example

    @code 

    #include "drivers/lcd/tft/bmp.h"

    bmp_error_t error;

    // Draw image.bmp (from the root folder) starting at pixel 0,0
    error = bmpDrawBitmap(0, 0, "/image.bmp");

    // Check 'error' for problems such as BMP_ERROR_FILENOTFOUND

    @endcode
*/
/**************************************************************************/
bmp_error_t bmpDrawBitmap(uint16_t x, uint16_t y, const char* filename)
{
  bmp_error_t error = BMP_ERROR_NONE;
  DSTATUS stat;
  BYTE res;

  stat = disk_initialize(0);

  if ((stat & STA_NOINIT) || (stat & STA_NODISK))
  {
    // Card not initialised or no disk present
    return BMP_ERROR_SDINITFAIL;
  }

  if (stat == 0)
  {
    // Try to mount drive
    res = f_mount(0, &Fatfs[0]);
    if (res != FR_OK) 
    {
      // Failed to mount 0:
      return BMP_ERROR_SDINITFAIL;
    }
    if (res == FR_OK)
    {
      // Try to open the requested file
      FIL imgfile;  
      if(f_open(&imgfile, filename, FA_READ | FA_OPEN_EXISTING) != FR_OK) 
      {  
        // Unable to open the requested file
        return BMP_ERROR_FILENOTFOUND;
      }
      // Try to render the specified image
      error = bmpParseBitmap(x, y, imgfile);
      // Close file
      f_close(&imgfile);
      // Unmount drive
      f_mount(0,0);
       // Return results
      return error;
    }
  }

  // Return OK signal
  return BMP_ERROR_NONE;
}

#if defined CFG_SDCARD_READONLY && CFG_SDCARD_READONLY == 0
/**************************************************************************/
/*!
    @brief  Writes the contents of the LCD screen to a 24-bit bitmap
            images.  CFG_SDCARD_READONLY must be set to '0' to be able
            to use this function.

    @section Example

    @code 

    #include "drivers/lcd/tft/bmp.h"

    bmp_error_t error;

    // Note: The LED stays on while the image is being written since
    //       it can take quite a while to read the entire screen
    //       pixel by pixel and write the data to the SD card

    // Turn the LED on to signal busy state
    gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON); 
    // Write the screen contents to a bitmap image
    error = bmpSaveScreenshot("capture.bmp");
    // Turn the LED off to indicate that the capture is complete
    gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF); 

    // Check 'error' for problems

    @endcode
*/
/**************************************************************************/
bmp_error_t bmpSaveScreenshot(const char* filename)
{
  DSTATUS stat;
  bmp_error_t error = BMP_ERROR_NONE;
  bmp_header_t header;
  bmp_infoheader_t infoHeader;
  uint32_t lcdWidth, lcdHeight, x, y, bgra32;
  UINT bytesWritten;
  uint16_t rgb565, eof;
  uint8_t r, g, b;

  // Create a new file (Crossworks only)
  stat = disk_initialize(0);
  if (stat & STA_NOINIT) 
  {
    return BMP_ERROR_SDINITFAIL;
  }
  if (stat & STA_NODISK) 
  {
    return BMP_ERROR_SDINITFAIL;
  }
  if (stat == 0)
  {
    // SD card sucessfully initialised
    DSTATUS stat;
    DWORD p2;
    WORD w1;
    BYTE res, b1;
    DIR dir; 
    // Try to mount drive
    res = f_mount(0, &Fatfs[0]);
    if (res != FR_OK) 
    {
      return BMP_ERROR_SDINITFAIL;
    }
    if (res == FR_OK)
    {
      // Create a file (overwriting any existing file!)
      if(f_open(&bmpSDFile, filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS)!=FR_OK) 
      {  
        return BMP_ERROR_UNABLETOCREATEFILE; 
      }
    }
  }

  lcdWidth = lcdGetWidth();
  lcdHeight = lcdGetHeight();

  // Create header
  header.type = 0x4d42;   // 'BM'
  header.size = (lcdWidth * lcdHeight * 3) + sizeof(header) + sizeof(infoHeader);   // File size in bytes
  header.reserved1 = 0;
  header.reserved2 = 0;
  header.offset = 0x36;   // Offset in bytes to the image data
  
  // Create infoheader
  infoHeader.size = sizeof(infoHeader);
  infoHeader.width = lcdWidth;
  infoHeader.height = lcdHeight;
  infoHeader.planes = 1;
  infoHeader.bits = 24;
  infoHeader.compression = BMP_COMPRESSION_NONE;
  infoHeader.imagesize = (lcdWidth * lcdHeight * 3) + 2;  // 3 bytes per pixel + 2 bytes for EOF
  infoHeader.xresolution = 0x0B12;
  infoHeader.yresolution = 0x0B12;
  infoHeader.ncolours = 0;
  infoHeader.importantcolours = 0;

  // Write header to disk
  f_write(&bmpSDFile, &header.type, sizeof(header.type), &bytesWritten);
  f_write(&bmpSDFile, &header.size, sizeof(header.size), &bytesWritten);
  f_write(&bmpSDFile, &header.reserved1, sizeof(header.reserved1), &bytesWritten);
  f_write(&bmpSDFile, &header.reserved2, sizeof(header.reserved2), &bytesWritten);
  f_write(&bmpSDFile, &header.offset, sizeof(header.offset), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.size, sizeof(infoHeader.size), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.width, sizeof(infoHeader.width), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.height, sizeof(infoHeader.height), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.planes, sizeof(infoHeader.planes), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.bits, sizeof(infoHeader.bits), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.compression, sizeof(infoHeader.compression), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.imagesize, sizeof(infoHeader.imagesize), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.xresolution, sizeof(infoHeader.xresolution), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.yresolution, sizeof(infoHeader.yresolution), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.ncolours, sizeof(infoHeader.ncolours), &bytesWritten);
  f_write(&bmpSDFile, &infoHeader.importantcolours, sizeof(infoHeader.importantcolours), &bytesWritten);

  // Write image data to disk (starting from bottom row)
  for (y = lcdHeight; y != 0; y--)
  {
    for (x = 0; x < lcdWidth; x++)
    {
      rgb565 = lcdGetPixel(x, y - 1);               // Get RGB565 pixel
      bgra32 = drawRGB565toBGRA32(rgb565);          // Convert RGB565 to 24-bit color
      r = (bgra32 & 0x00FF0000) >> 16;
      g = (bgra32 & 0x0000FF00) >> 8;
      b = (bgra32 & 0x000000FF);
      f_write(&bmpSDFile, &b, 1, &bytesWritten);    // Write RGB data
      f_write(&bmpSDFile, &g, 1, &bytesWritten);
      f_write(&bmpSDFile, &r, 1, &bytesWritten);
    }    
  }
  
  // Write EOF (2 bytes)
  eof = 0x0000;
  f_write(&bmpSDFile, &eof, 2, &bytesWritten);

  // Close the file
  f_close(&bmpSDFile);

  // Return OK signal
  return BMP_ERROR_NONE;
}
#endif  // End of read-only check to write bitmaps

#endif  // End of CFG_SDCARD check
