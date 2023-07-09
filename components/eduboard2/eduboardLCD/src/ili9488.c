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

#define ILI9488_SPI_SENDBUFFER_SIZE	1024

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
static const int SPI_Frequency = SPI_MASTER_FREQ_8M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_20M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_26M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_40M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_80M;

/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

TFT_t * lcddevice = NULL;

/* Internal API's */
//static void IRAM_ATTR queue_ready (spi_transaction_t *trans);

/* Internal Var's */
static volatile bool spi_trans_in_progress;
static volatile bool spi_color_sent;

//static void IRAM_ATTR queue_ready (spi_transaction_t *trans);
// callback for SPI post action.
// static void IRAM_ATTR queue_ready (spi_transaction_t *trans)
// {
//     spi_trans_in_progress = false;
// }

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
	devcfg.queue_size = 7;
	//devcfg.queue_size = 1;
	//devcfg.mode = 0;
	devcfg.mode = 2;
	//devcfg.flags = SPI_DEVICE_HALFDUPLEX;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;
	//devcfg.post_cb = queue_ready;
	//devcfg.pre_cb = NULL;

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


bool ili9488_spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength)
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

bool ili9488_spi_master_write_command(uint8_t cmd)
{
	static uint8_t Byte = 0;
	Byte = cmd;
	gpio_set_level( lcddevice->_dc, SPI_Command_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, &Byte, 1 );
}

bool ili9488_spi_master_write_data_byte(uint8_t data)
{
	static uint8_t Byte = 0;
	Byte = data;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, &Byte, 1 );
}

bool ili9488_spi_master_write_data_bytes(const uint8_t* Data, size_t DataLength)
{	
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, Data, DataLength );
}


bool ili9488_spi_master_write_data_word(uint16_t data)
{
	static uint8_t Byte[2];
	Byte[0] = (data >> 8) & 0xFF;
	Byte[1] = data & 0xFF;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, Byte, 2);
}

bool ili9488_spi_master_write_addr(uint16_t addr1, uint16_t addr2)
{
	static uint8_t Byte[4];
	Byte[0] = (addr1 >> 8) & 0xFF;
	Byte[1] = addr1 & 0xFF;
	Byte[2] = (addr2 >> 8) & 0xFF;
	Byte[3] = addr2 & 0xFF;
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, Byte, 4);
}

bool ili9488_spi_master_write_color(uint16_t color, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (color >> 8) & 0xFF;
		Byte[index++] = color & 0xFF;
	}
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, Byte, size*2);
}

// Add 202001
bool ili9488_spi_master_write_colors(uint16_t * colors, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (colors[i] >> 8) & 0xFF;
		Byte[index++] = colors[i] & 0xFF;
	}
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	return ili9488_spi_master_write_byte( lcddevice->_SPIHandle, Byte, size*2);
}


void spiSendData(uint8_t *data, uint16_t length)
{
    // if (length == 0) return;           //no need to send anything

    // //while(spi_trans_in_progress);       //Usually doesn't blocks for long.

    // spi_transaction_t t;
    // memset(&t, 0, sizeof(t));       	//Zero out the transaction
    // t.length = length * 8;              //Length is in bytes, transaction length is in bits.
    // t.tx_buffer = data;               	//Data
    // //spi_device_queue_trans(lcddevice->_SPIHandle, &t, portMAX_DELAY);
	// spi_device_transmit(lcddevice->_SPIHandle, &t);
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( length > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = length * 8;
		SPITransaction.tx_buffer = data;
		ret = spi_device_transmit( lcddevice->_SPIHandle, &SPITransaction );
		assert(ret==ESP_OK); 
	}
}

void spiSendColor(uint8_t * colorMap, uint16_t length)
{
    // if (length == 0) return;           //no need to send anything

    // //while(spi_trans_in_progress);       //Usually doesn't blocks for long, Check and add a NB Delay if required.

    // spi_transaction_t t;
    // memset(&t, 0, sizeof(t));           //Zero out the transaction
    // t.length = length * 8;              //Length is in bytes, transaction length is in bits.
    // t.tx_buffer = colorMap;                 //Data
    // spi_device_transmit(lcddevice->_SPIHandle, &t);

	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( length > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = length * 8;
		SPITransaction.tx_buffer = colorMap;
		ret = spi_device_transmit( lcddevice->_SPIHandle, &SPITransaction );
		assert(ret==ESP_OK); 
	}
}

