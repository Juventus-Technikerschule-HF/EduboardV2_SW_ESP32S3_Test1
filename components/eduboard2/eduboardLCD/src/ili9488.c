#include "../../eduboard2.h"

#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "ili9488.h"

#define TAG "ili9488"

#ifdef CONFIG_LCD_ILI9488

#define SPI_BUFFER_MAXLENGTH 2880	//1440
#define COLORS_MAXLENGTH SPI_BUFFER_MAXLENGTH/3

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
//static const int SPI_Frequency = SPI_MASTER_FREQ_10M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_20M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_26M;
static const int SPI_Frequency = SPI_MASTER_FREQ_40M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_80M;

/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

TFT_t * lcddevice = NULL;

void ili9488_spi_master_init(TFT_t * dev, int16_t PIN_MOSI, int16_t PIN_SCLK, int16_t PIN_CS, int16_t PIN_DC, int16_t PIN_RESET, int16_t PIN_BL)
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
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset(&devcfg, 0, sizeof(devcfg));
	devcfg.clock_speed_hz = SPI_Frequency;
	devcfg.queue_size = 4;
	devcfg.mode = 0;
	devcfg.flags = SPI_DEVICE_HALFDUPLEX;
	

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

bool ili9488_spi_write_cmd_data(uint8_t data) {
	spi_transaction_t SPITransaction;
	esp_err_t ret;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 8;
	SPITransaction.tx_buffer = &data;
	ret = spi_device_transmit( lcddevice->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK); 
	return true;
}
bool ili9488_spi_write_cmd(uint8_t cmd) {
	spi_transaction_t SPITransaction;
	esp_err_t ret;
	gpio_set_level( lcddevice->_dc, SPI_Command_Mode );
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = 8;
	SPITransaction.tx_buffer = &cmd;
	ret = spi_device_transmit( lcddevice->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK); 
	return true;
}


bool IRAM_ATTR ili9488_spi_write_colors(uint16_t *colors565, uint32_t length) {
	spi_transaction_t SPITransaction;
	esp_err_t ret;
	uint8_t colors666[length*3];
	for(uint32_t i = 0; i < length; i++) {
		colors666[(i*3)+0] = (0x1F&(colors565[i]>>11))*2;
		colors666[(i*3)+0] <<= 2;
		colors666[(i*3)+1] = (0x3F&(colors565[i]>>5));
		colors666[(i*3)+1] <<= 2;
		colors666[(i*3)+2] = (0x1F&(colors565[i]))*2;
		colors666[(i*3)+2] <<= 2;
	}
	if(length > COLORS_MAXLENGTH) {
		ESP_LOGE(TAG, "bufferlength too long!");
		return false;
	}
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = length * 3 * 8;
	SPITransaction.tx_buffer = &colors666[0];
	ret = spi_device_transmit( lcddevice->_SPIHandle, &SPITransaction );
	assert(ret==ESP_OK); 
	return true;
}
bool ili9488_lcd_setpos(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
	ili9488_spi_write_cmd(ILI9488_CMD_COLUMN_ADDRESS_SET);
	ili9488_spi_write_cmd_data(x1>>8);
	ili9488_spi_write_cmd_data(x1&0xff);
	ili9488_spi_write_cmd_data(x2>>8);
	ili9488_spi_write_cmd_data(x2&0xff);
	ili9488_spi_write_cmd(ILI9488_CMD_PAGE_ADDRESS_SET);
	ili9488_spi_write_cmd_data(y1>>8);
	ili9488_spi_write_cmd_data(y1&0xff);
	ili9488_spi_write_cmd_data(y2>>8);
	ili9488_spi_write_cmd_data(y2&0xff);

    ili9488_spi_write_cmd(ILI9488_CMD_MEMORY_WRITE);
	return true;
}

