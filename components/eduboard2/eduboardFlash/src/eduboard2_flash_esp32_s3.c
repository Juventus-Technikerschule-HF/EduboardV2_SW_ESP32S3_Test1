#include "../../eduboard2.h"
#include "../eduboard2_flash.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "esp_partition.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "soc/spi_pins.h"

#include "esp_system.h"



#define FLASH_FREQ_MHZ      40
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

#define TAG "Flash_driver"

spi_device_handle_t flash_handle;

void eduboard_init_flash() {
    const spi_bus_config_t bus_config = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCK,
    };

    const esp_flash_spi_device_config_t device_config = {
        .host_id = HOST_ID,
        .cs_id = 0,
        .cs_io_num = GPIO_FLASH_CS,
        .io_mode = SPI_FLASH_FASTRD,
        .freq_mhz = FLASH_FREQ_MHZ,
    };

    ESP_LOGI(TAG, "Initializing external SPI Flash");
    ESP_LOGI(TAG, "Pin assignments:");
    ESP_LOGI(TAG, "MOSI: %2d   MISO: %2d   SCLK: %2d   CS: %2d",
        bus_config.mosi_io_num, bus_config.miso_io_num,
        bus_config.sclk_io_num, device_config.cs_io_num
    );

    // Initialize the SPI bus
    ESP_LOGI(TAG, "DMA CHANNEL: %d", SPI_DMA_CHAN);
    ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_config, SPI_DMA_CHAN));

    // Add device to the SPI bus
    esp_flash_t* ext_flash;
    ESP_ERROR_CHECK(spi_bus_add_flash_device(&ext_flash, &device_config));

    // Probe the Flash chip and initialize it
    esp_err_t err = esp_flash_init(ext_flash);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize external Flash: %s (0x%x)", esp_err_to_name(err), err);
        return NULL;
    }

    // Print out the ID and size
    uint32_t id;
    ESP_ERROR_CHECK(esp_flash_read_id(ext_flash, &id));
    ESP_LOGI(TAG, "Initialized external Flash, size=%" PRIu32 " KB, ID=0x%" PRIx32, ext_flash->size / 1024, id);

    return ext_flash;
}