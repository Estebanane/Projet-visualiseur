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

#define MAX_LED_NUMBER 50

static uint16_t g_LedNumber = MAX_LED_NUMBER;

#define START_FRAME_LENGHT 4
#define LED_FRAME_LENGTH (MAX_LED_NUMBER * 4)
#define END_FRAME_LENGTH 			((uint8_t) (((g_LedNumber - 1) / 16) + 1))
#define MAX_END_FRAME_LENGTH 	((uint8_t) (((MAX_LED_NUMBER - 1) / 16) + 1))

#define LED_TRUE 	1
#define LED_FALSE 0

// Taille du stack pour le thread SPI
#define SPI_THREAD_STACK_SIZE (1024)

typedef char led_bool;

// Driver SPI
extern ARM_DRIVER_SPI Driver_SPI2;

// Tableau contenant la trame à envoyer
static uint8_t g_DataFrame[START_FRAME_LENGHT + LED_FRAME_LENGTH + MAX_END_FRAME_LENGTH];


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
	spiDriverVersion = Driver_SPI2.GetVersion();
	spiCapabilities = Driver_SPI2.GetCapabilities();
	
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
	uint8_t trame[] = {
			0x00, 0x00, 0x00, 0x00, 
			0xEF, 0xFF, 0x00, 0x00,			// 1	Bleu
			0xEF, 0x00, 0xFF, 0x00,			// 2	Vert
			0xEF, 0x00, 0x00, 0xFF,			// 3	Rouge
			0xE0, 0xFF, 0x00, 0xFF,			// 4
			0xEF, 0xFF, 0x00, 0xFF,			// 5	Magenta
			0xE0, 0xFF, 0x00, 0xFF,			// 6
			0xE0, 0xFF, 0x00, 0xFF,			// 7
			0xE0, 0xFF, 0x00, 0xFF,			// 8
			0xE0, 0xFF, 0x00, 0xFF,			// 9
			0xE0, 0xFF, 0x00, 0xFF,			// 10
			0xE0, 0xFF, 0x00, 0xFF,			// 11
			0xE0, 0xFF, 0x00, 0xFF,			// 12
			0xE0, 0xFF, 0x00, 0xFF,			// 13
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0xFF, 0x00,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// 
			0xE0, 0xFF, 0x00, 0xFF,			// ...
			0xE0, 0xFF, 0x00, 0xFF,			// 24
			0xFF, 0xFF, 0xFF, 0xFF
	};
	*/
		
	// Initialise les drivers SPI pour communiquer avec le bandeau LEDs,
	// à appeler qu'une seule fois.
	LED_init();
	
	
	
	/*
	LED_start_frame();
	LED_set_color(0, 0, 0, 0, 255);
	LED_set_color(1, 0, 255, 255, 255);
	LED_set_color(2, 0, 255, 0, 0);
	LED_set_color(3, 0, 255, 0, 255);
	LED_set_color(4, 0, 255, 0, 255);
	LED_set_color(5, 0, 255, 0, 255);
	LED_set_color(6, 0, 255, 0, 255);
	LED_set_color(7, 0, 255, 0, 255);
	LED_end_frame();
	LED_send();
	*/
	
	g_LedNumber = 50;
	
	while(1) {
		//Driver_SPI2.Send(trame, sizeof(trame));
		//osThreadFlagsWait(0x01, osFlagsWaitAny, 0);
		
		LED_start_frame();
		LED_clear();
		LED_end_frame();
		LED_send();
		
		osThreadFlagsWait(0x02, osFlagsWaitAny, 2000);
		
		LED_start_frame();
		LED_set_color(0, 0x0F, 0, 0, 255);		// B
		LED_set_color(1, 0x0F, 0, 255, 0);		// V
		LED_set_color(2, 0x00, 255, 0, 0);		// Eteint
		LED_set_color(3, 0x0F, 255, 0, 255);	// M
		LED_set_color(4, 0x0F, 0, 0, 255);		// B
		LED_set_color(5, 0x0F, 0, 255, 0);		// V
		LED_set_color(6, 0x0F, 255, 0, 0);		// R
		LED_set_color(7, 0x00, 255, 0, 255);	// Eteint
		LED_set_color(8, 0x0F, 0, 0, 255);		// B
		LED_set_color(9, 0x0F, 255, 0, 255);	// M
		LED_set_color(10, 0x0F, 255, 0, 0);		// R
		LED_set_color(11, 0x0F, 255, 0, 255);	// M
		LED_set_color(12, 0x00, 0, 0, 255);		// Eteint
		LED_set_color(13, 0x0F, 0, 255, 0);		// V
		LED_set_color(14, 0x0F, 0, 0, 255);		// B
		
		
		LED_set_color(15, 0x0F, 0, 0, 255);		// B
		LED_set_color(16, 0x0F, 0, 255, 0);		// V
		LED_set_color(17, 0x00, 255, 0, 0);		// Eteint
		LED_set_color(18, 0x0F, 255, 0, 255);	// M
		LED_set_color(19, 0x0F, 0, 0, 255);		// B
		LED_set_color(20, 0x0F, 0, 255, 0);		// V
		LED_set_color(21, 0x0F, 255, 0, 0);		// R
		LED_set_color(22, 0x00, 255, 0, 255);	// Eteint
		LED_set_color(23, 0x0F, 0, 0, 255);		// B
		LED_set_color(24, 0x0F, 255, 0, 255);	// M
		LED_set_color(25, 0x0F, 255, 0, 0);		// R
		LED_set_color(26, 0x0F, 255, 0, 255);	// M
		LED_set_color(27, 0x00, 0, 0, 255);		// Eteint
		LED_set_color(28, 0x0F, 0, 255, 0);		// V
		LED_set_color(29, 0x0F, 0, 0, 255);		// B
		LED_set_color(30, 0x0F, 255, 0, 0);		// R
		LED_set_color(31, 0x0F, 255, 0, 0);		// R
		
		LED_set_color(32, 0x0F, 0, 0, 255);		// B
		LED_set_color(33, 0x0F, 0, 255, 0);		// V
		LED_set_color(34, 0x00, 255, 0, 0);		// Eteint
		LED_set_color(35, 0x0F, 255, 0, 255);	// M
		LED_set_color(36, 0x0F, 0, 0, 255);		// B
		LED_set_color(37, 0x0F, 0, 255, 0);		// V
		LED_set_color(38, 0x0F, 255, 0, 0);		// R
		LED_set_color(39, 0x00, 255, 0, 255);	// Eteint
		LED_set_color(40, 0x0F, 0, 0, 255);		// B
		LED_set_color(41, 0x0F, 255, 0, 255);	// M
		LED_set_color(42, 0x0F, 255, 0, 0);		// R
		LED_set_color(43, 0x0F, 255, 0, 255);	// M
		LED_set_color(44, 0x00, 0, 0, 255);		// Eteint
		LED_set_color(45, 0x0F, 0, 255, 0);		// V
		LED_set_color(46, 0x0F, 0, 0, 255);		// B
		LED_set_color(47, 0x0F, 255, 0, 0);		// R
		LED_set_color(48, 0x0F, 255, 0, 0);		// R
		LED_set_color(49, 0x0F, 0, 0, 255);		// B
		LED_end_frame();
		LED_send();
		
		osThreadFlagsWait(0x02, osFlagsWaitAny, 2000);
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
	Driver_SPI2.Initialize(spi_callback);
	// Allume le périphérique SPI
	errorCode = Driver_SPI2.PowerControl(ARM_POWER_FULL);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.PowerControl: %u\n", STR_ERROR, errorCode);
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
		fprintf(stderr, "[%s] Driver_SPI2.Control: %u\n", STR_ERROR, errorCode);
		return LED_FALSE;
	}
	
	errorCode = Driver_SPI2.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.Control: %u\n", STR_ERROR, errorCode);
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
	uint16_t index = START_FRAME_LENGHT + (g_LedNumber * 4);
	uint8_t i;
	
	for(i = 0; i < END_FRAME_LENGTH; ++i)
		g_DataFrame[index + i] = 0x00;
}

void LED_clear() {
	uint8_t i;
	
	// Rempli la LED Frame
	for(i = 0; i < g_LedNumber; ++i)
		LED_set_color(i, 0, 100, 200, 50);
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
	uint16_t size = START_FRAME_LENGHT + (g_LedNumber * 4) + END_FRAME_LENGTH;
	printf("size: %d\n", size);
	
	errorCode = Driver_SPI2.Send(g_DataFrame, size);
	if(errorCode != 0) {
		fprintf(stderr, "[%s] Driver_SPI2.Send: %u\n", STR_ERROR, errorCode);
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
