#include "led.h"

// Permet d'utiliser l'API du protocol SPI
#include "Driver_SPI.h"

#include <stdio.h>

// Driver SPI
extern ARM_DRIVER_SPI Driver_SPI2;

// Tableau contenant la trame à envoyer
static uint8_t g_DataFrame[LED_START_FRAME_LENGHT + LED_FRAME_LENGTH + LED_END_FRAME_LENGTH];

static uint64_t g_SpiStack[(LED_SPI_THREAD_STACK_SIZE + 7) / 8];
const osThreadAttr_t LED_SpiThreadAttr = {
	.name = "SPI_THREAD",
	.stack_mem = g_SpiStack,
	.stack_size = sizeof(g_SpiStack)
};
osThreadId_t LED_SpiThreadID;

static const char *LED_STR_ERROR = "ERROR";

static uint8_t g_LedNumber = LED_MAX_LED_NUMBER;

/*
==============
LED_set_amount
==============
*/
void LED_set_amount(uint8_t number)
{
	g_LedNumber = number;
}

/*
========
LED_init
========
*/
led_bool LED_init()
{
	int32_t errorCode;
	
	// Initialise le driver SPI
	Driver_SPI2.Initialize(LED_spi_callback);
	// Allume le périphérique SPI
	errorCode = Driver_SPI2.PowerControl(ARM_POWER_FULL);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.PowerControl: %u\n", LED_STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	// Change la configuration du périphérique
	errorCode = Driver_SPI2.Control(
										ARM_SPI_MODE_MASTER | 
										ARM_SPI_CPOL0_CPHA0 | 
										ARM_SPI_MSB_LSB | 
										ARM_SPI_SS_MASTER_UNUSED |
										ARM_SPI_DATA_BITS(8), 
							500000);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.Control: %u\n", LED_STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	errorCode = Driver_SPI2.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.Control: %u\n", LED_STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	return LED_TRUE;
}


/*
================
LED_spi_callback
================
*/
void LED_spi_callback(uint32_t event)
{
	if(event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
		printf("SPI transfer complete\n");
		/* Success: Wakeup Thread */
		osThreadFlagsSet(LED_SpiThreadID, 0x01);
	}
	
	if(event & ARM_SPI_EVENT_DATA_LOST) {
		fprintf(stderr, "Error SPI data lost\n");
	}
	
	if(event & ARM_SPI_EVENT_MODE_FAULT) {
		/*  Occurs in master mode when Slave Select is deactivated and
							indicates Master Mode Fault. */
		fprintf(stderr, "Error SPI mode fault\n");
	}
}

/*
==============
LED_spi_thread
==============
*/
void LED_spi_thread(void *arg)
{
	led_color_t red[LED_NUMBER_OF_LEDS_PER_PILE] = {
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 },
		{ 255, 0, 0, 11 }
	};
	
	led_color_t green[LED_NUMBER_OF_LEDS_PER_PILE] = {
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 },
		{ 0, 255, 0, 11 }
	};
	
	led_color_t blue[LED_NUMBER_OF_LEDS_PER_PILE] = {
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 },
		{ 0, 0, 255, 11 }
	};
	
	// Initialise les drivers SPI pour communiquer avec le bandeau LEDs,
	// à appeler qu'une seule fois.
	LED_init();
	
	LED_start_frame();
	LED_clear();
	LED_end_frame();
	LED_send();
	
	while(1) {
		LED_start_frame();
		LED_set_pile_colors(0, red);
		LED_set_pile_colors(1, green);
		LED_set_pile_colors(2, blue);
		LED_set_pile_colors(3, green);
		LED_set_pile_colors(4, red);
		LED_end_frame();
		LED_send();
		
		// Delay de 2000 ticks
		osThreadFlagsWait(0x02, osFlagsWaitAny, 2000);
	}
}

/*
===============
LED_start_frame
===============
*/
void LED_start_frame()
{
	uint8_t i;
	
	// Rempli de 0 la 'Start Frame'
	for(i = 0; i < LED_START_FRAME_LENGHT; ++i)
		g_DataFrame[i] = 0x00;
}

/*
=============
LED_end_frame
=============
*/
void LED_end_frame()
{
	uint16_t index = LED_START_FRAME_LENGHT + (g_LedNumber * 4);
	uint8_t i;
	
	for(i = 0; i < LED_END_FRAME_LENGTH; ++i)
		g_DataFrame[index + i] = 0x00;
}

/*
=========
LED_clear
=========
*/
void LED_clear()
{
	uint8_t i;
	led_color_t color = { 100, 200, 50, 0 };
	
	// Rempli la LED Frame
	for(i = 0; i < g_LedNumber; ++i)
		LED_set_color(i, &color);	// Eteint la LED
}

/*
=============
LED_set_color
=============
*/
void LED_set_color(uint16_t ledNumber, const led_color_t *color)
{
	// Chaque frame de LED contient 4 octets
	uint16_t index = LED_START_FRAME_LENGHT + (ledNumber * 4);
	g_DataFrame[index] = 0xE0 | color->a;
	g_DataFrame[index + 1] = color->b;
	g_DataFrame[index + 2] = color->g;
	g_DataFrame[index + 3] = color->r;
}

/*
==============
LED_set_colors
==============
*/
void LED_set_colors(uint16_t ledOffset, const led_color_t *colors, uint16_t numberOfColors)
{
	uint16_t index;
	for(index = 0; index < numberOfColors; ++index)
		LED_set_color(ledOffset + index, colors + index);
}

/*
===================
LED_set_pile_colors
===================
*/
void LED_set_pile_colors(uint8_t pileNumber, const led_color_t *colors)
{
	uint16_t offset = LED_NUMBER_OF_LEDS_PER_PILE * pileNumber;
	LED_set_colors(offset, colors, LED_NUMBER_OF_LEDS_PER_PILE);
}

/*
========
LED_send
========
*/
led_bool LED_send() {
	uint32_t errorCode;
	uint16_t size = LED_START_FRAME_LENGHT + (g_LedNumber * 4) + LED_END_FRAME_LENGTH;
	
	errorCode = Driver_SPI2.Send(g_DataFrame, size);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.Send: %u\n", LED_STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	// Bloque le thread en cours jusqu'à que le message soit totalement envoyé
	osThreadFlagsWait(0x01, osFlagsWaitAny, 0);
	return LED_TRUE;
}
