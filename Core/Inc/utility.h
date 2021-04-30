#ifndef UTILITY
#define UTILITY

#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

void Print_msg(char* msg);
void Print_int8_t(char* name, uint8_t value);
void Print_int16_t(char* name, uint16_t value);
void Print_int32_t(char* name, uint32_t value);
void Error_Handler(char* err_msg);

#endif