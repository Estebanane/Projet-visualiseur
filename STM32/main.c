#include "main.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#include <stdio.h>
// Permet d'utiliser l'API du protocol SPI
#include "Driver_SPI.h"

#define LED_NUMBER 8
#define START_FRAME_LENGHT 4
#define LED_FRAME_LENGTH (LED_NUMBER * 4)
#define END_FRAME_LENGTH ((uint8_t) (((LED_NUMBER - 1) / 16) + 1))

#define LED_TRUE 1
#define LED_FALSE 0

// Taille du stack pour le thread SPI
#define SPI_THREAD_STACK_SIZE (1024)

typedef char led_bool;

// Driver SPI
extern ARM_DRIVER_SPI Driver_SPI1;

// Tableau contenant la trame à envoyer
static uint8_t g_DataFrame[START_FRAME_LENGHT + LED_FRAME_LENGTH + END_FRAME_LENGTH];

void spi_callback(uint32_t event);
void spi_thread(void *arg);

static uint64_t g_SpiStack[(SPI_THREAD_STACK_SIZE + 7) / 8];
static const osThreadAttr_t g_SpiThreadAttr = {
	.name = "SPI_THREAD",
	.stack_mem = g_SpiStack,
	.stack_size = sizeof(g_SpiStack)
};
static osThreadId_t g_SpiThreadID;

const char *STR_ERROR = "ERROR";

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

/**
  * Override default HAL_InitTick function
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  
  UNUSED(TickPriority);

  return HAL_OK;
}
#endif

static void SystemClock_Config(void);
static void Error_Handler(void);

led_bool LED_init();
void LED_start_frame();
void LED_end_frame();
void LED_clear();
void LED_set_color(uint16_t number, uint8_t intensity, uint8_t r, uint8_t g, uint8_t b);
led_bool LED_send();

// Point d'entrée du programme
int main(void) {
	ARM_DRIVER_VERSION spiDriverVersion;
	ARM_SPI_CAPABILITIES spiCapabilities;
	
	
  /* STM32F2xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 120 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();
	
	// Récupère la version du driver SPI
	spiDriverVersion = Driver_SPI1.GetVersion();
	spiCapabilities = Driver_SPI1.GetCapabilities();
	
	if(spiCapabilities.event_mode_fault == 0) {
		fprintf(stderr, "Error SPI Driver\n");
		while(1);
	}
	
	printf("SPI Driver v%d - API v%d\n", spiDriverVersion.drv, spiDriverVersion.api);

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize();

  // Initialise le thread pour la communication SPI
  g_SpiThreadID = osThreadNew(spi_thread, NULL, &g_SpiThreadAttr);

  // Démarre les threads
  osKernelStart();
#endif

  // Boucle infinie du programme
  while(1);
	
	return 0;
}

void spi_callback(uint32_t event) {
	if(event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
		printf("SPI transfer complete\n");
		/* Success: Wakeup Thread */
		osThreadFlagsSet(g_SpiThreadID, 0x01);
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

void spi_thread(void *arg) {
	int32_t errorCode;
	
	/*
	uint8_t data[] = {
			0x00, 0x00, 0x00, 0x00,		// Start Frame
			0xFF, 0xFF, 0x00, 0x00,		// Led 1 - Bleu
			0xFF, 0xFF, 0xFF, 0xFF,		// Led 2 - Blanc
			0xFF, 0x00, 0x00, 0xFF, 	// Led 3 - Rouge
			0xFF, 0xFF, 0xFF, 0xFF,		// Led 4 - Blanc
			0xFF, 0xFF, 0x00, 0x00,		// Led 5 - Bleu
			0xFF, 0xFF, 0xFF, 0xFF,		// Led 6 - Blanc
			0xFF, 0x00, 0x00, 0xFF,		// Led 7 - Rouge
			0x00
	};
	*/
	
	// Initialise les drivers SPI pour communiquer avec le bandeau LEDs,
	// à appeler qu'une seule fois.
	LED_init();
	
	// Permet de remplir la trame pour éteindre toutes les LEDs du bandeau
	LED_clear();
	
	// Envoie au bandeau LEDs la trame
	LED_send();
	
	while(1) {
		
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 240
  *            PLL_P                          = 2
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

static void Error_Handler(void) {
  /* User may add here some code to deal with this error */
  while(1) {
		
  }
}

led_bool LED_init() {
	int32_t errorCode;
	
	// Initialise le driver SPI
	Driver_SPI1.Initialize(spi_callback);
	// Allume le périphérique SPI
	errorCode = Driver_SPI1.PowerControl(ARM_POWER_FULL);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI1.PowerControl: %u\n", STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	// Change la configuration du périphérique
	errorCode = Driver_SPI1.Control(
										ARM_SPI_MODE_MASTER | 
										ARM_SPI_CPOL0_CPHA0 | 
										ARM_SPI_MSB_LSB | 
										ARM_SPI_SS_MASTER_UNUSED |
										ARM_SPI_DATA_BITS(8), 
							1000000);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI1.Control: %u\n", STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	errorCode = Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI1.Control: %u\n", STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	return LED_TRUE;
}

void LED_start_frame() {
	uint8_t i;
	
	// Rempli de 0 la 'Start Frame'
	for(i = 0; i < START_FRAME_LENGHT; ++i)
		g_DataFrame[i] = 0x00;
}

void LED_end_frame() {
	uint8_t i;
	
	// Rempli de 0 la fin de la 'End Trame'
	for(i = 0; i < END_FRAME_LENGTH; ++i)
		g_DataFrame[(sizeof(g_DataFrame) - 1) - i] = 0x00;
}

void LED_clear() {
	uint8_t i;
	
	LED_start_frame();
	
	// Rempli la LED Frame
	for(i = 0; i < LED_NUMBER; ++i)
		LED_set_color(i, 0, 0, 0, 0);
	
	LED_end_frame();
}

void LED_set_color(uint16_t number, uint8_t intensity, uint8_t r, uint8_t g, uint8_t b) {
	// Chaque frame de LED contient 4 octets
	uint16_t index = START_FRAME_LENGHT + (number * 4);
	g_DataFrame[index] = 0xE0 | intensity;
	g_DataFrame[index + 1] = b;
	g_DataFrame[index + 2] = g;
	g_DataFrame[index + 3] = r;
}

led_bool LED_send() {
	uint32_t errorCode;
	
	errorCode = Driver_SPI1.Send(g_DataFrame, sizeof(g_DataFrame));
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI1.Send: %u\n", STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	// Bloque le thread en cours jusqu'à que le message soit totalement envoyé
	osThreadFlagsWait(0x01, osFlagsWaitAny, 0);
	return LED_TRUE;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
