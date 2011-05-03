/**************************************************************************/
/*! 
    @file     cmd_sd_dir.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_sd_dir in the 'core/cmd'
              command-line interpretter.

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
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "core/cmd/cmd.h"
#include "project/commands.h"           // Generic helper functions

#ifdef CFG_SDCARD
  #include "core/timer32/timer32.h"
  #include "core/ssp/ssp.h"
  #include "drivers/fatfs/diskio.h"
  #include "drivers/fatfs/ff.h"

  static FILINFO Finfo;
  static FATFS Fatfs[1];

/**************************************************************************/
/*! 
    sd 'dir' command handler

    This demonstrates how to initialise the SD card, read the contents
    of a directory (including checking if an entry is a folder or a file),
    and checking the amount of freespace on available on the disk.
*/
/**************************************************************************/
void cmd_sd_dir(uint8_t argc, char **argv)
{
  char* path;

  // Display root folder by default
  path = argc > 0 ? argv[0] : "/";

  // Initialise SD Card
  DSTATUS stat;
  stat = disk_initialize(0);
  if (stat & STA_NOINIT) 
  {
    printf("SD init failed%s", CFG_PRINTF_NEWLINE);
  }
  if (stat & STA_NODISK) 
  {
    printf("No SD card%s", CFG_PRINTF_NEWLINE);
  }
  if (stat == 0)
  {
    BYTE res;
    DIR dir;

    // Try to mount drive
    res = f_mount(0, &Fatfs[0]);
    if (res != FR_OK) 
    {
      printf("Failed to mount partition%s" , CFG_PRINTF_NEWLINE);
    }
    if (res == FR_OK)
    {
      res = f_opendir(&dir, path);
      if (res) 
      {
          printf("Failed to open '%s' %s", path, CFG_PRINTF_NEWLINE);
          return;
      }

      // Display directory name
      printf("%s Contents of %s %s%s", CFG_PRINTF_NEWLINE, path, CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
      printf("       %-25s %12s %s", "Filename", "Size", CFG_PRINTF_NEWLINE);
      printf("       %-25s %12s %s", "--------", "----", CFG_PRINTF_NEWLINE);

      // Read directory contents
      int folderBytes = 0;
      for(;;) 
      {
          res = f_readdir(&dir, &Finfo);
          if ((res != FR_OK) || !Finfo.fname[0]) break;
          #if _USE_LFN == 0
            if (Finfo.fattrib & AM_DIR) 
              printf(" <DIR> %-25s %s", (char *)&Finfo.fname[0], CFG_PRINTF_NEWLINE);
            else
              printf("       %-25s %12d Bytes %s", (char *)&Finfo.fname[0], (int)(Finfo.fsize), CFG_PRINTF_NEWLINE);
              folderBytes += Finfo.fsize;
          #else
            // ToDo
          #endif
      }

      // Display folder size
      printf("%s", CFG_PRINTF_NEWLINE);
      printf("       %-25s %12d KB %s", "Folder Size: ", (int)(folderBytes / 1024), CFG_PRINTF_NEWLINE);

      // Get free disk space (only available if FATFS was compiled with _FS_MINIMIZE set to 0)
      #if _FS_MINIMIZE == 0 && _FS_READONLY == 0
        FATFS *fs = &Fatfs[0];
        DWORD clust;
      
        // Get free clusters
        res = f_getfree("0:", &clust, &fs);
    
        // Display total and free space
        printf("       %-25s %12d KB %s", "Disk Size: ", (int)((DWORD)(fs->max_clust - 2) * fs->csize / 2), CFG_PRINTF_NEWLINE);
        printf("       %-25s %12d KB %s", "Space Available: ", (int)(clust * fs->csize / 2), CFG_PRINTF_NEWLINE);
      #endif
    }
  }
}

#endif
