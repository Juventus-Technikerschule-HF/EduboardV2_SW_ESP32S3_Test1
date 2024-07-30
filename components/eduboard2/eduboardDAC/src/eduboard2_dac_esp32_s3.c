#include "../../eduboard2.h"
#include "../eduboard2_dac.h"

/*************************************************************************
*               Driver for SPI DAC MCP4802-E/SN
*
*
*************************************************************************/

#define DAC_FREQ_MHZ      SPI_MASTER_FREQ_10M

#define TAG "DAC_driver"

void eduboard_init_dac() {
    ESP_LOGI(TAG, "Init DAC...");
}