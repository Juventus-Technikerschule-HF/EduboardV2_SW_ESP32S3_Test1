idf_component_register(SRCS                 . 
                                            ./eduboardLED/src/eduboard2_led_esp32_s3.c 
                                            ./eduboardBuzzer/src/eduboard2_buzzer_esp32_s3.c
                                            ./eduboardButton/src/eduboard2_button_esp32_s3.c
                                            ./eduboardAnalog/src/eduboard2_analog_esp32_s3.c
                                            ./eduboardLCD/src/eduboard2_lcd_esp32_s3.c
                                            ./eduboardLCD/src/st7789.c
                        INCLUDE_DIRS        . 
                                            eduboardLED 
                                            eduboardBuzzer 
                                            eduboardButton
                                            eduboardAnalog
                                            eduboardLCD
                        REQUIRES            driver 
                                            )
