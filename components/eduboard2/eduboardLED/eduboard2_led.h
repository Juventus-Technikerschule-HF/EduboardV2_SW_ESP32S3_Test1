#pragma once
#include <stdlib.h>

#ifdef CONFIG_ENABLE_LED0
    #define LED0 0
#endif
#ifdef CONFIG_ENABLE_LED1
    #define LED1 1
#endif
#ifdef CONFIG_ENABLE_LED2
    #define LED2 2
#endif
#ifdef CONFIG_ENABLE_LED3
    #define LED3 3
#endif
#ifdef CONFIG_ENABLE_LED4
    #define LED4 4
#endif
#ifdef CONFIG_ENABLE_LED5
    #define LED5 5
#endif
#ifdef CONFIG_ENABLE_LED6
    #define LED6 6
#endif
#ifdef CONFIG_ENABLE_LED7
    #define LED7 7
#endif


void eduboard_set_led(uint8_t led_num, uint8_t level);
void eduboard_toggle_led(uint8_t led_num);
#ifdef CONFIG_ENABLE_PWMLED
void eduboard_set_pwmled(uint8_t red, uint8_t green, uint8_t blue);
#endif
#ifdef CONFIG_ENABLE_WS2812
void eduboard_set_ws2812(uint8_t red, uint8_t green, uint8_t blue);
#endif
void eduboard_init_leds();