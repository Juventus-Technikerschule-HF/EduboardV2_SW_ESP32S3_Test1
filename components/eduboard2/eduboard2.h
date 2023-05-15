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

#ifdef CONFIG_ENABLE_ANALOG
    #include "eduboardAnalog/eduboard2_analog.h"
#endif

#ifdef CONFIG_ENABLE_LCD
    #include "eduboardLCD/eduboard2_lcd.h"
#endif