#ifndef DHT11
#define DHT11

#include "stm32l0xx_hal.h"
#include "utility.h"

void DHT22_StartTransmission();
int8_t DHT22_CheckResponse();
uint8_t DHT22_Read_Byte();
void DHT22_ReadByteToBuffer(uint8_t* buffer);
void DHT22_ReadDataToBuffersDebug(uint8_t* h1, uint8_t* h2, uint8_t* t1, uint8_t* t2, uint8_t* cS);

#endif