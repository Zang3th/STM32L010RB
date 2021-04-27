#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l0xx_hal.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void Error_Handler(char* err_msg);

#endif