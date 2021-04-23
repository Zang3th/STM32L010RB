#include "main.h"
#include <stdio.h>

// --- Variables

TIM_HandleTypeDef htim2;

// --- Functions

void Error_Handler(void)
{
     __disable_irq();
    while (1)
	{
    	printf("Error!\n");
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	// Configure the main internal regulator output voltage
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	// Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	// Initializes the CPU, AHB and APB buses clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 3000;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 1000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

static void PortInit(void)
{
	// Enable GPIO Ports Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	// Configure and initialize PA5 (PIN 23) -> corresponds to green Onboard-LED
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

static void toggleButtonState(GPIO_PinState* btnState)
{
	if((*btnState) == GPIO_PIN_RESET)
	{
		(*btnState) = GPIO_PIN_SET;
	}
	else if((*btnState) == GPIO_PIN_SET)
	{
		(*btnState) = GPIO_PIN_RESET;
	}
}

static void fetchEvents(GPIO_PinState* btnState)
{
	GPIO_PinState state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

	if(state == GPIO_PIN_RESET)
	{
		HAL_Delay(100);
		state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
		if(state == GPIO_PIN_RESET)
		{
			toggleButtonState(btnState);
			printf("Toggle LED\n");
		}
	}
}

int main(void)
{
	// Variables
	GPIO_PinState buttonState = GPIO_PIN_RESET;

	// Init stuff
	HAL_Init();
	SystemClock_Config();
	PortInit();
	MX_TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);

	// Reset pins
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	while (1)
	{
		fetchEvents(&buttonState);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, buttonState);
	}
}
