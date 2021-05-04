#ifndef DHT
#define DHT

#include "stm32l0xx_hal.h"
#include "utility.h"
#include <stdbool.h>

void DHT_Init();
void DHT_ReadData();

#endif