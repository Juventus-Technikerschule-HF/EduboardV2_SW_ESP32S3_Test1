#include "eduboard2.h"
#include "memon.h"

#define TAG "TEST"

#define UPDATETIME_MS 10

struct systemdata_t {
    struct {
        float x;
        float y;
        float z;
    }accsensor;
    struct  {
        struct  {
            bool touched;
            int x;
            int y;
        }p1;
        struct  {
            bool touched;
            int x;
            int y;
        }p2;
    }touch;
    float temperature;
    int rotation;
    int adc_raw;
    int adc_mv;
    struct  {
        uint8_t sec;
        uint8_t min;
        uint8_t hour;
        uint8_t day;
        uint8_t month;
        uint16_t year;
        uint8_t weekday;
        uint32_t unixtimestamp;
    }rtc;
} ;

struct systemdata_t systemdata;

enum state {
    STATE_INIT,
    STATE_WAITFORMAINSCREEN,
    STATE_MAINSCREEN,
};

uint8_t state = STATE_INIT;


void readSensorValues() {
    // Read ACC-Sensor STK8321 data
    stk8321_get_motion_data(&systemdata.accsensor.x,&systemdata.accsensor.y,&systemdata.accsensor.z);
    // ESP_LOGI(TAG, "ACC: x:%.2f - y:%.2f - z:%.2f", systemdata.accsensor.x, systemdata.accsensor.y, systemdata.accsensor.z);

    // Read Touch Sensor FT6236 data
    if(ft6236_is_touched()) {
        touchevent_t touchevent = ft6236_get_touch_event(true);
        // ESP_LOGI(TAG, "Touched");
        // ESP_LOGI(TAG, "touches: %i", touchevent.touches);
        // ESP_LOGI(TAG, "P1: %i:%i", touchevent.points[0].x, touchevent.points[0].y);
        systemdata.touch.p1.touched = true;
        systemdata.touch.p1.x = touchevent.points[0].x;
        systemdata.touch.p1.y = touchevent.points[0].y;
        if(touchevent.touches > 1) {
            // ESP_LOGI(TAG, "P2: %i:%i", touchevent.points[1].x, touchevent.points[1].y);
            systemdata.touch.p2.touched = true;
            systemdata.touch.p2.x = touchevent.points[1].x;
            systemdata.touch.p2.y = touchevent.points[1].y;
        } else {
            systemdata.touch.p2.touched = false;
        }
    } else {
        systemdata.touch.p1.touched = false;
        systemdata.touch.p2.touched = false;
    }

    // Read rtc PCF8563T time and date data
    rtc_get_time(&systemdata.rtc.hour,&systemdata.rtc.min, &systemdata.rtc.sec);
    rtc_get_date(&systemdata.rtc.year,&systemdata.rtc.month,&systemdata.rtc.day,&systemdata.rtc.weekday);
    systemdata.rtc.unixtimestamp = rtc_get_unix_timestamp();
    // ESP_LOGI(TAG, "Time: %02i:%02i:%02i", systemdata.rtc.hour,systemdata.rtc.min,systemdata.rtc.sec);
    // ESP_LOGI(TAG, "Date: %02i.%02i.%04i - Weekday: %i", systemdata.rtc.day,systemdata.rtc.month,systemdata.rtc.year,systemdata.rtc.weekday);
    // ESP_LOGI(TAG, "Unix Timestamp: %u", (int)(systemdata.rtc.unixtimestamp));

    // Read TMP112 Temperature data
    systemdata.temperature = tmp112_get_value();
    // ESP_LOGI(TAG, "Temp: %.2f°C", systemdata.temperature);

    // Read ADC Values of Potentiometer in raw and mv
    systemdata.adc_raw = adc_get_raw();
    systemdata.adc_mv = adc_get_voltage_mv();
    // ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)systemdata.adc_raw, (unsigned int)systemdata.adc_mv);

    // Read rotation value of rotary encoder
    systemdata.rotation = rotary_encoder_get_rotation(false);
}

void stateManager() {
    switch(state) {
        case STATE_INIT:
            
        break;
        case STATE_WAITFORMAINSCREEN:
            if(button_get_state(SW0, true) == SHORT_PRESSED) {
                state = STATE_MAINSCREEN;
            }
        break;
        case STATE_MAINSCREEN:
            
        break;
    }
}

void drawTouchPoints() {
    lcdDrawRect(360,0,479,79, RED);
    if(systemdata.touch.p1.touched) {
        lcdDrawPixel(360 + (systemdata.touch.p1.x/4), 0 + (systemdata.touch.p1.y/4), PURPLE);
    }
    if(systemdata.touch.p2.touched) {
        lcdDrawPixel(360 + (systemdata.touch.p2.x/4), 0 + (systemdata.touch.p2.y/4), YELLOW);
    }
}
void drawACCValues() {
    lcdDrawRect(360, 79, 479, 159, RED);
    lcdDrawFillRect(370, 95, 370 + (1+systemdata.accsensor.x)*50.0, 105, RED);
    lcdDrawFillRect(370, 115, 370 + (1+systemdata.accsensor.y)*50.0, 125, GREEN);
    lcdDrawFillRect(370, 135, 370 + (1+systemdata.accsensor.z)*50.0, 145, BLUE);
}

