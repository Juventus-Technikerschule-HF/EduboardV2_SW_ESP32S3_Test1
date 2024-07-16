
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
    static int32_t rotenc_value_last = 0;
    SemaphoreHandle_t sem_alarm;
    sem_alarm = xSemaphoreCreateBinary();
    // eduboard_set_buzzer_volume(40);
    for(;;) {
        if(button_get_state(SW0, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Short:");
            buzzer_start(1000, 100);  
            vTaskDelay(100/portTICK_PERIOD_MS);
            buzzer_start(1500, 100);  
            vTaskDelay(100/portTICK_PERIOD_MS);
            buzzer_start(2000, 100);  
        }
        if(button_get_state(SW0, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW0 = Long:");
            uint8_t hour,min,sec;
            rtc_getTime(&hour, &min, &sec);
            min += 2;
            rtc_setAlarmTime(RTC_ALARM_DEACTIVATED, min, RTC_ALARM_DEACTIVATED, RTC_ALARM_DEACTIVATED);
            rtc_configAlarm(RTCALARM_ENABLED, sem_alarm);
            ESP_LOGI(TAG, "RTC Alarm set to %02i:%02i:%02i", hour,min,sec);
        }
        if(button_get_state(SW1, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW1 = Short:");
            
        }
        if(button_get_state(SW1, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW1 = Long:");
            rtc_setTimerTime(10);
            rtc_configTimer(RTCTIMER_ENABLED, NULL, RTCFREQ_1HZ);
            ESP_LOGI(TAG, "RTC Alarm started for 10s");
        }
        if(button_get_state(SW2, false) == SHORT_PRESSED) {
            ESP_LOGI(TAG, "SW2 = Short:");
            
        }
        if(button_get_state(SW2, true) == LONG_PRESSED) {
            ESP_LOGI(TAG, "SW2 = Long:");
            
        }
        if(button_get_state(SW3, false) == SHORT_PRESSED) {
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
        if(button_get_state(SW3, true) == LONG_PRESSED) {
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
        if(rotenc_value != rotenc_value_last) {
            ESP_LOGI(TAG, "Rotation: %i", (int)(rotenc_value));
        }
        rotenc_value_last = rotenc_value;
        if(ft6236_isTouched()) {
            touchevent_t touchevent = ft6236_getTouchEvent(true);
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
        if(xSemaphoreTake(sem_alarm, 0) == pdTRUE) {
            ESP_LOGI(TAG, "Alarm occured!");
            buzzer_start(2000, 100);
            vTaskDelay(200/portTICK_PERIOD_MS);
            buzzer_start(2000, 100);
            vTaskDelay(200/portTICK_PERIOD_MS);
            buzzer_start(2000, 100);
            vTaskDelay(200/portTICK_PERIOD_MS);
            buzzer_start(2000, 100);
            rtc_configAlarm(RTCALARM_DISABLED, NULL);
        }
        if(rtc_timer_elapsed()) {
            ESP_LOGI(TAG, "Timer elapsed!");
            buzzer_start(4000, 50);
            vTaskDelay(100/portTICK_PERIOD_MS);
            buzzer_start(2000, 50);
            vTaskDelay(100/portTICK_PERIOD_MS);
            buzzer_start(4000, 50);
            rtc_configTimer(RTCALARM_DISABLED, NULL, 0);
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
        // ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)eduboard_get_ADC_raw(), (unsigned int)eduboard_get_ADC_voltage_mv());
        ESP_LOGW(TAG, "-------------------------------------------------------------------");
        ESP_LOGI(TAG, "Temp: %.2f°C", tmp112_get_value());
        float x,y,z;
        stk8321_get_motion_data(&x,&y,&z);
        ESP_LOGI(TAG, "ACC: x:%.2f - y:%.2f - z:%.2f", x, y, z);
        uint8_t hour,min,sec;
        uint16_t year;
        uint8_t month,day,weekday;
        rtc_getTime(&hour,&min, &sec);
        rtc_getDate(&year,&month,&day,&weekday);
        ESP_LOGI(TAG, "Time: %02i:%02i:%02i", hour,min,sec);
        ESP_LOGI(TAG, "Date: %02i.%02i.%04i - Weekday: %i", day,month,year,weekday);
        ESP_LOGI(TAG, "Unix Timestamp: %u", (int)(rtc_getUnixTimestamp()));
        flash_checkConnection();
        vTaskDelay(2000/portTICK_PERIOD_MS);
        
    }
}