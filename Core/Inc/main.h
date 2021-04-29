#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l0xx_hal.h"
#include "utility.h"
#include "lcd.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern void Error_Handler(UART_HandleTypeDef* huart, char* err_msg);

#endif