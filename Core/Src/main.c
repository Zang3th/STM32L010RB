#include "main.h"
#include <stdio.h>
#include <string.h>

/* Static PIN/PORT LOOK-UP
	A:
		PA5: Onboard-LED
	C:
		PC13: Blue push button	
*/

// ----- Defines -----

#define SEGMENT_A GPIOA, GPIO_PIN_8
#define SEGMENT_B GPIOB, GPIO_PIN_10
#define SEGMENT_C GPIOB, GPIO_PIN_4
#define SEGMENT_D GPIOB, GPIO_PIN_5
#define SEGMENT_E GPIOB, GPIO_PIN_3
#define SEGMENT_F GPIOA, GPIO_PIN_10
#define SEGMENT_G GPIOC, GPIO_PIN_7
#define SEGMENT_DP GPIOA, GPIO_PIN_9

// ----- Variables ----- 

TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

// ----- Functions ----- 

void Print(char* msg)
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

	//Configure timer 2
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 2097; //2.097 MHZ divided by 2097 results in 1000 oscillations per second
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 100; //Counter goes up to 100, then starts anew. This results in 10 counter restarts per second
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

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler("HAL_TIMEx_MasterConfigSynchronization failed!");
	}
}

static void PortInit(void)
{
	//Enable GPIO Ports Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	//Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port A
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port B
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port C
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	//Initialize all Input-Pins of Port C
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void ShowOnSegment(unsigned int counter)
{
	if(counter == 0)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_SET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 1)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_SET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 2)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 3)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 4)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 5)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 6)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 7)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_SET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 8)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
	if(counter == 9)
	{
		HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
	}
}

//Timed interupt callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{	
	//Variables to keep track of the time and the counter
	static unsigned int elapsedTime = 0;
	static unsigned int counter = 0;

	//Toggle Onboard-LED => 1/2 sec. on and 1/2 sec. off
	if(elapsedTime == 500)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		elapsedTime = 0;
	}

	//Show current counter on 7-Segment-Display
	ShowOnSegment(counter);

	//Check if blue push button is pressed
	GPIO_PinState state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
	if(state == GPIO_PIN_RESET) //Falling edge trigger detection
	{	
		if(counter < 10)	
			counter++;
		else
			counter = 0;		

		Print("Button pressed!");		
	}

	//Increment elapsed time
	elapsedTime += 100;
}

int main(void)
{
	//Init stuff
	HAL_Init();
	SystemClock_Config();
	PortInit();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	HAL_TIM_Base_Start_IT(&htim2);

	while (1)
	{
		
	}
}
