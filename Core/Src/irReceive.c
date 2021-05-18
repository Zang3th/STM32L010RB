#include "irReceive.h"

// ----- Private functions ----- 

static GPIO_PinState IR_ReadPin(irReceiver_t* ir)
{
    return HAL_GPIO_ReadPin(ir->port, ir->pin);
}

// ----- Public Functions -----

void IR_Init(irReceiver_t* ir)
{
    //Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port A
	GPIO_InitStruct.Pin = ir->pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ir->port, &GPIO_InitStruct);
}

uint8_t IR_CheckForTransmission(irReceiver_t* ir)
{
    uint8_t response = 1;

    if(!(IR_ReadPin(ir)))           //If the pin is low
    {
        while(!(IR_ReadPin(ir)));   //Wait for pin to go high        
        while(IR_ReadPin(ir));      //Wait for pin to go low

        response = 0;               //Response ok
    }

    return response;
}

uint32_t IR_ReceiveSignal(irReceiver_t* ir)
{
    uint32_t data = 0;
    uint8_t count = 0;

    for (int i = 0; i < 32; i++)
    {
        count = 0;
        while(!(IR_ReadPin(ir)));           //Wait for pin to go high

        while((IR_ReadPin(ir)))             //Count the space length while the pin is high
        {
            count++;
            UT_Delay_MicroSeconds(100);
        }

        if(count > 10)                      //If the space is more than 1ms
        {
            data |= (1UL << (31 - i));      //Write 1
        }

        else
        {
            data &= ~(1UL << (31 - i));     //Write 0
        }            
    }

    return data;
}