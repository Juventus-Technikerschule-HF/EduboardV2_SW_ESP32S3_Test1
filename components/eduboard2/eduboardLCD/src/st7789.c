#include "../../eduboard2.h"

#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "st7789.h"

#define TAG "st7789"

#ifdef CONFIG_LCD_ST7789

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
//static const int SPI_Frequency = SPI_MASTER_FREQ_20M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_26M;
static const int SPI_Frequency = SPI_MASTER_FREQ_40M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_80M;

TFT_t * lcddevice = NULL;

void st7789_spi_master_init(TFT_t * dev, int16_t PIN_MOSI, int16_t PIN_SCLK, int16_t PIN_CS, int16_t PIN_DC, int16_t PIN_RESET, int16_t PIN_BL)
{
	esp_err_t ret;

	//ESP_LOGI(TAG, "GPIO_CS=%d",GPIO_CS);
	if ( PIN_CS >= 0 ) {
		//gpio_pad_select_gpio( GPIO_CS );
		gpio_reset_pin( PIN_CS );
		gpio_set_direction( PIN_CS, GPIO_MODE_OUTPUT );
		gpio_set_level( PIN_CS, 0 );
	}

	//ESP_LOGI(TAG, "GPIO_DC=%d",GPIO_DC);
	//gpio_pad_select_gpio( GPIO_DC );
	gpio_reset_pin( PIN_DC );
	gpio_set_direction( PIN_DC, GPIO_MODE_OUTPUT );
	gpio_set_level( PIN_DC, 0 );

	//ESP_LOGI(TAG, "GPIO_RESET=%d",GPIO_RESET);
	if ( PIN_RESET >= 0 ) {
		//gpio_pad_select_gpio( GPIO_RESET );
		gpio_reset_pin( PIN_RESET );
		gpio_set_direction( PIN_RESET, GPIO_MODE_OUTPUT );
		gpio_set_level( PIN_RESET, 1 );
		delayMS(50);
		gpio_set_level( PIN_RESET, 0 );
		delayMS(50);
		gpio_set_level( PIN_RESET, 1 );
		delayMS(50);
	}

	//ESP_LOGI(TAG, "GPIO_BL=%d",GPIO_BL);
	if ( PIN_BL >= 0 ) {
		//gpio_pad_select_gpio(GPIO_BL);
		gpio_reset_pin(PIN_BL);
		gpio_set_direction( PIN_BL, GPIO_MODE_OUTPUT );
		gpio_set_level( PIN_BL, 0 );
	}

	//ESP_LOGI(TAG, "GPIO_MOSI=%d",GPIO_MOSI);
	//ESP_LOGI(TAG, "GPIO_SCLK=%d",GPIO_SCLK);
	spi_bus_config_t buscfg = {
		.mosi_io_num = PIN_MOSI,
		.miso_io_num = -1,
		.sclk_io_num = PIN_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 0,
		.flags = 0
	};

	ret = spi_bus_initialize( HOST_ID, &buscfg, SPI_DMA_CH_AUTO );
	//ESP_LOGD(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset(&devcfg, 0, sizeof(devcfg));
	devcfg.clock_speed_hz = SPI_Frequency;
	devcfg.queue_size = 7;
	devcfg.mode = 2;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

	if ( PIN_CS >= 0 ) {
		devcfg.spics_io_num = PIN_CS;
	} else {
		devcfg.spics_io_num = -1;
	}
	
	spi_device_handle_t handle;
	ret = spi_bus_add_device( HOST_ID, &devcfg, &handle);
	//ESP_LOGD(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
	dev->_dc = PIN_DC;
	dev->_bl = PIN_BL;
	dev->_SPIHandle = handle;
}


bool st7789_spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength)
{
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Data;
#if 1
		ret = spi_device_transmit( SPIHandle, &SPITransaction );
#else
		ret = spi_device_polling_transmit( SPIHandle, &SPITransaction );
#endif
		assert(ret==ESP_OK); 
	}

	return true;
}

bool st7789_spi_master_write_command(uint8_t cmd)
{
	static uint8_t Byte = 0;
	Byte = cmd;
	gpio_set_level( lcddevice->_dc, SPI_Command_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, &Byte, 1 );
}

bool st7789_spi_master_write_data_byte(uint8_t data)
{
	static uint8_t Byte = 0;
	Byte = data;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, &Byte, 1 );
}


bool st7789_spi_master_write_data_word(uint16_t data)
{
	static uint8_t Byte[2];
	Byte[0] = (data >> 8) & 0xFF;
	Byte[1] = data & 0xFF;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, Byte, 2);
}

bool st7789_spi_master_write_addr(uint16_t addr1, uint16_t addr2)
{
	static uint8_t Byte[4];
	Byte[0] = (addr1 >> 8) & 0xFF;
	Byte[1] = addr1 & 0xFF;
	Byte[2] = (addr2 >> 8) & 0xFF;
	Byte[3] = addr2 & 0xFF;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, Byte, 4);
}