void drawTextValues() {
    lcdDrawRect(340, 160, 479, 319, RED);
    uint8_t mytext[50];
    sprintf((char *)mytext, "Temp: %.2f C", systemdata.temperature);
    lcdDrawString(fx16M, 350, 180, &mytext[0], WHITE);
    sprintf((char *)mytext, "ADC-Raw: %u", (unsigned int)systemdata.adc_raw);
    lcdDrawString(fx16M, 350, 200, &mytext[0], WHITE);
    sprintf((char *)mytext, "ADC-V: %umV", (unsigned int)systemdata.adc_mv);
    lcdDrawString(fx16M, 350, 220, &mytext[0], WHITE);
    sprintf((char *)mytext, "rotenc: %i", (int)systemdata.rotation);
    lcdDrawString(fx16M, 350, 240, &mytext[0], WHITE);
    
    sprintf((char *)mytext, "Time: %02u:%02u:%02u", (unsigned int)systemdata.rtc.hour, (unsigned int)systemdata.rtc.min, (unsigned int)systemdata.rtc.sec);
    lcdDrawString(fx16M, 350, 260, &mytext[0], WHITE);
    sprintf((char *)mytext, "Date: %02u:%02u:%04u", (unsigned int)systemdata.rtc.day, (unsigned int)systemdata.rtc.month, (unsigned int)systemdata.rtc.year);
    lcdDrawString(fx16M, 350, 280, &mytext[0], WHITE);
}

void drawMainScreen() {
    lcdFillScreen(BLACK);
    drawTouchPoints();
    drawACCValues();
    drawTextValues();
    lcdUpdateVScreen();
}





