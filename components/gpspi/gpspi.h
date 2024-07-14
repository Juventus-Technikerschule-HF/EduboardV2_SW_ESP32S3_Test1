#pragma once

#include <driver/spi_master.h>

#define HOST_ID SPI2_HOST

bool gpspi_write_data(spi_device_handle_t* handle, uint8_t* data, uint32_t len);
bool gpspi_write_cmd_data(spi_device_handle_t* handle, uint8_t cmd, uint8_t* data, uint32_t len);
bool gpspi_read_data(spi_device_handle_t* handle, uint8_t cmd, uint8_t* data, uint32_t len);

void gpspi_init(spi_device_handle_t* dev, int pinMOSI, int pinMISO, int pinSCK, int pinCS, uint32_t frequency);