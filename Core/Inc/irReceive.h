#ifndef IR_RECEIVE
#define IR_RECEIVE

#include "stm32l0xx_hal.h"
#include "stm32l010xb.h"
#include "utility.h"
#include <stdint.h>

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
} irReceiver_t;

void IR_Init(irReceiver_t* ir);
uint8_t IR_CheckForTransmission(irReceiver_t* ir);
uint32_t IR_ReceiveSignal(irReceiver_t* ir);

#endif