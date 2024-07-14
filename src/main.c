
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
    for(;;) {
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
                    // eduboard_set_pwmled(10, 50, 100);
                    eduboard_set_ws2812(10, 50, 100);
                    break;
                case 2:
                    // eduboard_set_pwmled(50, 100, 10);
                    eduboard_set_ws2812(50, 100, 10);
                    break;
                case 3:
                    // eduboard_set_pwmled(100, 10, 50);
                    eduboard_set_ws2812(100, 10, 50);
                    break;
                case 4:
                    // eduboard_set_pwmled(100, 100, 100);
                    eduboard_set_ws2812(100, 100, 100);
                    break;
                case 5:
                    // eduboard_set_pwmled(0, 0, 0);
                    eduboard_set_ws2812(0, 0, 0);
                    pwmstate = 0;
                    break;
            } 
        }
        if(getButtonState(SW3, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW3 = Long:");
        }
        if(getEncoderButtonState(false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "RotEnc Button Short Pressed");
        }
        if(getEncoderButtonState(true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "RotEnc Button Long Pressed");
            getEncoderRotation(true);
        }
        int32_t rotenc_value = getEncoderRotation(false);
        // if(rotenc_value != 0) {
        ESP_LOGI(TAG, "Rotation: %i", (int)(rotenc_value));
        // }
        if(isTouched()) {
            touchevent_t touchevent = getTouchEvent(true);
            ESP_LOGI(TAG, "Touched");
            ESP_LOGI(TAG, "touches: %i", touchevent.touches);
            ESP_LOGI(TAG, "P1: %i:%i", touchevent.points[0].x, touchevent.points[0].y);
            if(touchevent.touches > 1) {
                ESP_LOGI(TAG, "P2: %i:%i", touchevent.points[1].x, touchevent.points[1].y);
            }
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
    
    eduboard2_init();
    rtc_setDate(14,SUNDAY, JULY, 2024);
    rtc_setTime(06,29,00);

    xTaskCreate(gpioTestTask, "gpioTestTask", 20*2048, NULL, 10, NULL);
    for(;;) {
        tmp112_poll();
        ESP_LOGI(TAG, "Temp: %f", eduboard_get_val_tmp112());
        ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)eduboard_get_ADC_raw(), (unsigned int)eduboard_get_ADC_voltage_mv());
        // tmp112_poll();
        // ESP_LOGI(TAG, "Temp: %f", eduboard_get_val_tmp112());
        // ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)eduboard_get_ADC_raw(), (unsigned int)eduboard_get_ADC_voltage_mv());
        uint8_t hour,min,sec;
        uint16_t year;
        uint8_t month,day,weekday;
        rtc_getTime(&hour,&min, &sec);
        rtc_getDate(&year,&month,&day,&weekday);
        ESP_LOGI(TAG, "Time: %02i:%02i:%02i", hour,min,sec);
        ESP_LOGI(TAG, "Date: %02i.%02i.%04i - Weekday: %i", day,month,year,weekday);
        ESP_LOGI(TAG, "Unix Timestamp: %u", (int)(rtc_getUnixTimestamp()));
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}