bool st7789_spi_master_write_color(uint16_t color, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (color >> 8) & 0xFF;
		Byte[index++] = color & 0xFF;
	}
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, Byte, size*2);
}

// Add 202001
bool st7789_spi_master_write_colors(uint16_t * colors, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (colors[i] >> 8) & 0xFF;
		Byte[index++] = colors[i] & 0xFF;
	}
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return st7789_spi_master_write_byte( lcddevice->_SPIHandle, Byte, size*2);
}

void st7789_init(TFT_t * dev, int width, int height, int offsetx, int offsety)
{
	lcddevice = dev;
	lcddevice->_width = width;
	lcddevice->_height = height;
	lcddevice->_offsetx = offsetx;
	lcddevice->_offsety = offsety;
	lcddevice->_font_direction = DIRECTION0;
	lcddevice->_font_fill = false;
	lcddevice->_font_underline = false;

	st7789_spi_master_write_command(0x01);	//Software Reset
	delayMS(150);

	st7789_spi_master_write_command(0x11);	//Sleep Out
	delayMS(255);
	
	st7789_spi_master_write_command(0x3A);	//Interface Pixel Format
	st7789_spi_master_write_data_byte(0x55);
	delayMS(10);
	
	st7789_spi_master_write_command(0x36);	//Memory Data Access Control
	st7789_spi_master_write_data_byte(0x00);

	st7789_spi_master_write_command(0x2A);	//Column Address Set
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0xF0);

	st7789_spi_master_write_command(0x2B);	//Row Address Set
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0x00);
	st7789_spi_master_write_data_byte(0xF0);

	st7789_spi_master_write_command(0x21);	//Display Inversion On
	delayMS(10);

	st7789_spi_master_write_command(0x13);	//Normal Display Mode On
	delayMS(10);

	st7789_spi_master_write_command(0x29);	//Display ON
	delayMS(255);

	if(lcddevice->_bl >= 0) {
		gpio_set_level( lcddevice->_bl, 1 );
	}
}

void st7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= lcddevice->_width) return;
	if (y >= lcddevice->_height) return;

	
	uint16_t _x = x + lcddevice->_offsetx;
	uint16_t _y = y + lcddevice->_offsety;
	st7789_spi_master_write_command(0x2A);	// set column(x) address
	st7789_spi_master_write_addr(_x, _x);
	st7789_spi_master_write_command(0x2B);	// set Page(y) address
	st7789_spi_master_write_addr(_y, _y);
	st7789_spi_master_write_command(0x2C);	//	Memory Write
	st7789_spi_master_write_data_word(color);
}

void st7789_DrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors) {
	if (x+size > lcddevice->_width) return;
	if (y >= lcddevice->_height) return;
	
	uint16_t _x1 = x + lcddevice->_offsetx;
	uint16_t _x2 = _x1 + size;
	uint16_t _y1 = y + lcddevice->_offsety;
	uint16_t _y2 = _y1;
	st7789_spi_master_write_command(0x2A);	// set column(x) address
	st7789_spi_master_write_addr(_x1, _x2);
	st7789_spi_master_write_command(0x2B);	// set Page(y) address
	st7789_spi_master_write_addr(_y1, _y2);
	st7789_spi_master_write_command(0x2C);	//	Memory Write
	st7789_spi_master_write_colors(colors, size);
}

void st7789_DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	if (x1 >= lcddevice->_width) return;
	if (x2 >= lcddevice->_width) x2=lcddevice->_width-1;
	if (y1 >= lcddevice->_height) return;
	if (y2 >= lcddevice->_height) y2=lcddevice->_height-1;

	uint16_t _x1 = x1 + lcddevice->_offsetx;
	uint16_t _x2 = x2 + lcddevice->_offsetx;
	uint16_t _y1 = y1 + lcddevice->_offsety;
	uint16_t _y2 = y2 + lcddevice->_offsety;
	st7789_spi_master_write_command(0x2A);	// set column(x) address
	st7789_spi_master_write_addr(_x1, _x2);
	st7789_spi_master_write_command(0x2B);	// set Page(y) address
	st7789_spi_master_write_addr(_y1, _y2);
	st7789_spi_master_write_command(0x2C);	//	Memory Write
	for(int i=_x1;i<=_x2;i++){
		uint16_t size = _y2-_y1+1;
		st7789_spi_master_write_color(color, size);
	}
}

// Display OFF
void st7789_DisplayOff() {
	st7789_spi_master_write_command(0x28);	//Display off
}
 
// Display ON
void st7789_DisplayOn() {
	st7789_spi_master_write_command(0x29);	//Display on
}

void st7789_InversionOff() {
	st7789_spi_master_write_command(0x20);	//Display Inversion Off
}

// Display Inversion On
void st7789_InversionOn() {
	st7789_spi_master_write_command(0x21);	//Display Inversion On
}

#endif