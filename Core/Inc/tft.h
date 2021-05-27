#ifndef TFT
#define TFT

#include "stm32l0xx_hal.h"
#include "utility.h"

void TFT_Init();
void TFT_TurnDisplayOn();
void TFT_TurnDisplayOff();
void TFT_SetColor(uint8_t r, uint8_t g, uint8_t b);
void TFT_TestDisplay();

#endif