#include "../../eduboard2.h"
#include "../eduboard2_sensor.h"
#include "esp_log.h"

#include <string.h>

/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/* TMP112 temperature sensor module for Chrome EC */

#define I2C_FREQ_HZ 1000000 // Max 1MHz for esp-idf



#define C_TO_K(temp_c) ((temp_c) + 273)
#define TMP112_I2C_ADDR_FLAGS (0x48)
#define TMP112_REG_TEMP	0x00
#define TMP112_REG_CONF	0x01
#define TMP112_REG_HYST	0x02
#define TMP112_REG_MAX	0x03

#define TMP112_RESOLUTION 12
#define TMP112_SHIFT1 (16 - TMP112_RESOLUTION)
#define TMP112_SHIFT2 (TMP112_RESOLUTION - 8)

i2c_dev_t i2c_dev;

static int32_t temp_val_local;
// static int raw_read16(const int offset, int *data_ptr)
// {
// 	return i2c_read16(I2C_PORT_THERMAL, TMP112_I2C_ADDR_FLAGS,
// 			  offset, data_ptr);
// }
// static int raw_write16(const int offset, int data)
// {
// 	return i2c_write16(I2C_PORT_THERMAL, TMP112_I2C_ADDR_FLAGS,
// 			   offset, data);
// }
#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)
static esp_err_t read_register16(i2c_dev_t *dev, uint8_t reg, uint16_t *r)
{
    uint8_t d[] = { 0, 0 };
    CHECK(i2c_dev_read_reg(dev, reg, d, 2));
    *r = d[0] | (d[1] << 8);
    return ESP_OK;
}
inline static esp_err_t write_register16(i2c_dev_t *dev, uint8_t addr, uint16_t value)
{
    return i2c_dev_write_reg(dev, addr, &value, 2);
}


static int get_temp(int *temp_ptr)
{
	int rv;
	int temp_raw = 0;
	//rv = raw_read16(TMP112_REG_TEMP, &temp_raw);
    I2C_DEV_TAKE_MUTEX(&i2c_dev);
    rv = read_register16(&i2c_dev, TMP112_REG_TEMP, &temp_raw);
    I2C_DEV_GIVE_MUTEX(&i2c_dev);
	if (rv < 0)
		return rv;
	*temp_ptr = (int)(int16_t)temp_raw;
	return ESP_OK;
}
static inline int tmp112_reg_to_c(int16_t reg)
{
	int tmp;
	tmp = (((reg >> TMP112_SHIFT1) * 1000 ) >> TMP112_SHIFT2);
	return tmp / 1000;
}
int32_t eduboard_get_val_tmp112(void)
{
	return temp_val_local;
}
static void tmp112_poll(void)
{
	int temp_c = 0;
	if (get_temp(&temp_c) == ESP_OK)
		temp_val_local = C_TO_K(tmp112_reg_to_c(temp_c));
}

void eduboard_init_tmp112(void)
{
	i2c_dev.port = 0;
    i2c_dev.addr = TMP112_I2C_ADDR_FLAGS;
    i2c_dev.cfg.sda_io_num = GPIO_I2C_SDA;
    i2c_dev.cfg.scl_io_num = GPIO_I2C_SCL;
    i2c_dev.cfg.master.clk_speed = I2C_FREQ_HZ;
    ESP_ERROR_CHECK(i2c_dev_create_mutex(&i2c_dev));

    I2C_DEV_TAKE_MUTEX(&i2c_dev);
    
    uint16_t tmp;
	uint16_t set_mask, clr_mask;
	/* 12 bit mode */
	set_mask = (3 << 5);
	/* not oneshot mode */
	clr_mask = 0x80;
	//raw_read16(TMP112_REG_CONF, &tmp);
    read_register16(&i2c_dev, TMP112_REG_CONF, &tmp);
	//raw_write16(TMP112_REG_CONF, (tmp & ~clr_mask) | set_mask);
    write_register16(&i2c_dev, TMP112_REG_CONF, (tmp & ~clr_mask) | set_mask);

    I2C_DEV_GIVE_MUTEX(&i2c_dev);
}