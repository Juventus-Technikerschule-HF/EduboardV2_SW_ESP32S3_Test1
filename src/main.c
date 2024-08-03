#include "eduboard2.h"
#include "memon.h"

#include "math.h"

#define TAG "TEST"

#define UPDATETIME_MS 100

uint8_t sinedata[] = {0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,
                        0x7F,0x98,0xB0,0xC6,0xD9,0xE9,0xF4,0xFC,0xFE,0xFC,0xF4,0xE9,0xD9,0xC6,0xB0,0x98,
                        0x7F,0x66,0x4E,0x38,0x25,0x15,0xA,0x2,0x0,0x2,0xA,0x15,0x25,0x38,0x4E,0x66,};

enum state {
    STATE_INIT,
    STATE_WAITFORMAINSCREEN,
    STATE_TEST_BUTTON,
    STATE_TEST_LED,
    STATE_TEST_WS2812,
    STATE_TEST_BUZZER,
    STATE_TEST_ROTARYENCODER,
    STATE_TEST_ADC,
    STATE_TEST_DAC,
    STATE_TEST_TOUCH,
    STATE_TEST_ACCSENSOR,
    STATE_TEST_TEMPERATURE,
    STATE_TEST_RTC,
    STATE_TEST_FLASH,
    STATE_TEST_USBSERIAL,
};



uint8_t state = STATE_INIT;

