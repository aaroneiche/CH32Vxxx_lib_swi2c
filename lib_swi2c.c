/******************************************************************************
* lib_GPIOCTRL
* A runtime-capable GPIO Library, with Digital Read/Write and
* TODO: Analog Read & Analog Write/PWM
*
*
* See GitHub for details: https://github.com/ADBeta/CH32V003_lib_GPIOCTRL
*
* ADBeta (c) 2024
******************************************************************************/
#include "lib_swi2c.h"

#include <stdint.h>


/*** Copied from lib_GPIOCTRL ************************************************/
// https://github.com/ADBeta/CH32V003_lib_GPIOCTRL

/*** GPIO Pin Mode Enumeration ***********************************************/
/// @breif GPIO Pin Mode data. The lower nibble is the raw binary data for the
/// [R32_GPIOx_CFGLR] Register. The upper nibble is used for additional flags 
typedef enum {
	INPUT_ANALOG       = 0x00,
	INPUT_FLOATING     = 0x04,
	// Mapped to INPUT_PP, Sets OUTDR based on the upper nibble
	INPUT_PULLUP       = 0x18,
	INPUT_PULLDOWN     = 0x08,
	//
	OUTPUT_10MHZ_PP    = 0x01,
	OUTPUT_10MHZ_OD    = 0x05,
	//
	OUTPUT_2MHZ_PP     = 0x02,
	OUTPUT_2MHZ_OD     = 0x06,
} gpio_mode_t;


/*** GPIO Output State Enumerations ******************************************/
/// @breif GPIO Pin State Enum, simple implimentation of a HIGH/LOW System
typedef enum {
	GPIO_LOW     = 0x00,
	GPIO_HIGH    = 0x01,
} gpio_state_t;


/*** Registers for GPIO Port *************************************************/
/// @breif GPIO Port Register, Directly Maps to Memory starting at 
/// [R32_GPIOx_CFGLR] for each PORT Respectively
typedef struct {
	volatile uint32_t CFGLR;  // Configuration Register (lower)
	volatile uint32_t CFGHR;  // Configuration Register (upper)
	volatile uint32_t INDR;   // Input Data Register
	volatile uint32_t OUTDR;  // Output Data Register
	volatile uint32_t BSHR;   // Set/Reset Register
	volatile uint32_t BCR;    // Port Reset Register
	volatile uint32_t LCKR;   // Lock Register
} gpio_port_reg_t;


/*** Register Address Definitions ********************************************/
#define RCC_APB2PCENR ((volatile uint32_t *)0x40021018)
#define APB2PCENR_AFIO   0x01
#define APB2PCENR_IOPxEN 0x04

#define PORTA_GPIO_REGISTER_BASE 0x40010800
// NOTE: PORTB is not available for the CH32V003.
#define PORTB_GPIO_REGISTER_BASE 0x40010C00
#define PORTC_GPIO_REGISTER_BASE 0x40011000
#define PORTD_GPIO_REGISTER_BASE 0x40011400

#define GPIO_PORTA ((gpio_port_reg_t *)PORTA_GPIO_REGISTER_BASE)
// NOTE: PORTB is not available for the CH32V003.
#define GPIO_PORTB ((gpio_port_reg_t *)PORTB_GPIO_REGISTER_BASE)
#define GPIO_PORTC ((gpio_port_reg_t *)PORTC_GPIO_REGISTER_BASE)
#define GPIO_PORTD ((gpio_port_reg_t *)PORTD_GPIO_REGISTER_BASE)

/// @breif The GPIO Ports are places into an array for easy indexing in the
/// GPIO Functions
/// NOTE: Only 3 PORTs are usable in the CH32V003, 4 for other MCUs
static gpio_port_reg_t *gpio_port_reg[4] = {
	GPIO_PORTA,
	NULL,
	GPIO_PORTC,
	GPIO_PORTD,
};


/// @breif Sets the OUTDR Register for the passed Pin
/// @param gpio_pin_t pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @param gpio_state_t state, GPIO State to be set (e.g GPIO_HIGH)
/// @return None
__attribute__((always_inline))
static inline void gpio_digital_write(const gpio_pin_t pin, const gpio_state_t state)
{
	// Make array of uint8_t from [pin] enum. See definition for details
	uint8_t *byte = (uint8_t *)&pin;

	if(state == GPIO_HIGH)
		gpio_port_reg[ byte[0] ]->OUTDR |=  (0x01 << byte[1]);
	if(state == GPIO_LOW)
		gpio_port_reg[ byte[0] ]->OUTDR &= ~(0x01 << byte[1]);
}

/// @breif Reads the INDR Register of the specified pin and returns state
/// @param gpio_pin_t pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @return gpio_state_t, the current state of the pin, (e.g GPIO_HIGH)
__attribute__((always_inline))
static inline gpio_state_t gpio_digital_read(const gpio_pin_t pin)
{
	// Make array of uint8_t from [pin] enum. See definition for details
	uint8_t *byte = (uint8_t *)&pin;

	// If the Input Reg has the wanted bit set, return HIGH
	if( (gpio_port_reg[ byte[0] ]->INDR & (0x01 << byte[1])) != 0x00 ) 
		return GPIO_HIGH;

	// else return LOW 
	return GPIO_LOW;
}

/// @breif Sets the Config and other needed Registers for a passed pin and mode
/// @param gpio_pin_t pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @param gpio_mode_t mode, the GPIO Mode Variable (e.g OUTPUT_10MHZ_PP)
/// @return None
static void gpio_set_mode(const gpio_pin_t pin, const gpio_mode_t mode)
{
	// TODO: Any pin over 8 needs to change CFGHR
	
	// Make array of uint8_t from [pin] enum. See definition for details
	uint8_t *byte = (uint8_t *)&pin;
	
	// Set the RCC Register to enable clock on the specified port
	*RCC_APB2PCENR |= (APB2PCENR_AFIO | (APB2PCENR_IOPxEN << byte[0]));

	// Clear then set the GPIO Config Register
	gpio_port_reg[ byte[0] ]->CFGLR &=        ~(0x0F  << (4 * byte[1]));
	gpio_port_reg[ byte[0] ]->CFGLR |=  (mode & 0x0F) << (4 * byte[1]);

	// If [mode] is INPUT_PULLUP or INPUT_PULLDOWN, set the [OUTDR] Register
	if(mode == INPUT_PULLUP || mode == INPUT_PULLDOWN)
		gpio_digital_write(pin, mode >> 4);
}


/*** Software I2C Functions **************************************************/
// Asserting I2C lines is when they are OUTPUT Pulling LOW
#define ASSERT_SCL gpio_set_mode(i2c->pin_scl, GPIO_OUTPUT_10MHZ_PP); 
#define ASSERT_SDA gpio_set_mode(i2c->pin_sda, GPIO_OUTPUT_10MHZ_PP);

// Releasing I2C lines is setting them INPUT FLOATING, Pulled HIGH Externally
#define RELEASE_SCL gpio_set_mode(i2c->pin_scl, GPIO_INPUT_FLOATING); 
#define RELEASE_SDA gpio_set_mode(i2c->pin_sda, GPIO_INPUT_FLOATING);

/*** Helper Functions ********************************************************/
// Waits for the calculated amount of time (Limits bus speed)
// TODO:
static void wait()
{

}


/*** Library Functions *******************************************************/
void swi2c_init(i2c_bus_t *i2c, const gpio_pin_t scl, const gpio_pin_t sda)
{

}


swi2c_start(const i2c_bus_t *i2c)
{

}


swi2c_stop(const i2c_bus_t *i2c)
{

}






