idf_component_register(SRCS                 . 
                                            ./eduboardLED/src/eduboard2_led_esp32_s3.c 
                                            ./eduboardBuzzer/src/eduboard2_buzzer_esp32_s3.c
                                            ./eduboardButton/src/eduboard2_button_esp32_s3.c
                                            ./eduboardAnalog/src/eduboard2_analog_esp32_s3.c
                                            ./eduboardLCD/src/eduboard2_lcd_esp32_s3.c
                                            ./eduboardLCD/src/lcdDriver.c
                                            ./eduboardLCD/src/st7789.c
                                            ./eduboardLCD/src/ili9488.c
                                            ./eduboardLCD/src/decode_png.c
                                            ./eduboardLCD/src/pngle.c
                                            ./eduboardLCD/src/decode_jpeg.c
                                            ./eduboardSensor/src/eduboard2_tmp112_esp32_s3.c
                                            ./eduboardTouch/src/eduboard2_ft6236_esp32_s3.c
                                            ./eduboardSpiffs/src/fontx.c
                                            ./eduboardSpiffs/src/eduboard2_spiffs.c
                                            ./eduboardInit/src/eduboard2_init.c
                        INCLUDE_DIRS        . 
                                            eduboardLED 
                                            eduboardBuzzer 
                                            eduboardButton
                                            eduboardAnalog
                                            eduboardLCD
                                            eduboardSensor
                                            eduboardTouch
                                            eduboardSpiffs
                                            eduboardInit
                        REQUIRES            driver 
                                            led_strip_encoder
                                            esp_adc
                                            gpi2c
                                            vfs
                                            spiffs)
