#ifndef SEVEN_SEGMENT
#define SEVEN_SEGMENT

#include "stm32l0xx_hal.h"

void segment_Init();
void segment_Display(char character);

#endif