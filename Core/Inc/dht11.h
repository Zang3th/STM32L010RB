#ifndef DHT11
#define DHT11

#include "stm32l0xx_hal.h"
#include "utility.h"

void DHT11_StartTransmission();
uint8_t DHT11_CheckResponse();
uint16_t DHT11_Read_2Byte();

#endif