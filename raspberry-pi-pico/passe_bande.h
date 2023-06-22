#ifndef __LED_PASSE_BANDE_H__
#define __LED_PASSE_BANDE_H__

#include "pico/stdlib.h"

#include <stdint.h>

extern uint32_t g_CompteurFiltre;
extern float g_SortieFiltre;
extern float g_SortieMax;

#if __cplusplus
extern "C" {
#endif

// Déclaration de l'interruption d'échantillonage
bool filtre1_timer(repeating_timer_t *t);

#if __cplusplus
}
#endif

#endif