#include "../../eduboard2.h"
#include "../eduboard2_rotary_encoder.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG "Eduboard2_RotaryEncoderDriver"
#define BUTTON_UPDATE_TIME_MS       10

typedef struct {
    uint32_t count;
    button_state state;
    uint32_t timeout;
} button_data;

button_state rotarybuttonstate;


void rotaryEncoderTask(void* param) {
    ESP_LOGI(TAG, "Init Rotary Encoder");
    for(;;) {

        vTaskDelay(1/portTICK_PERIOD_MS);
    }
}

button_state getEncoderButtonState(bool reset) {

    return rotarybuttonstate;
}
int32_t getEncoderRotation(bool reset) {
    return 0;
}
void eduboard_init_rotary_encoder() {
    xTaskCreate(rotaryEncoderTask, "rotenc_task", 2048, NULL, 10, NULL);
}