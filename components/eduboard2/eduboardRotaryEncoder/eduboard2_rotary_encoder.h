#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "../eduboardButton/eduboard2_button.h"

button_state getEncoderButtonState(bool reset);
int32_t getEncoderRotation(bool reset);
void eduboard_init_rotary_encoder();