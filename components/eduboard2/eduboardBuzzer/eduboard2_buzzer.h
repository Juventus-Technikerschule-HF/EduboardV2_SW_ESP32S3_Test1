#pragma once
#include <stdlib.h>

#ifdef CONFIG_ENABLE_BUZZER
void eduboard_start_buzzer(uint16_t freq_hz, uint32_t length_ms);
void eduboard_stop_buzzer();
void eduboard_set_buzzer_volume(uint8_t volume);
void eduboard_init_buzzer();
#endif