#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#include "led.hpp"
#include "bt.hpp"
#include "passe_bande.h"

#define LED_PIN         PICO_DEFAULT_LED_PIN

#define SPI_CLK_PIN     2
#define SPI_MOSI_PIN    3

#define UART_TX_PIN     4
#define UART_RX_PIN     5

#define FLAG_OK         100
#define FLAG_VALUE      123
#define FLAG_BT_CHANGE  124

#define ADC_PIN         26
#define ADC_NUM         0

#define LED_THRESHOLD_1     0.150f
#define LED_THRESHOLD_2     0.300f
#define LED_THRESHOLD_3     0.450f
#define LED_THRESHOLD_4     0.600f
#define LED_THRESHOLD_5     0.750f
#define LED_THRESHOLD_6     1.0f
#define LED_THRESHOLD_7     1.15f
#define LED_THRESHOLD_8     1.30f
#define LED_THRESHOLD_9     1.45f

std::string g_CommandLed;

uint8_t g_Demo = 0;

uint32_t g_Demo1MS = 50;

LedColor pile1[] = {
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF
};

LedColor pile2[] = {
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF
};

LedColor pile3[] = {
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF
};

LedColor pile4[] = {
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF
};

LedColor pile5[] = {
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF,
    LED_OFF
};

// while(1)
// {
//     add_alarm_in_us(23, interrupt_1, NULL, false);
// }

/*
uint16_t get_adc_led_number(uint16_t acquisitions)
{
    while(true) {
        g_Signal = adcInput;
        adcInput = g_SortieFiltre;

        
    }

    //printf("adc_input_max: %f\n", adcInputMax);
    return (adcInputMax * 10) / LED_ADC_RANGE;
}
*/

/*
============================
core1_entry

point d'entrée du 2ème coeur
============================
*/
void core1_entry()
{
    // Initialisation...
    uint32_t flagValue;
    BT bt(uart1, UART_TX_PIN, UART_RX_PIN);

    multicore_fifo_push_blocking(FLAG_VALUE);

    flagValue = multicore_fifo_pop_blocking();

    if(flagValue != FLAG_VALUE)
        return;

    while(true) {
        g_CommandLed = bt.readLine();
        if(!g_CommandLed.empty()) {
            printf("%s\n", g_CommandLed.c_str());
            multicore_fifo_push_blocking(FLAG_BT_CHANGE);

            // Attend que le coeur 0 ai mit à jour les LEDs
            flagValue = multicore_fifo_pop_blocking();
        }
    }
}

