#pragma once
/*----------------------------------------------------------------------------------*/
/* Edubard2 Defines                                                                 */
/* Martin Burger                                                                    */
/* Juventus Technikerschule                                                         */
/*----------------------------------------------------------------------------------*/

#ifdef EDUBOARD_CPU_BOARD_ESP32_S3

    #define GPIO_LED_0      4
    #define GPIO_LED_1      5
    #define GPIO_LED_2      6
    #define GPIO_LED_3      7
    #define GPIO_LED_4      38
    #define GPIO_LED_5      35
    #define GPIO_LED_6      37
    #define GPIO_LED_7      36

    #define GPIO_BUZZER     46

    #define GPIO_SW_0       21
    #define GPIO_SW_1       47
    #define GPIO_SW_2       48
    #define GPIO_SW_3       45

    #define GPIO_AN_0       9

    #define GPIO_MISO       12
    #define GPIO_MOSI       13
    #define GPIO_SCK        14

    #define GPIO_UART1_TX   17
    #define GPIO_UART1_RX   18

    #define GPIO_I2C_SDA    2
    #define GPIO_I2C_SCL    1

    #define GPIO_LCD_CS     11
    #define GPIO_LCD_DC     16

    #define GPIO_GENERAL_RESET  8

#endif

#ifdef CONFIG_ENABLE_LED0 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED1 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED2 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED3 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED4 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED5 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED6 
    #define CONFIG_ENABLE_LED
#endif
#ifdef CONFIG_ENABLE_LED7 
    #define CONFIG_ENABLE_LED
#endif

#ifdef CONFIG_ENABLE_SW0
    #define CONFIG_ENABLE_BUTTONS
#endif
#ifdef CONFIG_ENABLE_SW1
    #define CONFIG_ENABLE_BUTTONS
#endif
#ifdef CONFIG_ENABLE_SW2
    #define CONFIG_ENABLE_BUTTONS
#endif
#ifdef CONFIG_ENABLE_SW3
    #define CONFIG_ENABLE_BUTTONS
#endif

#ifdef CONFIG_ENABLE_AN0
    #define CONFIG_ENABLE_ANALOG
#endif