void ili9488_init(TFT_t * dev, int width, int height, int offsetx, int offsety)
{
	lcddevice = dev;
	lcddevice->_width = width;
	lcddevice->_height = height;
	lcddevice->_offsetx = offsetx;
	lcddevice->_offsety = offsety;
	lcddevice->_font_direction = DIRECTION0;
	lcddevice->_font_fill = false;
	lcddevice->_font_underline = false;

	ESP_LOGI(TAG, "ILI9488 initialization.\n");

  	ili9488_spi_write_cmd(ILI9488_CMD_SOFTWARE_RESET);  //Exit Sleep
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ili9488_spi_write_cmd(ILI9488_CMD_POSITIVE_GAMMA_CORRECTION);
	ili9488_spi_write_cmd_data(0x00);
	ili9488_spi_write_cmd_data(0x03);
	ili9488_spi_write_cmd_data(0x09);
	ili9488_spi_write_cmd_data(0x08);
	ili9488_spi_write_cmd_data(0x16);
	ili9488_spi_write_cmd_data(0x0A);
	ili9488_spi_write_cmd_data(0x3F);
	ili9488_spi_write_cmd_data(0x78);
	ili9488_spi_write_cmd_data(0x4C);
	ili9488_spi_write_cmd_data(0x09);
	ili9488_spi_write_cmd_data(0x0A);
	ili9488_spi_write_cmd_data(0x08);
	ili9488_spi_write_cmd_data(0x16);
	ili9488_spi_write_cmd_data(0x1A);
	ili9488_spi_write_cmd_data(0x0F);
	

	ili9488_spi_write_cmd(ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION);
	ili9488_spi_write_cmd_data(0x00);
	ili9488_spi_write_cmd_data(0x16);
	ili9488_spi_write_cmd_data(0x19);
	ili9488_spi_write_cmd_data(0x03);
	ili9488_spi_write_cmd_data(0x0F);
	ili9488_spi_write_cmd_data(0x05);
	ili9488_spi_write_cmd_data(0x32);
	ili9488_spi_write_cmd_data(0x45);
	ili9488_spi_write_cmd_data(0x46);
	ili9488_spi_write_cmd_data(0x04);
	ili9488_spi_write_cmd_data(0x0E);
	ili9488_spi_write_cmd_data(0x0D);
	ili9488_spi_write_cmd_data(0x35);
	ili9488_spi_write_cmd_data(0x37);
	ili9488_spi_write_cmd_data(0x0F);
	
	
	 
	ili9488_spi_write_cmd(ILI9488_CMD_POWER_CONTROL_1);      //Power Control 1 
	ili9488_spi_write_cmd_data(0x17);    //Vreg1out 
	ili9488_spi_write_cmd_data(0x15);    //Verg2out 
	 
	ili9488_spi_write_cmd(ILI9488_CMD_POWER_CONTROL_2);      //Power Control 2     
	ili9488_spi_write_cmd_data(0x41);    //VGH,VGL 
	 
	ili9488_spi_write_cmd(ILI9488_CMD_VCOM_CONTROL_1);      //Power Control 3 
	ili9488_spi_write_cmd_data(0x00); 
	ili9488_spi_write_cmd_data(0x12);    //Vcom 
	ili9488_spi_write_cmd_data(0x80);
	 
	ili9488_spi_write_cmd(ILI9488_CMD_MEMORY_ACCESS_CONTROL);      //Memory Access 
	ili9488_spi_write_cmd_data(0x48); 
	 
	ili9488_spi_write_cmd(ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET);      // Interface Pixel Format 
	ili9488_spi_write_cmd_data(0x66); 	  //18 bit    
	//ili9488_spi_write_cmd_data(0x55); 	  //16 bit    
	 
	ili9488_spi_write_cmd(ILI9488_CMD_INTERFACE_MODE_CONTROL);      // Interface Mode Control 
	ili9488_spi_write_cmd_data(0x80);     			 //SDO NOT USE
	 
	ili9488_spi_write_cmd(ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL);      //Frame rate 
	ili9488_spi_write_cmd_data(0xA0);    //60Hz 
	 
	ili9488_spi_write_cmd(ILI9488_CMD_DISPLAY_INVERSION_CONTROL);      //Display Inversion Control 
	ili9488_spi_write_cmd_data(0x02);    //2-dot 
	 
	ili9488_spi_write_cmd(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL);      //Display Function Control  RGB/MCU Interface Control 
	ili9488_spi_write_cmd_data(0x02);    //MCU 
	ili9488_spi_write_cmd_data(0x02);    //Source,Gate scan dieection 
	  
	ili9488_spi_write_cmd(ILI9488_CMD_SET_IMAGE_FUNCTION);      // Set Image Functio
	ili9488_spi_write_cmd_data(0x00);    // Disable 24 bit data
	 
	ili9488_spi_write_cmd(ILI9488_CMD_ADJUST_CONTROL_3);      // Adjust Control 
	ili9488_spi_write_cmd_data(0xA9);     
	ili9488_spi_write_cmd_data(0x51);     
	ili9488_spi_write_cmd_data(0x2C);     
	ili9488_spi_write_cmd_data(0x82);    // D7 stream, loose 

	ili9488_spi_write_cmd(ILI9488_CMD_SLEEP_OUT); //Sleep out
	vTaskDelay(120 / portTICK_PERIOD_MS);
	ili9488_spi_write_cmd(ILI9488_CMD_DISPLAY_ON);
  
	///Enable backlight
	ESP_LOGI(TAG, "Enable backlight.\n");
	if(lcddevice->_bl >= 0) {
		gpio_set_level( lcddevice->_bl, 1 );
	}
}

