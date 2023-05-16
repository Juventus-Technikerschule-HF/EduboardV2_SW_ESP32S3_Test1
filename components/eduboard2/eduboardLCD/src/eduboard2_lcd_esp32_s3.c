#include "../../eduboard2.h"
#include "../eduboard2_lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#ifdef CONFIG_LCD_ST7789
    #include "st7789.h"
#elif CONFIG_LCD_ILI9488

#endif



#define TAG "Eduboard2_LCDDriver"
#ifdef CONFIG_LCD_TEST
TickType_t FillTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	lcdFillScreen(dev, RED);
    lcdUpdateVScreen(dev);
	vTaskDelay(50);
	lcdFillScreen(dev, GREEN);
    lcdUpdateVScreen(dev);
	vTaskDelay(50);
	lcdFillScreen(dev, BLUE);
    lcdUpdateVScreen(dev);
	vTaskDelay(50);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t ColorBarTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	if (width < height) {
		uint16_t y1,y2;
		y1 = height/3;
		y2 = (height/3)*2;
		lcdDrawFillRect(dev, 0, 0, width-1, y1-1, RED);
        vTaskDelay(1);
		lcdDrawFillRect(dev, 0, y1, width-1, y2-1, GREEN);
        vTaskDelay(1);
		lcdDrawFillRect(dev, 0, y2, width-1, height-1, BLUE);
        lcdUpdateVScreen(dev);
	} else {
		uint16_t x1,x2;
		x1 = width/3;
		x2 = (width/3)*2;
		lcdDrawFillRect(dev, 0, 0, x1-1, height-1, RED);
        vTaskDelay(1);
		lcdDrawFillRect(dev, x1, 0, x2-1, height-1, GREEN);
        vTaskDelay(1);
		lcdDrawFillRect(dev, x2, 0, width-1, height-1, BLUE);
        lcdUpdateVScreen(dev);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t LineTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color=RED;
	for(int ypos=0;ypos<height;ypos=ypos+10) {
		lcdDrawLine(dev, 0, ypos, width, ypos, color);
	}

	for(int xpos=0;xpos<width;xpos=xpos+10) {
		lcdDrawLine(dev, xpos, 0, xpos, height, color);
	}
    lcdUpdateVScreen(dev);
	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t CircleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = CYAN;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;
	for(int i=5;i<height;i=i+5) {
		lcdDrawCircle(dev, xpos, ypos, i, color);
        lcdUpdateVScreen(dev);
	}
	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t RectAngleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = CYAN;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;

	uint16_t w = width * 0.6;
	uint16_t h = w * 0.5;
	int angle;
	for(angle=0;angle<=(360*3);angle=angle+30) {
		lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, color);
		vTaskDelay(10/portTICK_PERIOD_MS);
        lcdUpdateVScreen(dev);
        lcdClearVScreen(dev);
		//lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, BLACK);        
	}

	for(angle=0;angle<=180;angle=angle+30) {
		lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, color);
        lcdUpdateVScreen(dev);
	}


	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t TriangleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = CYAN;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;

	uint16_t w = width * 0.6;
	uint16_t h = w * 1.0;
	int angle;

	for(angle=0;angle<=(360*3);angle=angle+30) {
		lcdDrawTriangle(dev, xpos, ypos, w, h, angle, color);
		vTaskDelay(10/portTICK_PERIOD_MS);
        lcdUpdateVScreen(dev);
        lcdClearVScreen(dev);
		//lcdDrawTriangle(dev, xpos, ypos, w, h, angle, BLACK);
	}

	for(angle=0;angle<=360;angle=angle+30) {
		lcdDrawTriangle(dev, xpos, ypos, w, h, angle, color);
        lcdUpdateVScreen(dev);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t RoundRectTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	uint16_t limit = width;
	if (width > height) limit = height;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = BLUE;
	for(int i=5;i<limit;i=i+5) {
		if (i > (limit-i-1) ) break;
		//ESP_LOGI(__FUNCTION__, "i=%d, width-i-1=%d",i, width-i-1);
		lcdDrawRoundRect(dev, i, i, (width-i-1), (height-i-1), 10, color);
        lcdUpdateVScreen(dev);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t FillRectTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	lcdFillScreen(dev, CYAN);

	uint16_t red;
	uint16_t green;
	uint16_t blue;
	srand(startTick);
	for(int i=1;i<100;i++) {
		red=rand()%255;
		green=rand()%255;
		blue=rand()%255;
		color=rgb565_conv(red, green, blue);
		uint16_t xpos=rand()%width;
		uint16_t ypos=rand()%height;
		uint16_t size=rand()%(width/5);
		lcdDrawFillRect(dev, xpos, ypos, xpos+size, ypos+size, color);
        lcdUpdateVScreen(dev);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t ColorTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	uint16_t color;
	lcdFillScreen(dev, WHITE);
	color = RED;
	uint16_t delta = height/16;
	uint16_t ypos = 0;
	for(int i=0;i<16;i++) {
		//ESP_LOGI(__FUNCTION__, "color=0x%x",color);
		lcdDrawFillRect(dev, 0, ypos, width-1, ypos+delta, color);
        lcdUpdateVScreen(dev);
		color = color >> 1;
		ypos = ypos + delta;
	}
    

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t TextTest(TFT_t * dev, char* text) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
    lcdClearVScreen(dev);

	lcdSetCursor(0,0);
	lcdSetTextSize(2);
	lcdSetTextColor(rgb565_conv(0xFF, 0, 0xFF));
	lcdPrintln(dev, text);
	lcdUpdateVScreen(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

void lcdTest(TFT_t* dev) {
    FillTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ColorBarTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    LineTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    CircleTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    RoundRectTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    RectAngleTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    TriangleTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    FillRectTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ColorTest(dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    vTaskDelay(1000/portTICK_PERIOD_MS);

	TextTest(dev, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam lectus justo, dapibus ut fringilla nec, interdum sed eros. Integer euismod tincidunt fermentum. Ut vel aliquet augue. Nam ullamcorper orci eget eros accumsan suscipit. Praesent \nlacus nulla, tristique a tortor lobortis, rhoncus venenatis nulla. Etiam scelerisque sodales finibus. Donec vel dui quis dolor imperdiet porta quis sed ante. Donec porta, nunc eu dignissim hendrerit, neque tortor maximus urna, eu mollis massa arcu sed lectus. Integer commodo augue tortor, id ornare nunc tempor vel. Integer nec risus tortor. Etiam iaculis fermentum libero scelerisque blandit. Mauris vestibulum eget felis a tempor. Pellentesque vitae nulla pulvinar, porttitor mi nec, lobortis est.");
	vTaskDelay(10000/portTICK_PERIOD_MS);
}
#endif

void eduboard_init_lcd() {
    ESP_LOGI(TAG, "Init LCD");
    TFT_t dev;
    spi_master_init(&dev, GPIO_MOSI, GPIO_SCK, GPIO_LCD_CS, GPIO_LCD_DC, GPIO_GENERAL_RESET, -1);
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, 0, 0);
    lcdSetupVScreen();
    ESP_LOGI(TAG, "LCD Init Done");
#ifdef CONFIG_LCD_TEST
    while(1) {
        lcdTest(&dev);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
#endif

}