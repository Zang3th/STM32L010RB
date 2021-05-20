#ifndef SEVEN_SEGMENT
#define SEVEN_SEGMENT

#include "stm32l0xx_hal.h"

void Segment_Reset();
void Segment_Init();
void Segment_Display(char character);

#endif