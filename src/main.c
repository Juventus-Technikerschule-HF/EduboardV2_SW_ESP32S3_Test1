
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "eduboard2.h"
#include "memon.h"

#define TAG "TEST"


void gpioTestTask(void* p) {
    static uint8_t pwmstate = 0;
    eduboard_init_leds();
    for(;;) {
        //ESP_LOGI(TAG, "GPIO Task:");
        if(getButtonState(SW0, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Short:");
            eduboard_toggle_led(LED2);
        }
        if(getButtonState(SW0, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Long:");
            eduboard_toggle_led(LED3);
        }
        if(getButtonState(SW1, true) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW1 = Short:");
            eduboard_toggle_led(LED4);
        }
        if(getButtonState(SW2, true) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW2 = Short:");
            eduboard_toggle_led(LED5);
        }
        if(getButtonState(SW3, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW3 = Short:");
            eduboard_toggle_led(LED6);        
        }
        if(getButtonState(SW3, true) == LONG_PRESSED) {
            pwmstate++;
            switch(pwmstate) {
                case 1:
                    eduboard_set_pwmled(10, 50, 100);
                    eduboard_set_ws2812(10, 50, 100);
                    break;
                case 2:
                    eduboard_set_pwmled(50, 100, 10);
                    eduboard_set_ws2812(50, 100, 10);
                    break;
                case 3:
                    eduboard_set_pwmled(100, 10, 50);
                    eduboard_set_ws2812(100, 10, 50);
                    break;
                case 4:
                    eduboard_set_pwmled(100, 100, 100);
                    eduboard_set_ws2812(100, 100, 100);
                    break;
                case 5:
                    eduboard_set_pwmled(0, 0, 0);
                    eduboard_set_ws2812(0, 0, 0);
                    pwmstate = 0;
                    break;
            } 
        }
        eduboard_toggle_led(LED0);
        eduboard_toggle_led(LED1);
        eduboard_toggle_led(LED7);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void app_main() 
{
    vTaskDelay(5000/portTICK_PERIOD_MS);
    //initMemon();
    //memon_enable();    
    eduboard_init_buzzer();    
    eduboard_init_buttons();
    eduboard_set_buzzer_volume(3);
    eduboard_init_ADC();
    xTaskCreate(gpioTestTask, "gpioTestTask", 2*2048, NULL, 10, NULL);
    eduboard_init_lcd();    
    int i = 0;
    for(;;) {
        i++;
        //ESP_LOGI(TAG, "Test: %i", i);      
        // eduboard_start_buzzer(1000, 100);  
        // vTaskDelay(100/portTICK_PERIOD_MS);
        // eduboard_start_buzzer(1500, 100);  
        // vTaskDelay(100/portTICK_PERIOD_MS);
        // eduboard_start_buzzer(2000, 100);  
        vTaskDelay(2000/portTICK_PERIOD_MS);        
    }
}