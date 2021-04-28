#ifndef UTILITY
#define UTILITY

#include "stm32l0xx_hal.h"
#include <string.h>

void Print(UART_HandleTypeDef* huart, char* msg);
void Error_Handler(UART_HandleTypeDef* huart, char* err_msg);

#endif