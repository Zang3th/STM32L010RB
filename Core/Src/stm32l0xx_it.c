#include "main.h"
#include "stm32l0xx_it.h"

extern TIM_HandleTypeDef htim2;

void NMI_Handler(void)
{
  while (1)
  {

  }
}

void HardFault_Handler(void)
{
  while (1)
  {

  }
}

void SVC_Handler(void)
{

}


void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}