void testButtons() {
    static uint8_t buttonFade[4] = {0,0,0,0};
    
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        buttonFade[0] = 10;
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        buttonFade[1] = 10;
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        buttonFade[2] = 10;
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        buttonFade[3] = 10;
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "Button Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    float percentage = 0;
    for(int i = 0; i < 4; i++) {        
        if(buttonFade[i] > 0) {
            percentage = 1.0/10.0*(float)(buttonFade[i]);
            lcdDrawFillRect((i*120)+1, 219, (i*120)+118, 319, rgb565_conv((uint8_t)(255.0*percentage), 0x00, (uint8_t)(255.0*percentage)));
            buttonFade[i]--;
        }
    }
    lcdDrawString(fx24M, 30, 269, "BT1", WHITE);
    lcdDrawString(fx24M, 150, 269, "BT2", WHITE);
    lcdDrawString(fx24M, 270, 269, "BT3", WHITE);
    lcdDrawString(fx24M, 390, 269, "BT4", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testLED() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            led_setAll(0x00);
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        led_toggle(LED0);
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        led_setAll(0xFF);
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        led_setAll(0x00);
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        led_toggle(LED7);
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "LED-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx24M, 30, 269, "Left", WHITE);
    lcdDrawString(fx24M, 150, 269, " On", WHITE);
    lcdDrawString(fx24M, 270, 269, "OFF", WHITE);
    lcdDrawString(fx24M, 385, 269, "Right", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testWS2812() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            ws2812_set(0x00, 0x00, 0x00);
            buzzer_set_volume(10);
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        ws2812_set(0xFF, 0x00, 0x00);
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        ws2812_set(0x00, 0xFF, 0x00);
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        ws2812_set(0x00, 0x00, 0xFF);
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        ws2812_set(0xFF, 0x00, 0xFF);
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "WS2812-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx24M, 30, 269, "RED", WHITE);
    lcdDrawString(fx24M, 140, 269, "GREEN", WHITE);
    lcdDrawString(fx24M, 265, 269, "BLUE", WHITE);
    lcdDrawString(fx24M, 380, 269, "PURPLE", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testBuzzer() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        buzzer_start(2000, 100);
        vTaskDelay(200/portTICK_PERIOD_MS);
        buzzer_start(2000, 100);
        vTaskDelay(200/portTICK_PERIOD_MS);
        buzzer_start(2000, 100);
        vTaskDelay(200/portTICK_PERIOD_MS);
        buzzer_start(2000, 100);
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "Buzzer-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx24M, 30, 269, "Test", WHITE);
    
    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testRotaryEncoder() {
    static uint8_t rotencbuttonfade = 0;
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        rotary_encoder_get_rotation(true);
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }

    if(rotary_encoder_button_get_state(true) > NOT_PRESSED) {
        rotencbuttonfade = 10;
    }

    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "RotaryEncoder-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);

    int32_t rotencvalue = rotary_encoder_get_rotation(false);    
    int16_t xdif = 0, ydif = 0;
    int16_t radius = 40;
    float angle = 360.0/48.0*(float)rotencvalue;
    float anglerad = M_PI/180.0*angle;
    xdif = (int16_t)((float)(radius)*cos(anglerad));
    ydif = (int16_t)((float)(radius)*sin(anglerad));

    uint16_t xmid = 240;
    uint16_t ymid = 160;

    lcdDrawLine(xmid, ymid, xmid + xdif, ymid + ydif, RED);
    lcdDrawCircle(xmid,ymid,radius, BLUE);

    if(rotencbuttonfade > 0) {
        float percentage = 1.0/10.0*(float)(rotencbuttonfade);
        lcdDrawFillCircle(xmid,ymid,5, rgb565_conv(0x00, (uint8_t)(255.0*percentage), (uint8_t)(255.0*percentage)));
        rotencbuttonfade--;
    }

    
    lcdDrawString(fx24M, 20, 269, "Reset", WHITE);
    
    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
#define ADC_BUFFER_SIZE 400
void testADC() {
    static bool init = false;
    static uint8_t adcRawBuffer[ADC_BUFFER_SIZE];
    if(init == false) {
        memset(adcRawBuffer, 0x00, sizeof(adcRawBuffer));
        init = true;
    }

    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }

    uint32_t adcValue = adc_get_raw();
    uint32_t adcVoltage_mv = adc_get_voltage_mv();

    for(int i = ADC_BUFFER_SIZE-1; i > 0; i--) {
        adcRawBuffer[i] = adcRawBuffer[i-1];
    }
    adcRawBuffer[0] = (uint8_t)(200.0/4096.0*(float)(adcValue));

    

    lcdFillScreen(BLACK);

    lcdDrawDataUInt8(10, 80, ADC_BUFFER_SIZE, 120, 0, 200, false, adcRawBuffer, YELLOW);
    lcdDrawRect(10,80, 410, 200, BLUE);
    char adcText[60];
    sprintf(adcText, "Raw: %i -> %imV", (int)adcValue, (int)adcVoltage_mv);
    lcdDrawString(fx16M, 30, 100, adcText, WHITE);

    lcdDrawString(fx32M, 10, 30, "ADC-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}

void dacCallbackFunction() {
    dac_load_stream_data(sinedata, sinedata);
}
void testDAC() {    
    static bool initDone = false;
    if(initDone == false) {
        dac_set_config(DAC_A, DAC_GAIN_1, true);
        dac_set_config(DAC_B, DAC_GAIN_1, true);
        dac_update();
        dac_set_stream_callback(&dacCallbackFunction);
        dac_load_stream_data(sinedata, sinedata);
        initDone = true;
    }
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            dac_set_config(DAC_A, DAC_GAIN_1, false);
            dac_set_config(DAC_B, DAC_GAIN_1, false);
            dac_set_stream_callback(NULL);
            dac_update();
            initDone = false;
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        dac_set_value(DAC_A, 0x80);
        dac_set_value(DAC_B, 0x40);
        dac_update();
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        dac_set_value(DAC_A, 0x40);
        dac_set_value(DAC_B, 0x80);
        dac_update();
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }
    
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "DAC-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx24M, 10, 269, "0.5V/1V", WHITE);
    lcdDrawString(fx24M, 130, 269, "1V/0.5V", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testTouch() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }

    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "Touch-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    if(ft6236_is_touched()) {
        touchevent_t touchevent = ft6236_get_touch_event(true);
        char touchtext[50];
        lcdDrawPixel(360 + (touchevent.points[0].x/4), 80 + (touchevent.points[0].y/4), PURPLE);
        sprintf(touchtext, "TouchP1 : x:%i - y:%i", touchevent.points[0].x, touchevent.points[0].y);
        lcdDrawString(fx16M, 10, 90, touchtext, WHITE);
        if(touchevent.touches > 1) {
            lcdDrawPixel(360 + (touchevent.points[1].x/4), 80 + (touchevent.points[1].y/4), YELLOW);
            sprintf(touchtext, "TouchP2 : x:%i - y:%i", touchevent.points[1].x, touchevent.points[1].y);
            lcdDrawString(fx16M, 10, 110, touchtext, WHITE);
        }
    }
    lcdDrawRect(360,80,479,159, RED);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
#define ACC_BUFFER_SIZE 400
void testAccSensor() {
    static bool init = false;
    static uint8_t accXBuffer[ACC_BUFFER_SIZE];
    static uint8_t accYBuffer[ACC_BUFFER_SIZE];
    static uint8_t accZBuffer[ACC_BUFFER_SIZE];
    if(init == false) {
        memset(accXBuffer, 0x00, sizeof(accXBuffer));
        memset(accYBuffer, 0x00, sizeof(accYBuffer));
        memset(accZBuffer, 0x00, sizeof(accZBuffer));
        init = true;
    }

    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }

    float x,y,z;
    stk8321_get_motion_data(&x,&y,&z);

    for(int i = ACC_BUFFER_SIZE-1; i > 0; i--) {
        accXBuffer[i] = accXBuffer[i-1];
        accYBuffer[i] = accYBuffer[i-1];
        accZBuffer[i] = accZBuffer[i-1];
    }
    accXBuffer[0] = (uint8_t)(128.0 + (100.0*x));
    accYBuffer[0] = (uint8_t)(128.0 + (100.0*y));
    accZBuffer[0] = (uint8_t)(128.0 + (100.0*z));

    lcdFillScreen(BLACK);

    lcdDrawDataUInt8(10, 80, ACC_BUFFER_SIZE, 120, 0, 255, false, accXBuffer, RED);
    lcdDrawDataUInt8(10, 80, ACC_BUFFER_SIZE, 120, 0, 255, false, accYBuffer, GREEN);
    lcdDrawDataUInt8(10, 80, ACC_BUFFER_SIZE, 120, 0, 255, false, accZBuffer, BLUE);
    
    lcdDrawRect(10,80, 410, 200, BLUE);
    char accText[60];
    sprintf(accText, "X: %.2f - Y: %.2f - Z: %.2f", x,y,z);
    lcdDrawString(fx16M, 30, 100, accText, WHITE);

    lcdDrawString(fx32M, 10, 30, "ACCSensor-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testTemperature() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }

    float temperature = tmp112_get_value();

    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "Temperature-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);

    char tempstring[60];
    sprintf(tempstring, "Temperature: %.2f deg Celsius", temperature);
    lcdDrawString(fx24M, 10, 100, tempstring, WHITE);
    
    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testRTC() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        rtc_set_date(14,SUNDAY, JULY, 2024);
        rtc_set_time(18,15,00);
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "RTC-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);

    uint8_t hour,min,sec,month,day,weekday;
    uint16_t year;
    rtc_get_time(&hour,&min, &sec);
    rtc_get_date(&year,&month,&day,&weekday);
    uint32_t unixtimestamp = rtc_get_unix_timestamp();

    char mytext[60];
    sprintf((char *)mytext, "Time: %02u:%02u:%02u", (unsigned int)hour, (unsigned int)min, (unsigned int)sec);
    lcdDrawString(fx16M, 10, 90, &mytext[0], WHITE);
    sprintf((char *)mytext, "Date: %02u:%02u:%04u", (unsigned int)day, (unsigned int)month, (unsigned int)year);
    lcdDrawString(fx16M, 10, 110, &mytext[0], WHITE);
    sprintf((char *)mytext, "Unix: %u", (unsigned int)unixtimestamp);
    lcdDrawString(fx16M, 10, 130, &mytext[0], WHITE);
    
    lcdDrawString(fx24M, 10, 269, "Set Time", WHITE);
    lcdDrawString(fx24M, 130, 269, "Set Date", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testFlash() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state++;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "Flash-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx32M, 10, 100, "Todoooo", RED);

    lcdDrawString(fx24M, 30, 269, "BT1", WHITE);
    lcdDrawString(fx24M, 150, 269, "BT2", WHITE);
    lcdDrawString(fx24M, 270, 269, "BT3", WHITE);
    lcdDrawString(fx24M, 390, 269, "BT4", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}
void testUSBSerial() {
    for(int i = 0; i < 4; i++) {
        if(button_get_state(i, false) == LONG_PRESSED) {
            state = STATE_TEST_BUTTON;
        }
    }
    if(button_get_state(SW0, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW1, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW2, true) == SHORT_PRESSED) {
        
    }
    if(button_get_state(SW3, true) == SHORT_PRESSED) {
        
    }
    lcdFillScreen(BLACK);
    lcdDrawString(fx32M, 10, 30, "USBSerial-Test", GREEN);
    lcdDrawString(fx24M, 10, 60, "Long press any Button for next Test", GREEN);
    
    lcdDrawString(fx32M, 10, 100, "Todoooo", RED);

    lcdDrawString(fx24M, 30, 269, "BT1", WHITE);
    lcdDrawString(fx24M, 150, 269, "BT2", WHITE);
    lcdDrawString(fx24M, 270, 269, "BT3", WHITE);
    lcdDrawString(fx24M, 390, 269, "BT4", WHITE);

    lcdDrawRect(0, 219, 119, 319, BLUE);
    lcdDrawRect(120, 219, 239, 319, BLUE);
    lcdDrawRect(240, 219, 359, 319, BLUE);
    lcdDrawRect(360, 219, 479, 319, BLUE);
    
    lcdUpdateVScreen();
}



void testTask(void* p) {
    uint32_t updates = 0, lastupdates = 0;
    uint32_t time = 0, lasttime = 0;
    TickType_t t_updatetime = xTaskGetTickCount();
    for(;;) {
        time = xTaskGetTickCount();
        if(time-lasttime >= 1000) {
            ESP_LOGI(TAG, "Updates/s: %i", (unsigned int)(updates-lastupdates));
            lastupdates = updates;
            lasttime = time;
        }
        updates++;
        switch(state) {
            case STATE_INIT:
                state = STATE_WAITFORMAINSCREEN;
            break;
            case STATE_WAITFORMAINSCREEN:
                if(button_get_state(SW0, true) == SHORT_PRESSED) {
                    state = STATE_TEST_BUTTON;
                }
            break;
            case STATE_TEST_BUTTON:
                testButtons();
            break;
            case STATE_TEST_LED:
                testLED();
            break;
            case STATE_TEST_WS2812:
                testWS2812();
            break;
            case STATE_TEST_BUZZER:
                testBuzzer();
            break;
            case STATE_TEST_ROTARYENCODER:
                testRotaryEncoder();
            break;
            case STATE_TEST_ADC:
                testADC();
            break;
            case STATE_TEST_DAC:
                testDAC();
            break;
            case STATE_TEST_TOUCH:
                testTouch();
            break;
            case STATE_TEST_ACCSENSOR:
                testAccSensor();
            break;
            case STATE_TEST_TEMPERATURE:
                testTemperature();
            break;
            case STATE_TEST_RTC:
                testRTC();
            break;
            case STATE_TEST_FLASH:
                testFlash();
            break;
            case STATE_TEST_USBSERIAL:
                testUSBSerial();
            break;
        }
        vTaskDelayUntil(&t_updatetime, UPDATETIME_MS/portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // initMemon();
    // memon_enable();

    eduboard2_init();
    
    xTaskCreate(testTask, "testTask", 20*2048, NULL, 10, NULL);
    for(;;) {
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}