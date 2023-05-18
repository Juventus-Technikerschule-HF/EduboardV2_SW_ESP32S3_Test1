idf_component_register(SRCS                 . 
                                            ./eduboardLED/src/eduboard2_led_esp32_s3.c 
                                            ./eduboardBuzzer/src/eduboard2_buzzer_esp32_s3.c
                                            ./eduboardButton/src/eduboard2_button_esp32_s3.c
                                            ./eduboardAnalog/src/eduboard2_analog_esp32_s3.c
                                            ./eduboardLCD/src/eduboard2_lcd_esp32_s3.c
                                            ./eduboardLCD/src/st7789.c
                                            ./eduboardSensor/src/eduboard2_tmp112_esp32_s3.c
                        INCLUDE_DIRS        . 
                                            eduboardLED 
                                            eduboardBuzzer 
                                            eduboardButton
                                            eduboardAnalog
                                            eduboardLCD
                                            eduboardSensor
                        REQUIRES            driver 
                                            led_strip_encoder
                                            esp_adc
                                            gpi2c)
