#include "../../eduboard2.h"
#include "../eduboard2_led.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG "Eduboard2_LEDDriver"

uint8_t led_pins[] = {GPIO_LED_0, GPIO_LED_1, GPIO_LED_2, GPIO_LED_3, GPIO_LED_4, GPIO_LED_5, GPIO_LED_6, GPIO_LED_7};

uint8_t checkLEDEnabled(uint8_t led_num) {
    switch(led_num) {
        case 0:
#ifndef CONFIG_ENABLE_LED0
            return 0;
#endif
        break;
        case 1:
#ifndef CONFIG_ENABLE_LED1
            return 0;
#endif
        break;
        case 2:
#ifndef CONFIG_ENABLE_LED2
            return 0;
#endif
        break;
        case 3:
#ifndef CONFIG_ENABLE_LED3
            return 0;
#endif
        break;
        case 4:
#ifndef CONFIG_ENABLE_LED4
            return 0;
#endif
        break;
        case 5:
#ifndef CONFIG_ENABLE_LED5
            return 0;
#endif
        break;
        case 6:
#ifndef CONFIG_ENABLE_LED6
            return 0;
#endif
        break;
        case 7:
#ifndef CONFIG_ENABLE_LED7
            return 0;
#endif
        break;
    }
    return 1;
}

void eduboard_set_led(uint8_t led_num, uint8_t level) {
    if(checkLEDEnabled(led_num) == 0) {
        ESP_LOGE(TAG, "LED%i not Enabled", led_num);
        return;
    }
    gpio_set_level(led_pins[led_num], level);
}
void eduboard_toggle_led(uint8_t led_num) {
    static uint8_t led_stat;
    if(checkLEDEnabled(led_num) == 0) {
        ESP_LOGE(TAG, "LED%i not Enabled", led_num);
        return;
    }
    if(((led_stat >> led_num) & 0x01) == 1) {
        led_stat &= (~(0x01 << led_num));
        gpio_set_level(led_pins[led_num], 0);
    } else {
        led_stat |= (0x01 << led_num);
        gpio_set_level(led_pins[led_num], 1);
    }
}
void eduboard_init_leds() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.pin_bit_mask = 0;
#ifdef CONFIG_ENABLE_LED0
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_0);
#endif
#ifdef CONFIG_ENABLE_LED1
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_1);
#endif
#ifdef CONFIG_ENABLE_LED2
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_2);
#endif
#ifdef CONFIG_ENABLE_LED3
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_3);
#endif
#ifdef CONFIG_ENABLE_LED4
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_4);
#endif
#ifdef CONFIG_ENABLE_LED5
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_5);
#endif
#ifdef CONFIG_ENABLE_LED6
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_6);
#endif
#ifdef CONFIG_ENABLE_LED7
    io_conf.pin_bit_mask |= (1ULL<<GPIO_LED_7);
#endif
    gpio_config(&io_conf);
}

