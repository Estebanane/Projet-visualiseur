#ifndef __PROJET_LED_H__
#define __PROJET_LED_H__

#include "stm32f2xx_hal.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

#include <stddef.h>
#include <stdint.h>

#define LED_MAX_LED_NUMBER 50
#define LED_START_FRAME_LENGHT 4
#define LED_FRAME_LENGTH (LED_MAX_LED_NUMBER * 4)
#define LED_END_FRAME_LENGTH ((uint8_t) (((LED_MAX_LED_NUMBER - 1) / 16) + 1))

#define LED_NUMBER_OF_LEDS_PER_PILE 10

// Taille du stack pour le thread SPI
#define LED_SPI_THREAD_STACK_SIZE (1024)

#define LED_TRUE 	1
#define LED_FALSE 0

typedef char led_bool;

typedef struct led_color_s {
	uint8_t r;		///< Couleur rouge
	uint8_t g;		///< Couleur verte
	uint8_t b;		///< Couleur bleue
	uint8_t a;		///< Intensité de la lumière
} led_color_t;

// Si le code est compilé par un compilateur C++
#ifdef __cplusplus
extern "C" {
#endif

	void LED_spi_callback(uint32_t event);
	void LED_spi_thread(void *arg);

	led_bool LED_init();
	void LED_start_frame();
	void LED_end_frame();
	void LED_clear();
	void LED_set_amount(uint8_t number);
	void LED_set_color(uint16_t ledNumber, const led_color_t *color);
	void LED_set_colors(uint16_t ledOffset, const led_color_t *colors, uint16_t numberOfColors);
	void LED_set_pile_colors(uint8_t pileNumber, const led_color_t *colors);
	led_bool LED_send();

#ifdef __cplusplus
}
#endif

extern const osThreadAttr_t LED_SpiThreadAttr;
extern osThreadId_t LED_SpiThreadID;

#endif