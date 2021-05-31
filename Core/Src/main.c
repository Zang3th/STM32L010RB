#include "main.h"

/* Static PIN/PORT LOOK-UP
	
	Always:
		USART_TX:		PA2 		(Auto)
		USART_RX:		PA3 		(Auto)

	TFT-Display (2.4"):
		RST:			PA5
		CS:				PA6
		RS:				PA7
		WR:				PA8
		RD:				PA9

		D0:				PB0
		D1:				PB1
		D2:				PB2
		D3:				PB3
		D4:				PB4
		D5:				PB5
		D6:				PB6
		D7:				PB7

	LED:
		VDD:			PB8			(X)	

*/

// ----- Variables ----- 

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;
UART_HandleTypeDef huart2;
SPI_HandleTypeDef hspi1;

// ----- Functions ----- 

static void SystemClock_Config(void)
{
  	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  	//Configure the main internal regulator output voltage
  	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  	//Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure
  	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  	{
  	  	UT_Error_Handler("HAL_RCC_OscConfig failed!");
  	}

  	//Initializes the CPU, AHB and APB buses clocks
  	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  	{
  	  	UT_Error_Handler("HAL_RCC_ClockConfig failed!");
  	}

  	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  	{
  	  	UT_Error_Handler("HAL_RCCEx_PeriphCLKConfig failed!");
  	}
}

static void MX_USART2_UART_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		UT_Error_Handler("HAL_UART_Init failed!");
	}
}

static void MX_TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	//Configure timer 2
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 3200; //32MHZ divided by 3200. This results in 10000 oscillations per second
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 10000; //Counter goes up to 10000, then starts anew. This results in 1 counter restarts every second
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIM_Base_Init failed!");
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; //Use internal clock as a clock source
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIM_ConfigClockSource failed!");
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIMEx_MasterConfigSynchronization failed!");
	}

	HAL_TIM_MspPostInit(&htim2);
}

static void MX_TIM21_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	//Configure timer 21
	htim21.Instance = TIM21;
	htim21.Init.Prescaler = 28; //32MHZ divided by 28. High resolution for microsecond-Delay-Function
	htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim21.Init.Period = 65535;
	htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim21) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIM_Base_Init failed!");
	}	

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIM_ConfigClockSource failed!");
	}	

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK)
	{
		UT_Error_Handler("HAL_TIMEx_MasterConfigSynchronization failed!");
	}
}

static void MX_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; //4 MBits/s
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;

	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		UT_Error_Handler("HAL_SPI_Init failed!");
	}
}

static void Port_Init(void)
{
	//Enable GPIO Ports Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	//Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port B
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//Timed interupt callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{	
	//Toggle Onboard-LED (1 sec. on and 1 sec. off)
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
}

int main(void)
{		
	//Init stuff
	HAL_Init();
	SystemClock_Config();
	Port_Init();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	MX_TIM21_Init();	
	MX_SPI1_Init();

	//Start timers
	HAL_TIM_Base_Start_IT(&htim2);	
	HAL_TIM_Base_Start(&htim21);

	//TFT stuff (Colors are RGB565, 65K color, 16-bit/pixel)
	TFT_Init();
	TFT_TurnDisplayOn();
	
	while (1)
	{			
		TFT_SetColor(0xff, 0x00, 0x00);
		TFT_SetColor(0x00, 0xff, 0x00); 
		TFT_SetColor(0x00, 0x00, 0xff);
	}
}