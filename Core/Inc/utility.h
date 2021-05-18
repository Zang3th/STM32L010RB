#ifndef UTILITY
#define UTILITY

#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim21;

void UT_PrintMsg(char* msg);
void UT_printf(const char* format, ...);
void UT_Error_Handler(char* err_msg);
void UT_Delay_MicroSeconds(uint16_t uSec);

#endif