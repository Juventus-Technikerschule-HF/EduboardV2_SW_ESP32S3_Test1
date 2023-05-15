#include "../../eduboard2.h"
#include "../eduboard2_analog.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define TAG "Eduboard2_ADCDriver"


uint32_t eduboard_get_ADC(uint8_t adc_num) {
    return 0;
}
void eduboard_init_ADC() {
    ESP_LOGI(TAG, "ADC not yet implemented");
}