#include "../../eduboard2.h"
#include "../eduboard2_dac.h"
#include "driver/gpio.h"
#include "esp_timer.h"

/*************************************************************************
*               Driver for SPI DAC MCP4802-E/SN
*
*
*************************************************************************/

#define DAC_FREQ_MHZ      SPI_MASTER_FREQ_20M

#define DAC_A_BASE              0x0000
#define DAC_B_BASE              0x8000
#define DAC_CONFIG_GAIN_1       0x2000
#define DAC_CONFIG_GAIN_2       0x0000
#define DAC_CONFIG_OUTPUT_ON    0x1000
#define DAC_CONFIG_OUTPUT_OFF   0x0000

#ifdef CONFIG_DAC_STREAMING
QueueHandle_t dacstreamqueue;
#endif


typedef struct {
    bool enabled;
    dac_gain_t gain;
    uint8_t value;
} dac_data_t;

SemaphoreHandle_t dacLock;

dac_data_t dacData[2];

#define TAG "DAC_driver"

spi_device_handle_t dev_dac_spi;

void dac_setConfig(dac_num_t dacNum, dac_gain_t gain, bool enabled) {
    xSemaphoreTake(dacLock, portMAX_DELAY);
    dacData[dacNum].gain = gain;
    dacData[dacNum].enabled = enabled;
    xSemaphoreGive(dacLock);
}

void dac_setValue(dac_num_t dacNum, uint8_t value) {
    xSemaphoreTake(dacLock, portMAX_DELAY);
    dacData[dacNum].value = value;
    xSemaphoreGive(dacLock);
}

void dac_update() {
#ifndef CONFIG_DAC_STREAMING
    uint16_t dataA = DAC_A_BASE;
    uint16_t dataB = DAC_B_BASE;
    xSemaphoreTake(dacLock, portMAX_DELAY);
    if(dacData[DAC_A].enabled) dataA |= DAC_CONFIG_OUTPUT_ON;
    if(dacData[DAC_B].enabled) dataB |= DAC_CONFIG_OUTPUT_ON;
    if(dacData[DAC_A].gain) dataA |= DAC_CONFIG_GAIN_1;
    if(dacData[DAC_B].gain) dataB |= DAC_CONFIG_GAIN_1;
    dataA |= ((dacData[DAC_A].value & 0x00FF)<<4);
    dataB |= ((dacData[DAC_B].value & 0x00FF)<<4);
    xSemaphoreGive(dacLock);
    uint8_t data[2];
    data[0] = dataA >> 8;
    data[1] = dataA & 0xFF;
    gpspi_write_data(&dev_dac_spi, data, 2);
    data[0] = dataB >> 8;
    data[1] = dataB & 0xFF;
    gpspi_write_data(&dev_dac_spi, data, 2);
    gpio_set_level(GPIO_SD_LDAC_CS, 0);
    vTaskDelay(1);
    gpio_set_level(GPIO_SD_LDAC_CS, 1);
#endif
}

static void periodic_dac_callback(void* arg);

void eduboard_init_dac() {
    ESP_LOGI(TAG, "Init DAC...");
    dacLock = xSemaphoreCreateMutex();
    for(int i = 0; i < 2; i++) {
        dacData[i].enabled = false;
        dacData[i].gain = false;
        dacData[i].value = 0;
    }
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.pin_bit_mask = 0;
    io_conf.pin_bit_mask |= (1ULL<<GPIO_SD_LDAC_CS);
    gpio_config(&io_conf);
    gpio_set_level(GPIO_SD_LDAC_CS, 1);
#ifdef CONFIG_DAC_STREAMING
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_dac_callback,
        .name = "periodic_dac"
    };
    esp_timer_handle_t periodic_dac_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_dac_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_dac_timer, 500000));
    dacstreamqueue = xQueueCreate(2, CONFIG_DAC_STREAMING_BUFFERSIZE);
    gpspi_init_nonblocking(&dev_dac_spi, GPIO_MOSI, GPIO_MISO, GPIO_SCK, GPIO_FLASH_DAC_CS, DAC_FREQ_MHZ, false);
#else
    gpspi_init(&dev_dac_spi, GPIO_MOSI, GPIO_MISO, GPIO_SCK, GPIO_FLASH_DAC_CS, DAC_FREQ_MHZ, false);
#endif
}