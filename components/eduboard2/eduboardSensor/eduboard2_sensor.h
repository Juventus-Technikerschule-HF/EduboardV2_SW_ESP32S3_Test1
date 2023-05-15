#pragma once

#include <esp_err.h>
#include <i2cdev.h>

int32_t eduboard_get_val_tmp112(void);
void eduboard_init_tmp112(void);