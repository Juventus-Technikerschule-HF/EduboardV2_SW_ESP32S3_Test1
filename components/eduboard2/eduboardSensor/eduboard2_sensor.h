#pragma once

#include <esp_err.h>

float eduboard_get_val_tmp112(void);
void tmp112_poll(void);
void eduboard_init_tmp112(void);