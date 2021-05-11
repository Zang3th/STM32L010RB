#ifndef DHT11
#define DHT11

#include "stm32l0xx_hal.h"
#include "utility.h"

void DHT22_StartTransmission();
int8_t DHT22_CheckResponse();
uint16_t DHT22_Read_2Byte();

#endif