#include "../../eduboard2.h"
#include "../eduboard2_sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include <string.h>

#define TAG "Sensor_TMP112"

/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/* TMP112 temperature sensor module for Chrome EC */

#define I2C_FREQ_HZ 100000 // Max 1MHz for esp-idf



#define C_TO_K(temp_c) ((temp_c) + 273)
//#define TMP112_I2C_ADDR_FLAGS (0x48)
#define TMP112_I2C_ADDR_FLAGS (0x48)
#define TMP112_REG_TEMP	0x00
#define TMP112_REG_CONF	0x01
#define TMP112_REG_HYST	0x02
#define TMP112_REG_MAX	0x03

#define TMP112_RESOLUTION 12
#define TMP112_SHIFT1 (16 - TMP112_RESOLUTION)
#define TMP112_SHIFT2 (TMP112_RESOLUTION - 8)

static float temp_val_local;

static esp_err_t read_register16(uint8_t reg, uint16_t *r)
{
    return i2c_master_write_read_device(0, TMP112_I2C_ADDR_FLAGS, &reg, 1, r, 2, portMAX_DELAY);
	//return i2c_dev_read_reg(&i2c_dev, reg, r, 2);
}
inline static esp_err_t write_register16(uint8_t reg_addr, uint16_t data)
{
    uint8_t senddata[3] = {reg_addr, (data >> 8), (data & 0xFF)};
	return i2c_master_write_to_device(0, TMP112_I2C_ADDR_FLAGS, &senddata, 3, portMAX_DELAY);
	//return i2c_dev_write_reg(&i2c_dev, reg_addr, &senddata, 3);
}


static int get_temp()
{
	esp_err_t err;
	uint16_t temp_raw = 0;
	err = read_register16(TMP112_REG_TEMP, &temp_raw);
	temp_raw = ((temp_raw << 8) | (temp_raw >> 8))>>4;
	ESP_LOGI(TAG, "Temp read: %4x", temp_raw);
    if (err != ESP_OK)
		return err;
	if((temp_raw & 0x0800) == 0x0000) {
		//Positive Number:
		temp_val_local = (0.0625 * temp_raw);
	} else {
		ESP_LOGI(TAG, "1: %4X", temp_raw);
		temp_raw = ((~temp_raw)+1)&0x0FFF;
		ESP_LOGI(TAG, "2: %4X", temp_raw);
		temp_val_local = -(0.0625 * temp_raw);
		ESP_LOGI(TAG, "3: %f", temp_val_local);

	}
	return ESP_OK;
}
float eduboard_get_val_tmp112(void)
{
	return temp_val_local;
}
void tmp112_poll(void)
{
	get_temp();
}

void eduboard_init_tmp112(void)
{
	int i2c_master_port = 0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_I2C_SDA,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0));
	
    uint16_t tmp;
	uint16_t set_mask, clr_mask;
	/* 12 bit mode */
	set_mask = (3 << 5);
	/* not oneshot mode */
	clr_mask = 0x80;
	//raw_read16(TMP112_REG_CONF, &tmp);
    read_register16(TMP112_REG_CONF, &tmp);
	ESP_LOGI(TAG, "TMP112 read: %4X", (int)tmp);
	//tmp = (tmp & ~clr_mask) | set_mask;
	tmp = 0x8062;
	//tmp = 0x6280;
	ESP_LOGI(TAG, "TMP112 writeback: %4X", (int)tmp);
	//raw_write16(TMP112_REG_CONF, (tmp & ~clr_mask) | set_mask);
    write_register16(TMP112_REG_CONF, tmp);
}