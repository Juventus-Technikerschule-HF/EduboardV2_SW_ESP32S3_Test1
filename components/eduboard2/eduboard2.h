#pragma once

#include "eduboard2_config.h"
#include "eduboard2_defines.h"

#ifdef CONFIG_ENABLE_LED
    #include "eduboardLED/eduboard2_led.h"
#endif

#ifdef CONFIG_ENABLE_BUZZER
    #include "eduboardBuzzer/eduboard2_buzzer.h"
#endif

#ifdef CONFIG_ENABLE_BUTTONS
    #include "eduboardButton/eduboard2_button.h"
#endif
#ifdef CONFIG_ENABLE_ROTARYENCODER
    #include "eduboardRotaryEncoder/eduboard2_rotary_encoder.h"
#endif

#ifdef CONFIG_ENABLE_ANALOG
    #include "eduboardAnalog/eduboard2_analog.h"
#endif

#ifdef CONFIG_ENABLE_SPIFFS
    #include "eduboardSpiffs/eduboard2_spiffs.h"
#endif

#ifdef CONFIG_ENABLE_LCD
    #include "eduboardLCD/eduboard2_lcd.h"
#endif
#ifdef CONFIG_ENABLE_FLASH
    #include "eduboardFlash/eduboard2_flash.h"
#endif

#ifdef CONFIG_ENABLE_SENSOR_TMP112
    #include "eduboardSensor/eduboard2_sensor.h"
#endif
#ifdef CONFIG_ENABLE_TOUCH_FT6236
    #include "eduboardTouch/eduboard2_touch.h"
#endif
#ifdef CONFIG_ENABLE_RTC
    #include "eduboardRTC/eduboard2_rtc.h"
#endif

#ifdef CONFIG_ENABLE_I2C
    #include "gpi2c.h"
#endif

#ifdef CONFIG_ENABLE_SPI
    #include "gpspi.h"
#endif

#include "eduboardInit/eduboard2_init.h"