/*
===========================
main

point d'entrée du programme
===========================
*/
int main()
{
    Led led(120, SPI_CLK_PIN, SPI_MOSI_PIN, spi0);
    
    int bytesWritten;
    

    uint32_t flagValue;
    uint adc_raw;

    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    // Initialise les entrées / sorties standards pour le Pico
    stdio_init_all();

    // Réveil le 2ème coeur et lui indique d'exécuter la fonction spécifiée
    multicore_launch_core1(core1_entry);

    // Attend qu'une valeur soit disponible dans le FIFO pour la lire,
    // permet d'attendre que le coeur 1 finisse d'inialiser ce dont il a besoin
    flagValue = multicore_fifo_pop_blocking();
    if(flagValue != FLAG_VALUE) {
        fprintf(stderr, "Bad value from core 1\n");
        multicore_reset_core1();
    }else
        multicore_fifo_push_blocking(FLAG_VALUE);

    // Initialise la broche tout ou rien correspondant à la LED
    gpio_init(LED_PIN);
    // Indique la direction de la broche,
    // ici c'est une sortie car on veut allumer ou éteindre la LED
    gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);

    printf("Program initialized, ready to party!\n");

    // Eteint toutes les LEDs au démarrage
    led.startFrame();
    led.turnOff();
    led.endFrame();
    led.send();

    led.setLedNumber(50);
    led.startFrame();
    led.setPileColors(0, 10, pile1);
    led.setPileColors(1, 10, pile2);
    led.setPileColors(2, 10, pile3);
    led.setPileColors(3, 10, pile4);
    led.setPileColors(4, 10, pile5);
    led.endFrame();
    led.send();


    uint16_t numberOfLedsToShow;

    
    
    //add_repeating_timer_ms(2, filtre1_timer, NULL, &timer);

    // Boucle infinie du programme
    while(1) {
        // Synchro avec le 2ème coeur qui s'occupe du Bluetooth
        // TODO: cette partie doit entièrement être dans le 2ème coeur
        if(multicore_fifo_rvalid()) {
            flagValue = multicore_fifo_pop_blocking();
            switch(flagValue) {
                default: break;
                case FLAG_BT_CHANGE: {
                    gpio_put(LED_PIN, true);
                    led.startFrame();
                    led.executeCommand(g_CommandLed);
                    led.endFrame();
                    led.send();
                    gpio_put(LED_PIN, false);
                } break;
            }
            multicore_fifo_push_blocking(FLAG_OK);
        }

        // Quand il n'y a pas de démo en cours
        if(g_Demo == 0) {
            repeating_timer_t timer;
            // Lance un timer qui se répète à un intervale de temps
            add_repeating_timer_us(100, filtre1_timer, NULL, &timer);
            while(g_CompteurFiltre < 2000);     // Attend qu'il y ai eu 2000 acquisitions
            cancel_repeating_timer(&timer);     // Stop le timer
            g_CompteurFiltre = 0;
            printf("Valeur filtre: %f\n", g_SortieMax);

            numberOfLedsToShow = (g_SortieMax * 10) / 0.5f;

            Led::turnLedOn(pile1, 0, 10, numberOfLedsToShow);

            led.startFrame();
            led.setPileColors(0, 10, pile1);
            led.endFrame();
            led.send();

            g_SortieMax = 0.0f;
        }else if(g_Demo == 1) {
            int pileIndex;
            static int pile1Num = 9;
            static int pile2Num = 7;
            static int pile3Num = 5;
            static int pile4Num = 3;
            static int pile5Num = 1;

            // Pile 1
            for(pileIndex = 0; pileIndex < pile1Num; ++pileIndex)
                pile1[pileIndex] = Led::_pileColor[0];
            for(; pileIndex < 10; ++pileIndex)
                pile1[pileIndex] = LED_OFF;

            pile1Num++;
            if(pile1Num >= 11)
                pile1Num = 0;

            // Pile 2
            for(pileIndex = 0; pileIndex < pile2Num; ++pileIndex)
                pile2[pileIndex] = Led::_pileColor[1];
            for(; pileIndex < 10; ++pileIndex)
                pile2[pileIndex] = LED_OFF;

            pile2Num++;
            if(pile2Num >= 11)
                pile2Num = 0;

            // Pile 3
            for(pileIndex = 0; pileIndex < pile3Num; ++pileIndex)
                pile3[pileIndex] = Led::_pileColor[2];
            for(; pileIndex < 10; ++pileIndex)
                pile3[pileIndex] = LED_OFF;

            pile3Num++;
            if(pile3Num >= 11)
                pile3Num = 0;

            // Pile 4
            for(pileIndex = 0; pileIndex < pile4Num; ++pileIndex)
                pile4[pileIndex] = Led::_pileColor[3];
            for(; pileIndex < 10; ++pileIndex)
                pile4[pileIndex] = LED_OFF;

            pile4Num++;
            if(pile4Num >= 11)
                pile4Num = 0;

            // Pile 5
            for(pileIndex = 0; pileIndex < pile5Num; ++pileIndex)
                pile5[pileIndex] = Led::_pileColor[4];
            for(; pileIndex < 10; ++pileIndex)
                pile5[pileIndex] = LED_OFF;

            pile5Num++;
            if(pile5Num >= 11)
                pile5Num = 0;

            led.startFrame();
            led.setPileColors(0, 10, pile1);
            led.setPileColors(1, 10, pile2);
            led.setPileColors(2, 10, pile3);
            led.setPileColors(3, 10, pile4);
            led.setPileColors(4, 10, pile5);
            led.endFrame();
            led.send();

            sleep_ms(g_Demo1MS);
        }else if(g_Demo == 2) {

        }

        

        // Récupère la valeur max de l'amplitude sur un nombre d'acquisitions
        //numberOfLedsToShow = get_adc_led_number(10000);
        //printf("LEDs number: %d\n", numberOfLedsToShow);
        // Led::turnLedOn(pile1, 0, 10, numberOfLedsToShow);                    
        // led.setPileColors(0, 10, pile1);
        // led.endFrame();
        // led.send();

        //sleep_ms(250);
    }
    
    return 0;
}
