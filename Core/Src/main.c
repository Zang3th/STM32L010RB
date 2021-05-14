#include "main.h"

/* Static PIN/PORT LOOK-UP
	
	Always:
		PA2:	USART_TX -> Connected to debugger
		PA3:	USART_RX -> Connected to debugger
		PA5: 	Onboard-LED
		PA15:	Timer 2 PWM
		PC13: 	Blue push button

	Project specific:
		LCD:
			PA0: 		RS
			PA1:		R/W
			PB0 - PB3:	DB0 - DB3
			PB4 - PB7: 	DB4 - DB7
			PB8: 		E

		Temperature Sensor (DHT22):
			PA6:		Data-bus
*/

// ----- Variables ----- 

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;
UART_HandleTypeDef huart2;

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

static void Port_Init(void)
{
	//Enable GPIO Ports Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	//Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port A
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port C
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//Timed interupt callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{	
	//Toggle Onboard-LED (1 sec. on and 1 sec. off)
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

int main(void)
{		
	//Init stuff
	HAL_Init();
	SystemClock_Config();
	Port_Init();
	MX_TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);	
	MX_USART2_UART_Init();
	MX_TIM21_Init();
	HAL_TIM_Base_Start(&htim21);

	//LCD stuff
	HAL_Delay(30);
	LCD_InitPins();
	LCD_Set8BitMode();
	LCD_TurnDisplayOn();
	LCD_ClearDisplay();
	LCD_ReturnHome();
	
	uint16_t humidity = 0, temperature = 0;

	while (1)
	{				
		//Reading DHT
		DHT22_StartTransmission();
		uint8_t response = DHT22_CheckResponse();	 

		if(response == 0)
		{
			int8_t success = DHT22_ReadData(&humidity, &temperature);
			if(success == 0)
			{
				UT_printf("\n\rHumidity: %d.%d%%\n\r", humidity / 10, humidity % 10);
				UT_printf("Temperature: %d.%d\n\r", temperature / 10, temperature % 10);				
				LCD_ClearDisplay();
				LCD_printf("Humidity: %d.%d%%", humidity / 10, humidity % 10);
				LCD_printf("Temp.: %d.%dC", temperature / 10, temperature % 10);
			}
			else			
			{
				UT_printf("\n\rChecksum wrong!\n\r");				
				LCD_ClearDisplay();				
				LCD_printf("Checksum wrong!");
			}		
		}			
		else if(response == 1)
		{			
			UT_printf("\n\rSensor wasn't low after 40us!");				
			LCD_ClearDisplay();				
			LCD_printf("Not low 40us!");				
		}			
		else if(response == 2)
		{
			UT_printf("\n\rSensor wasn't high after 120us!");						
			LCD_ClearDisplay();				
			LCD_printf("Not high 120us!");
		}			
		
		HAL_Delay(3000);
	}
}