static void ili_send_cmd(uint8_t cmd)
{
	gpio_set_level( lcddevice->_dc, SPI_Command_Mode );
	spiSendData(&cmd, 1);
}

static void ili_send_data(void * data, uint16_t length)
{
	gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
	spiSendData(data, length);
}

static void ili_send_color(void * data, uint16_t length)
{
    gpio_set_level( lcddevice->_dc, SPI_Data_Mode );
    spiSendColor(data, length);
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

	static DRAM_ATTR lcd_init_cmd_t ili_init_cmds[]={
		{ILI9488_CMD_POSITIVE_GAMMA_CORRECTION, {0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F}, 15},
		{ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION, {0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F,}, 15},
    	{ILI9488_CMD_POWER_CONTROL_1,           {0x17, 0x15}, 2},
		{ILI9488_CMD_POWER_CONTROL_2,           {0x41}, 1},
		{ILI9488_CMD_VCOM_CONTROL_1,            {0x00, 0x12, 0x80}, 3},
    	{ILI9488_CMD_MEMORY_ACCESS_CONTROL,     {TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_MX}, 1},			  // Memory Access Control
		//{ILI9488_CMD_MEMORY_ACCESS_CONTROL,     {0x20 | 0x00}, 1},			  // Memory Access Control
		//{ILI9488_CMD_MEMORY_ACCESS_CONTROL,     {0x00}, 1},			  // Memory Access Control
		//{ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET,   {0x66}, 1},			  // Pixel Interface Format 18 bit colour for SPI
		{ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET,   {0x55}, 1},			  // Pixel Interface Format 16 bit colour
		{ILI9488_CMD_INTERFACE_MODE_CONTROL,    {0x80}, 1},       // Interface Mode Control
		{ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL, {0xA0}, 1},       // Frame Rate Control
		{ILI9488_CMD_DISPLAY_INVERSION_CONTROL, {0x02}, 1},       // Display Inversion Control
		{ILI9488_CMD_DISPLAY_FUNCTION_CONTROL,  {0x02, 0x02, 0x3B}, 3},
		//{ILI9488_CMD_DISPLAY_FUNCTION_CONTROL,  {0x02, 0x02}, 2},
		{ILI9488_CMD_ENTRY_MODE_SET,            {0xC6}, 1},
		{ILI9488_CMD_SET_IMAGE_FUNCTION,		{0x00}, 1},
		//{ILI9488_CMD_WRITE_CTRL_DISPLAY, 		{0x28}, 1},
		//{ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS, 	{0x10}, 1},
    	{ILI9488_CMD_ADJUST_CONTROL_3,          {0xA9, 0x51, 0x2C, 0x82}, 4},
		//{ILI9488_CMD_ADJUST_CONTROL_3,          {0xA9, 0x51, 0x2C, 0x02}, 4},
		{ILI9488_CMD_SLEEP_OUT,                 {0}, 0x80},
		{ILI9488_CMD_DISPLAY_ON,                {0}, 0x80},
		{0, {0}, 0xff},
	};

	ESP_LOGI(TAG, "ILI9488 initialization.\n");

  	ili9488_spi_master_write_command(ILI9488_CMD_SOFTWARE_RESET);  //Exit Sleep
	vTaskDelay(100 / portTICK_PERIOD_MS);

 	//Send all the commands
	uint16_t cmd = 0;
	while (ili_init_cmds[cmd].databytes!=0xff) {
		ili9488_spi_master_write_command(ili_init_cmds[cmd].cmd);
		ili9488_spi_master_write_data_bytes(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes&0x1F);
		if (ili_init_cmds[cmd].databytes & 0x80) {
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		cmd++;
	}
  
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
	uint8_t data[4];
	/*Column addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_COLUMN_ADDRESS_SET);
	data[0] = (_x >> 8) & 0xFF;
	data[1] = _x & 0xFF;
	data[2] = (_x >> 8) & 0xFF;
	data[3] = _x & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Page addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_PAGE_ADDRESS_SET);
	data[0] = (_y >> 8) & 0xFF;
	data[1] = _y & 0xFF;
	data[2] = (_y >> 8) & 0xFF;
	data[3] = _y & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Memory write*/
	ili9488_spi_master_write_command(ILI9488_CMD_MEMORY_WRITE);
	
	uint32_t size = 1;
	uint16_t buf[1];

	buf[0] = color;

	ili9488_spi_master_write_data_bytes(buf, size * 2);
}

void ili9488_DrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors) {
	if (x+size > lcddevice->_width) return;
	if (y >= lcddevice->_height) return;
	
	uint16_t _x1 = x + lcddevice->_offsetx;
	uint16_t _x2 = _x1 + size;
	uint16_t _y1 = y + lcddevice->_offsety;
	uint16_t _y2 = _y1;
	uint8_t data[4];
	/*Column addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_COLUMN_ADDRESS_SET);
	data[0] = (_x1 >> 8) & 0xFF;
	data[1] = _x1 & 0xFF;
	data[2] = (_x2 >> 8) & 0xFF;
	data[3] = _x2 & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Page addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_PAGE_ADDRESS_SET);
	data[0] = (_y1 >> 8) & 0xFF;
	data[1] = _y1 & 0xFF;
	data[2] = (_y2 >> 8) & 0xFF;
	data[3] = _y2 & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Memory write*/
	ili9488_spi_master_write_command(ILI9488_CMD_MEMORY_WRITE);
	
	uint32_t bufsize = (_x2 - _x1 + 1) * (_y2 - _y1 + 1);
	uint16_t buf[ILI9488_SPI_SENDBUFFER_SIZE/2];

	uint32_t i;
	if(size < (ILI9488_SPI_SENDBUFFER_SIZE/2)) {
		for(i = 0; i < bufsize; i++) buf[i] = colors[i];

	} else {
		for(i = 0; i < (ILI9488_SPI_SENDBUFFER_SIZE/2); i++) buf[i] = colors[i];
	}

	while(bufsize > (ILI9488_SPI_SENDBUFFER_SIZE/2)) {
		ili9488_spi_master_write_data_bytes(buf, ILI9488_SPI_SENDBUFFER_SIZE/2);
		bufsize -= (ILI9488_SPI_SENDBUFFER_SIZE/2);
	}

	ili9488_spi_master_write_data_bytes(buf, bufsize);	/*Send the remaining data*/
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
	uint8_t data[4];
	/*Column addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_COLUMN_ADDRESS_SET);
	data[0] = (_x1 >> 8) & 0xFF;
	data[1] = _x1 & 0xFF;
	data[2] = (_x2 >> 8) & 0xFF;
	data[3] = _x2 & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Page addresses*/
	ili9488_spi_master_write_command(ILI9488_CMD_PAGE_ADDRESS_SET);
	data[0] = (_y1 >> 8) & 0xFF;
	data[1] = _y1 & 0xFF;
	data[2] = (_y2 >> 8) & 0xFF;
	data[3] = _y2 & 0xFF;
	ili9488_spi_master_write_data_bytes(data, 4);

	/*Memory write*/
	ili9488_spi_master_write_command(ILI9488_CMD_MEMORY_WRITE);
	
	uint32_t size = (_x2 - _x1 + 1) * (_y2 - y1 + 1);
	uint16_t buf[ILI9488_SPI_SENDBUFFER_SIZE/2];

	uint32_t i;
	if(size < (ILI9488_SPI_SENDBUFFER_SIZE/2)) {
		for(i = 0; i < size; i++) buf[i] = color;

	} else {
		for(i = 0; i < (ILI9488_SPI_SENDBUFFER_SIZE/2); i++) buf[i] = color;
	}

	while(size > (ILI9488_SPI_SENDBUFFER_SIZE/2)) {
		ili9488_spi_master_write_data_bytes(buf, ILI9488_SPI_SENDBUFFER_SIZE);
		size -= (ILI9488_SPI_SENDBUFFER_SIZE/2);
	}

	ili9488_spi_master_write_data_bytes(buf, size * 2);	/*Send the remaining data*/
}

// Display OFF
void ili9488_DisplayOff() {
	ili9488_spi_master_write_command(ILI9488_CMD_DISPLAY_OFF);	//Display off
}
 
// Display ON
void ili9488_DisplayOn() {
	ili9488_spi_master_write_command(ILI9488_CMD_DISPLAY_ON);	//Display on
}

void ili9488_InversionOff() {
	ili9488_spi_master_write_command(ILI9488_CMD_DISP_INVERSION_OFF);	//Display Inversion Off
}

// Display Inversion On
void ili9488_InversionOn() {
	ili9488_spi_master_write_command(ILI9488_CMD_DISP_INVERSION_ON);	//Display Inversion On
}

#endif