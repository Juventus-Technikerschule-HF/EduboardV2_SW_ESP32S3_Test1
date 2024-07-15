#include "../../eduboard2.h"
#include "../eduboard2_flash.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_system.h"
#include "w25.h"




#define FLASH_FREQ_MHZ      SPI_MASTER_FREQ_10M

#define TAG "Flash_driver"

spi_device_handle_t dev_flash_spi;

void flash_checkConnection() {
    w25_read_jedec_id();
}

void eduboard_init_flash() {
    ESP_LOGI(TAG, "init flash...");    
    gpspi_init(&dev_flash_spi, GPIO_MOSI, GPIO_MISO, GPIO_SCK, GPIO_FLASH_CS, FLASH_FREQ_MHZ, false);
    w25_init(&dev_flash_spi);

    ESP_LOGI(TAG, "init flash done");
}