/* SD card and FAT filesystem example.
   This example uses SPI peripheral to communicate with SD card.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "../../eduboard2.h"
#include "../eduboard2_sdcard.h"
#include "sd_io.h"

#define SDCARD_FREQ_MHZ      SPI_MASTER_FREQ_10M

static const char *TAG = "SDCard_Driver";

#define MOUNT_POINT "/sdcard"

SD_DEV dev[1];          // Create device descriptor
uint8_t buffer[512];    // Example of your buffer data

void sdcard_test(void)
{
    
}

void eduboard_init_sdcard(void) {
    SDRESULTS res;
  // Part of your initialization code here
  if(SD_Init(dev)==SD_OK)
  {
    // You can read the sd card. For example you can read from the second
    // sector the set of bytes between [04..20]:
    // - Second sector is 1
    // - Offset is 4
    // - Bytes to count is 16 (20 minus 4)
    res = SD_Read(dev, (void*)buffer, 1, 4, 16);
    if(res==SD_OK)
    {
      // Maybe you wish change the data on this sector:
      res = SD_Write(dev, (void*)buffer, 1);
      if(res==SD_OK)
      {
        // Some action here
      }
    }
  }
}