void testTask(void* p) {
    
    // static uint8_t pwmstate = 0;
    // static uint8_t ledstate = 0x00;
    // // static int32_t rotenc_value_last = 0;
    // SemaphoreHandle_t sem_alarm;
    // sem_alarm = xSemaphoreCreateBinary();
    // eduboard_set_buzzer_volume(40);
    for(;;) {
        switch(state) {
            case STATE_INIT:
                rtc_set_date(14,SUNDAY, JULY, 2024);
                rtc_set_time(06,29,00);

                state = STATE_WAITFORMAINSCREEN;
            break;
            case STATE_WAITFORMAINSCREEN:
                stateManager();
            break;
            case STATE_MAINSCREEN:
                readSensorValues();
                stateManager();
                drawMainScreen();
            break;
        }
        // if(button_get_state(SW0, false) == SHORT_PRESSED) {
        //     ESP_LOGI(TAG, "SW0 = Short:");
        //     buzzer_start(1000, 100);
        //     vTaskDelay(100/portTICK_PERIOD_MS);
        //     buzzer_start(1500, 100);
        //     vTaskDelay(100/portTICK_PERIOD_MS);
        //     buzzer_start(2000, 100);
        // }
        // if(button_get_state(SW0, true) == LONG_PRESSED) {
        //     ESP_LOGI(TAG, "SW0 = Long:");
        //     uint8_t hour,min,sec;
        //     rtc_get_time(&hour, &min, &sec);
        //     min += 2;
        //     rtc_set_alarm_time(RTC_ALARM_DEACTIVATED, min, RTC_ALARM_DEACTIVATED, RTC_ALARM_DEACTIVATED);
        //     rtc_config_alarm(RTCALARM_ENABLED, sem_alarm);
        //     ESP_LOGI(TAG, "RTC Alarm set to %02i:%02i:%02i", hour,min,sec);
        // }
        // if(button_get_state(SW1, false) == SHORT_PRESSED) {
        //     ESP_LOGI(TAG, "SW1 = Short:");

        // }
        // if(button_get_state(SW1, true) == LONG_PRESSED) {
        //     ESP_LOGI(TAG, "SW1 = Long:");
        //     rtc_set_timer_time(10);
        //     rtc_config_timer(RTCTIMER_ENABLED, NULL, RTCFREQ_1HZ);
        //     ESP_LOGI(TAG, "RTC Alarm started for 10s");
        // }
        // if(button_get_state(SW2, false) == SHORT_PRESSED) {
        //     ESP_LOGI(TAG, "SW2 = Short:");

        // }
        // if(button_get_state(SW2, true) == LONG_PRESSED) {
        //     ESP_LOGI(TAG, "SW2 = Long:");

        // }
        // if(button_get_state(SW3, false) == SHORT_PRESSED) {
        //     ESP_LOGI(TAG, "SW3 = Short:");
        //     pwmstate++;
        //     switch(pwmstate) {
        //         case 1:
        //             // eduboard_set_pwmled(10, 50, 100);
        //             eduboard_set_ws2812(10, 50, 100);
        //             break;
        //         case 2:
        //             // eduboard_set_pwmled(50, 100, 10);
        //             eduboard_set_ws2812(50, 100, 10);
        //             break;
        //         case 3:
        //             // eduboard_set_pwmled(100, 10, 50);
        //             eduboard_set_ws2812(100, 10, 50);
        //             break;
        //         case 4:
        //             // eduboard_set_pwmled(100, 100, 100);
        //             eduboard_set_ws2812(100, 100, 100);
        //             break;
        //         case 5:
        //             // eduboard_set_pwmled(0, 0, 0);
        //             eduboard_set_ws2812(0, 0, 0);
        //             pwmstate = 0;
        //             break;
        //     }
        // }
        // if(button_get_state(SW3, true) == LONG_PRESSED) {
        //     ESP_LOGI(TAG, "SW3 = Long:");
        // }
        // if(rotary_encoder_button_get_state(false) == SHORT_PRESSED) {
        //     ESP_LOGI(TAG, "RotEnc Button Short Pressed");
        // }
        // if(rotary_encoder_button_get_state(true) == LONG_PRESSED) {
        //     ESP_LOGI(TAG, "RotEnc Button Long Pressed");
        //     rotary_encoder_get_rotation(true);
        // }
        // // int32_t rotenc_value = rotary_encoder_get_rotation(false);
        // // if(rotenc_value != rotenc_value_last) {
        // //     ESP_LOGI(TAG, "Rotation: %i", (int)(rotenc_value));
        // // }
        // // rotenc_value_last = rotenc_value;
        // // if(ft6236_is_touched()) {
        // //     touchevent_t touchevent = ft6236_get_touch_event(true);
        // //     ESP_LOGI(TAG, "Touched");
        // //     ESP_LOGI(TAG, "touches: %i", touchevent.touches);
        // //     ESP_LOGI(TAG, "P1: %i:%i", touchevent.points[0].x, touchevent.points[0].y);
        // //     if(touchevent.touches > 1) {
        // //         ESP_LOGI(TAG, "P2: %i:%i", touchevent.points[1].x, touchevent.points[1].y);
        // //     }
        // // }
        // for(int i = 0; i< 8; i++) {
        //     if(ledstate == i) {
        //         eduboard_set_led(i, 1);
        //     } else {
        //         eduboard_set_led(i, 0);
        //     }
        // }
        // ledstate++;
        // if(ledstate == 8) {
        //     ledstate = 0;
        // }
        // if(xSemaphoreTake(sem_alarm, 0) == pdTRUE) {
        //     ESP_LOGI(TAG, "Alarm occured!");
        //     buzzer_start(2000, 100);
        //     vTaskDelay(200/portTICK_PERIOD_MS);
        //     buzzer_start(2000, 100);
        //     vTaskDelay(200/portTICK_PERIOD_MS);
        //     buzzer_start(2000, 100);
        //     vTaskDelay(200/portTICK_PERIOD_MS);
        //     buzzer_start(2000, 100);
        //     rtc_config_alarm(RTCALARM_DISABLED, NULL);
        // }
        // if(rtc_timer_elapsed()) {
        //     ESP_LOGI(TAG, "Timer elapsed!");
        //     buzzer_start(4000, 50);
        //     vTaskDelay(100/portTICK_PERIOD_MS);
        //     buzzer_start(2000, 50);
        //     vTaskDelay(100/portTICK_PERIOD_MS);
        //     buzzer_start(4000, 50);
        //     rtc_config_timer(RTCALARM_DISABLED, NULL, 0);
        // }

        vTaskDelay(UPDATETIME_MS/portTICK_PERIOD_MS);
    }
}

void app_main()
{
    //vTaskDelay(5000/portTICK_PERIOD_MS);
    // initMemon();
    // memon_enable();

    eduboard2_init();
    // rtc_set_date(14,SUNDAY, JULY, 2024);
    // rtc_set_time(06,29,00);

    xTaskCreate(testTask, "testTask", 20*2048, NULL, 10, NULL);
    for(;;) {
        ESP_LOGW(TAG, "-------------------------------------------------------------------");
        // ESP_LOGI(TAG, "Temp: %.2f°C", tmp112_get_value());
        // ESP_LOGI(TAG, "ADC - raw: %u - voltage: %umv", (unsigned int)adc_get_raw(), (unsigned int)adc_get_voltage_mv());
        // float x,y,z;
        // stk8321_get_motion_data(&x,&y,&z);
        // ESP_LOGI(TAG, "ACC: x:%.2f - y:%.2f - z:%.2f", x, y, z);
        // uint8_t hour,min,sec;
        // uint16_t year;
        // uint8_t month,day,weekday;
        // rtc_get_time(&hour,&min, &sec);
        // rtc_get_date(&year,&month,&day,&weekday);
        // ESP_LOGI(TAG, "Time: %02i:%02i:%02i", hour,min,sec);
        // ESP_LOGI(TAG, "Date: %02i.%02i.%04i - Weekday: %i", day,month,year,weekday);
        // ESP_LOGI(TAG, "Unix Timestamp: %u", (int)(rtc_get_unix_timestamp()));
        flash_checkConnection();
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}