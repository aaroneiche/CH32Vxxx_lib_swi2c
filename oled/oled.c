/******************************************************************************
* lib_swi2c demonstation and test ground
* 
* ADBeta (c) 2024
******************************************************************************/

// ch32v003fun is included for helpful features like printf etc.
// it is NOT Needed in order to use this library
#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include "oled.h"
#include "lib_swi2c.h"

i2c_device_t dev = {
	// .pin_scl = GPIO_PC3,
	// .pin_sda = GPIO_PC4,
	.pin_scl = GPIO_PA2,
	.pin_sda = GPIO_PD6,

	// .address = 0xD0,
	// .address = 0x3C,
	.address = 0x78,
};

uint8_t data[12];
uint8_t ssd1306_buffer[SSD1306_W * SSD1306_H / 8];

void ssd1306_refresh();
void ssd1306_setbuf(uint8_t color);

int main()
{
	SystemInit();

	swi2c_init(&dev);
	// swi2c_master_transmit(&dev, 0x00, SSD1306_DISPLAYOFF, 1);
	// swi2c_scan(&dev);

	ssd1306_setbuf(1);
	// ssd1306_refresh();

	ssd1306_buffer[0] = 0xAA;
	ssd1306_buffer[1] = 0x55;
	ssd1306_buffer[2] = 0xAA;
	ssd1306_buffer[3] = 0x55;
	ssd1306_buffer[10] = 0xAA;
	ssd1306_buffer[11] = 0x55;
	ssd1306_buffer[12] = 0xAA;
	ssd1306_buffer[13] = 0x55;

	// ssd1306_buffer[4] = 0x00;
	// ssd1306_buffer[30] = 0xAF;
	// ssd1306_buffer[32] = 0xAF;
	// ssd1306_buffer[33] = 0xAF;
	// ssd1306_buffer[34] = 0xAF;
	// ssd1306_buffer[35] = 0xAF;

	// swi2c_master_transmit(&dev, 0x00, ssd1306_init_array, sizeof(ssd1306_init_array));

	// ssd1306_setbuf(1);
	ssd1306_refresh();

	// printf("All done");


	/*
	ssd1306_cmd(SSD1306_COLUMNADDR);
	ssd1306_cmd(SSD1306_OFFSET);				 // Column start address (0 = reset)
	ssd1306_cmd(SSD1306_OFFSET + SSD1306_W - 1); // Column end address (127 = reset)

	// #if defined SSD1306_64X48

	ssd1306_cmd(SSD1306_PAGEADDR);
	ssd1306_cmd(2); // Page start address (0 = reset) (2 = offset for 48px display)
	ssd1306_cmd(7); // Page end address
	*/

	// swi2c_master_transmit(&dev, 0x00, SSD1306_DISPLAYOFF, 1);
	// swi2c_master_transmit(&dev, 0xD5, 0x80, 1);
	
	// swi2c_master_transmit(&dev, 0x00, );
	// printf("2\n");

	// printf("2\n");
	// swi2c_master_transmit(&dev, 0x01, SSD1306_SETDISPLAYCLOCKDIV, 1);
	// printf("3\n");
	// swi2c_master_transmit(&dev, 0x02, 0x80, 1);
	
	while(1){

	};
}

void ssd1306_refresh() {
	// Initialization
	swi2c_master_transmit(&dev, 0x00, ssd1306_init_array, sizeof(ssd1306_init_array));

	// Sets the column start and end.
	// swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_COLUMNADDR, 32, 95}, 3);
	
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_COLUMNADDR}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){0x20}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){0x5F}, 1);
	
	// Sets the page (row)
	// swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_PAGEADDR, 2,5}, 3);

	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_PAGEADDR}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){2}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){7}, 1);

	// Delay_Ms(1000);

	for (int i = 0; i < sizeof(ssd1306_buffer); i += SSD1306_PSZ)
	{
		// ssd1306_data(&ssd1306_buffer[i], SSD1306_PSZ);
		// swi2c_master_transmit(&dev, 0x40, &ssd1306_buffer[i], SSD1306_PSZ);
		swi2c_master_transmit(&dev, 0x40, &ssd1306_buffer[i], SSD1306_PSZ);
	}
}

void ssd1306_setbuf(uint8_t color)
{
	memset(ssd1306_buffer, color ? 0xFF : 0x00, sizeof(ssd1306_buffer));
}