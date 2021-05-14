#ifndef DHT11
#define DHT11

#include "stm32l0xx_hal.h"
#include "utility.h"

void DHT22_StartTransmission();
int8_t DHT22_CheckResponse();
void DHT22_ReadDataDebug();
int8_t DHT22_ReadData(uint16_t* humidity, uint16_t* temperature);

#endif