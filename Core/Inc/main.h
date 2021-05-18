#ifndef MAIN
#define MAIN

#include "stm32l0xx_hal.h"
#include "utility.h"
#include "lcd.h"
#include "dht.h"
#include <stdio.h>

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern void UT_Error_Handler(char* err_msg);

#endif