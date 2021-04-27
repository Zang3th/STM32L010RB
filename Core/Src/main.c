#include "main.h"
#include <stdio.h>
#include <string.h>

/* Static PIN/PORT LOOK-UP
	A:
		PA5: Onboard-LED
	C:
		PC13: Blue push button	
*/

// ----- Variables ----- 

TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc;

// ----- Functions ----- 

static void Print(char* msg)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

void Error_Handler(char* err_msg)
{
     __disable_irq();
    while (1)
	{
    	Print(err_msg);
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  //Configure the main internal regulator output voltage
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  //Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler("HAL_RCC_OscConfig failed!");
  }
  
  //Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler("HAL_RCC_ClockConfig failed!");
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
		Error_Handler("HAL_UART_Init failed!");
	}
}

static void MX_TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	//Configure timer 2
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 209; //2.097 MHZ divided by 209 results in 10000 oscillations per second
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 100; //Counter goes up to 100, then starts anew. This results in 100 counter restarts per second
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler("HAL_TIM_Base_Init failed!");
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; //Use internal clock as a clock source (at 2 MHZ, max. 32 MHZ)
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler("HAL_TIM_ConfigClockSource failed!");
	}

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  	{
   		Error_Handler("HAL_TIM_PWM_Init failed!");
  	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler("HAL_TIMEx_MasterConfigSynchronization failed!");
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  	sConfigOC.Pulse = 50;
  	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  	{
   		Error_Handler("HAL_TIM_PWM_ConfigChannel failed!");
  	}

	HAL_TIM_MspPostInit(&htim2);
}

static void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  //Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = ENABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler("HAL_ADC_Init failed!");
  }

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler("HAL_ADC_ConfigChannel failed!");
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
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Input-Pins of Port C
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//Timed interupt callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{	
	//Variables to keep track of the time and the ADC value
	static unsigned int elapsedTime = 0;
	static uint32_t val = 0;
	static char buffer[15];
	static uint32_t pulse = 0;
		
	//Get ADC value
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 100);
	val = HAL_ADC_GetValue(&hadc);
	sprintf(buffer, "value = %lu\r\n", val);
	Print(buffer);
	
	//Check if 1 sec. elapsed
	if((elapsedTime % 10000) == 0)
	{
		//Toggle Onboard-LED (1 sec. on and 1 sec. off)
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);			
	}	

	//Increment elapsed time
	elapsedTime += 100;

	if(elapsedTime > 10000)
		elapsedTime -= 10000;
	
	//Convert ADC value into a usable pulse width
	pulse = val / 40;
	if(pulse > 100)
		pulse = 100;
	else if(pulse < 2)
		pulse = 0;	

	//Update PWM with new pulse width
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
	sprintf(buffer, "pulse = %lu\r\n", pulse);
	Print(buffer);
}

int main(void)
{
	//Init stuff
	HAL_Init();
	SystemClock_Config();
	Port_Init();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	MX_ADC_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	while (1)
	{

	}
}