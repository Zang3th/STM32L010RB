#ifndef UTILITY
#define UTILITY

#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;

void UT_printf(const char* format, ...);
void UT_Error_Handler(char* err_msg);
void UT_Delay_MicroSeconds(uint32_t uSec);

#endif