void ili9488_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= lcddevice->_width) return;
	if (y >= lcddevice->_height) return;

	
	uint16_t _x = x + lcddevice->_offsetx;
	uint16_t _y = y + lcddevice->_offsety;
	
	ili9488_lcd_setpos(_x,_x,_y,_y);
	uint16_t colors[1] = {color};
	ili9488_spi_write_colors(&colors[0], 1);
}

void ili9488_DrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors) {
	if (x+size > lcddevice->_width) return;
	if (y >= lcddevice->_height) return;
	
	uint16_t _x1 = x + lcddevice->_offsetx;
	uint16_t _x2 = _x1 + size;
	uint16_t _y1 = y + lcddevice->_offsety;
	uint16_t _y2 = _y1;
	//uint16_t i = 0;
	ili9488_lcd_setpos(_x1, _x2, _y1, _y2);
	if(size <= COLORS_MAXLENGTH) {
		ili9488_spi_write_colors(&colors[0], size);
	} else {
		ESP_LOGE(TAG, "Size of Colors array too large");
	}
}

void ili9488_DrawMultiLines(uint16_t start_y, uint16_t lines, uint16_t * colors) {
	uint16_t _x1 = 0;
	uint16_t _x2 = lcddevice->_width-1;
	uint16_t _y1 = start_y; 
	uint16_t _y2 = start_y + lines - 1;
	ili9488_lcd_setpos(_x1, _x2, _y1, _y2);
	#define LINES_PER_UPDATE 3 //Maxed by Transfer-Buffer of SPI Unit
	for(int y = 0; y < lines; y+= LINES_PER_UPDATE) {
		uint32_t pos = y*lcddevice->_width;
		ili9488_spi_write_colors(&colors[pos], lcddevice->_width*LINES_PER_UPDATE);
	}
}

void ili9488_DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	if (x1 >= lcddevice->_width) return;
	if (x2 >= lcddevice->_width) x2=lcddevice->_width-1;
	if (y1 >= lcddevice->_height) return;
	if (y2 >= lcddevice->_height) y2=lcddevice->_height-1;

	uint16_t _x1 = x1 + lcddevice->_offsetx;
	uint16_t _x2 = x2 + lcddevice->_offsetx;
	uint16_t _y1 = y1 + lcddevice->_offsety;
	uint16_t _y2 = y2 + lcddevice->_offsety;
	ili9488_lcd_setpos(_x1, _x2, _y1, _y2);
	uint32_t size = (x2-x1+1) * (y2-y1+1);
	uint16_t colors[COLORS_MAXLENGTH];
	
	for(uint16_t i = 0; i < COLORS_MAXLENGTH; i++) {
		colors[i] = color;
	}
	while(size > COLORS_MAXLENGTH) {
		ili9488_spi_write_colors(&colors[0], COLORS_MAXLENGTH);
		size -= COLORS_MAXLENGTH;
	}
	ili9488_spi_write_colors(&colors[0], size);
}

// Display OFF
void ili9488_DisplayOff() {
	ili9488_spi_write_cmd(ILI9488_CMD_DISPLAY_OFF);	//Display off
}
 
// Display ON
void ili9488_DisplayOn() {
	ili9488_spi_write_cmd(ILI9488_CMD_DISPLAY_ON);	//Display on
}

void ili9488_InversionOff() {
	ili9488_spi_write_cmd(ILI9488_CMD_DISP_INVERSION_OFF);	//Display Inversion Off
}

// Display Inversion On
void ili9488_InversionOn() {
	ili9488_spi_write_cmd(ILI9488_CMD_DISP_INVERSION_ON);	//Display Inversion On
}

#endif