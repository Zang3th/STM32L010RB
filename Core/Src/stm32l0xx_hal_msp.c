#include "main.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
    //Peripheral clock enable
    __HAL_RCC_TIM2_CLK_ENABLE();

    //Enable timed interrupts based on timer 2
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
  else if(htim_base->Instance==TIM21)
  {
    //Peripheral clock enable
    __HAL_RCC_TIM21_CLK_ENABLE();
  }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
    //Peripheral clock disable
    __HAL_RCC_TIM2_CLK_DISABLE();

    //Disable timed interrupts based on timer 2
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
  else if(htim_base->Instance==TIM21)
  {
    //Peripheral clock disable
    __HAL_RCC_TIM21_CLK_DISABLE();
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE();

    //USART2 GPIO Configuration
    //PA2     ------> USART2_TX
    //PA3     ------> USART2_RX
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
    //Peripheral clock disable
    __HAL_RCC_USART2_CLK_DISABLE();

    //USART2 GPIO Configuration
    //PA2     ------> USART2_TX
    //PA3     ------> USART2_RX
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
  }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{

}