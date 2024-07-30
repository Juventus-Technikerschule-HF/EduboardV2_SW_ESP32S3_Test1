#pragma once
#define EDUBOARD_CPU_BOARD_ESP32_S3
//#define EDUBOARD_CPU_BOARD_ATMEGA328PB

/*LED Config*/
#define CONFIG_ENABLE_LED0
#define CONFIG_ENABLE_LED1
#define CONFIG_ENABLE_LED2
#define CONFIG_ENABLE_LED3
#define CONFIG_ENABLE_LED4
#define CONFIG_ENABLE_LED5
#define CONFIG_ENABLE_LED6
#define CONFIG_ENABLE_LED7

// #define CONFIG_ENABLE_PWMLED

#define CONFIG_ENABLE_WS2812

/*Buzzer Config*/
#define CONFIG_ENABLE_BUZZER

/*Button Config*/
#define CONFIG_ENABLE_SW0
#define CONFIG_ENABLE_SW1
#define CONFIG_ENABLE_SW2
#define CONFIG_ENABLE_SW3

#define CONFIG_ENABLE_ROTARYENCODER

/*Analog Input Config*/
#define CONFIG_ENABLE_AN0
#ifdef CONFIG_ENABLE_AN0
    //#define CONFIG_AN0_DEBUG
#endif

/*DAC Output Config*/
#define CONFIG_ENABLE_DAC

/*LCD Config*/
#define CONFIG_ENABLE_LCD
#ifdef CONFIG_ENABLE_LCD
    // #define CONFIG_LCD_ST7789
    #define CONFIG_LCD_ILI9488
    
    // #define CONFIG_LCD_RESOLUTION_240x240
    // #define CONFIG_LCD_RESOLUTION_240x320
    #define CONFIG_LCD_RESOLUTION_320x480

    // #define CONFIG_USE_VSCREEN
    #define CONFIG_USE_DIFFUPDATE
    
    // #define CONFIG_LCD_TEST    
#endif

#define CONFIG_ENABLE_SENSOR_TMP112

#define CONFIG_ENABLE_SENSOR_STK8321

#define CONFIG_ENABLE_TOUCH_FT6236

#define CONFIG_ENABLE_SPIFFS

#define CONFIG_ENABLE_RTC
#ifdef CONFIG_ENABLE_RTC
    // #define CONFIG_RTC_RESET_DATA_ON_STARTUP
    // #define CONFIG_RTC_SHOW_TIME
#endif

// #define CONFIG_ENABLE_FLASH
