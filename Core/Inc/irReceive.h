#ifndef IR_RECEIVE
#define IR_RECEIVE

#include "stm32l0xx_hal.h"
#include "stm32l010xb.h"

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
} irReceiver_t;

#endif