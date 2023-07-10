
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
#include "gpi2c.h"
#include "eduboard2.h"
#include "memon.h"

#define TAG "TEST"


void gpioTestTask(void* p) {
    static uint8_t pwmstate = 0;
    static uint8_t ledstate = 0x00;
    eduboard_init_leds();
    for(;;) {
        //ESP_LOGI(TAG, "GPIO Task:");
        if(getButtonState(SW0, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Short:");
            eduboard_start_buzzer(1000, 100);  
            vTaskDelay(100/portTICK_PERIOD_MS);
            eduboard_start_buzzer(1500, 100);  
            vTaskDelay(100/portTICK_PERIOD_MS);
            eduboard_start_buzzer(2000, 100);  
        }
        if(getButtonState(SW0, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Long:");
            
        }
        if(getButtonState(SW1, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW1 = Short:");
            
        }
        if(getButtonState(SW1, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW1 = Long:");
            
        }
        if(getButtonState(SW2, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW2 = Short:");
            
        }
        if(getButtonState(SW2, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW2 = Long:");
            
        }
        if(getButtonState(SW3, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW3 = Short:");
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
        if(getButtonState(SW3, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW3 = Long:");
        }
        
        for(int i = 0; i< 8; i++) {
            if(ledstate == i) {
                eduboard_set_led(i, 1);
            } else {
                eduboard_set_led(i, 0);
            }
        }
        ledstate++;
        if(ledstate == 8) {
            ledstate = 0;
        }

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void app_main() 
{
    //vTaskDelay(5000/portTICK_PERIOD_MS);
    // initMemon();
    // memon_enable();    
    
    eduboard_init_spiffs();
    eduboard_init_buzzer();    
    eduboard_init_buttons();
    eduboard_set_buzzer_volume(3);
    eduboard_init_ADC();
    xTaskCreate(gpioTestTask, "gpioTestTask", 20*2048, NULL, 10, NULL);
    eduboard_init_tmp112();    
    eduboard_init_lcd();       
    int i = 0;
    for(;;) {
        i++;
        // ESP_LOGI(TAG, "Test: %i", i);      
        // eduboard_start_buzzer(1000, 100);  
        // vTaskDelay(100/portTICK_PERIOD_MS);
        // eduboard_start_buzzer(1500, 100);  
        // vTaskDelay(100/portTICK_PERIOD_MS);
        // eduboard_start_buzzer(2000, 100);  
        tmp112_poll();
        ESP_LOGI(TAG, "Temp: %f", eduboard_get_val_tmp112());
        ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)eduboard_get_ADC_raw(), (unsigned int)eduboard_get_ADC_voltage_mv());
        vTaskDelay(2000/portTICK_PERIOD_MS);        
    }
}