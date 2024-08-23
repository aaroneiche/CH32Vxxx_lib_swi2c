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

uint8_t ssd1306_buffer[SSD1306_W * SSD1306_H / 8];

void ssd1306_refresh();
void ssd1306_setbuf(uint8_t color);

int main()
{
	SystemInit();

	swi2c_init(&dev);
	
	ssd1306_setbuf(1);
	// ssd1306_refresh();

	// ssd1306_buffer[0] = 0xFF;
	// ssd1306_buffer[1] = 0xFF;
	// ssd1306_buffer[2] = 0xFF;
	// ssd1306_buffer[3] = 0xFF;
	// ssd1306_buffer[4] = 0xFF;
	// ssd1306_buffer[5] = 0xFF;
	// ssd1306_buffer[6] = 0xFF;
	// ssd1306_buffer[7] = 0xFF;

	// ssd1306_buffer[64] = 0xAA;
	// ssd1306_buffer[65] = 0x55;
	// ssd1306_buffer[66] = 0xAA;
	// ssd1306_buffer[67] = 0x55;
	// ssd1306_buffer[68] = 0xAA;
	// ssd1306_buffer[69] = 0x55;
	// ssd1306_buffer[70] = 0xAA;
	// ssd1306_buffer[71] = 0x55;

	// ssd1306_buffer[128] = 0xAA;
	// ssd1306_buffer[129] = 0x55;
	// ssd1306_buffer[130] = 0xAA;
	// ssd1306_buffer[131] = 0x55;

	// ssd1306_buffer[192] = 0xAA;
	// ssd1306_buffer[193] = 0x55;
	// ssd1306_buffer[194] = 0xAA;
	// ssd1306_buffer[195] = 0x55;

	// ssd1306_buffer[254] = 0xAA;

	// ssd1306_buffer[256] = 0xAA;
	// ssd1306_buffer[257] = 0x55;
	// ssd1306_buffer[258] = 0xAA;
	// ssd1306_buffer[259] = 0x55;
	// ssd1306_buffer[260] = 0xAA;
	// ssd1306_buffer[261] = 0x55;
	// ssd1306_buffer[262] = 0xAA;
	// ssd1306_buffer[263] = 0x55;

	// Initialization
	swi2c_master_transmit(&dev, 0x00, ssd1306_init_array, sizeof(ssd1306_init_array));
/* 
	uint8_t *cmd_list = (uint8_t *)ssd1306_init_array;

	while(*cmd_list != SSD1306_TERMINATE_CMDS) {
		swi2c_master_transmit(&dev, 0x00, *cmd_list++, 1);
		printf("Sending %02X \n", *cmd_list);
	} */


	// ssd1306_setbuf(1);
	ssd1306_refresh();
	
/* 	
	for(int s = 0; s < 16; s++){
		swi2c_master_transmit(&dev, 0x00, (uint8_t *){SSD1306_SETDISPLAYOFFSET, s}, 1);
		printf("Sending %u\n",s);
		Delay_Ms(500);
	}
 */

	while(1){

	};
}

void ssd1306_refresh() {
	

	// Sets the column start and end.
	// swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_COLUMNADDR, 32, 95}, 3);
	
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_COLUMNADDR}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){0x20}, 1);  //Display offset: 32
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){0x5F}, 1); 	//disply offset + display width - 1: 95
	
	// Sets the page (row)
	// swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_PAGEADDR, 2,5}, 3);

	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){SSD1306_PAGEADDR}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){2}, 1);
	swi2c_master_transmit(&dev, 0x00, (const uint8_t[]){7}, 1);// Initialization
	
	// Delay_Ms(1000);
	// printf("size of buffer: %u\n", sizeof(ssd1306_buffer));

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