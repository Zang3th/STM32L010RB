#ifndef DHT
#define DHT

#include "stm32l0xx_hal.h"
#include "stm32l010xb.h"
#include "utility.h"

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
} dht_t;

void DHT_StartTransmission(dht_t* dht);
uint8_t DHT_CheckResponse(dht_t* dht);
uint8_t DHT_ReadData(dht_t* dht, uint16_t* humidity, uint16_t* temperature);
void DHT_ReadData_Debug(dht_t* dht);

#endif