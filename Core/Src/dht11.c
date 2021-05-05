#include "dht11.h"

/* Static PIN/PORT LOOK-UP
	
	Always:
		PA2:	USART_TX -> Connected to debugger
		PA3:	USART_RX -> Connected to debugger
		PA5: 	Onboard-LED
		PA15:	Timer 2 PWM
		PC13: 	Blue push button

	Project specific:
		LCD:
			PA0: 		RS
			PA1:		R/W
			PB0 - PB3:	DB0 - DB3
			PB4 - PB7: 	DB4 - DB7
			PB8: 		E

		Temperature Sensor (DHT11):
			PA6:		Data-bus
*/

// ----- Defines -----

#define DATA GPIOA, GPIO_PIN_6

// ----- Private functions ----- 

static void DHT11_PinAsOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void DHT11_PinAsInput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL; //Pullup?

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// ----- Public Functions -----

void DHT11_StartTransmission()
{		
    DHT11_PinAsOutput();            //Set pin as output
	HAL_GPIO_WritePin(DATA, 0);     //Set pin low	
    HAL_Delay(18);	                //Wait 18ms
	HAL_GPIO_WritePin(DATA, 1);     //Set pin high	
	UT_Delay_MicroSeconds(20);      //Wait 20us
    DHT11_PinAsInput();             //Set pin as input
}

uint8_t DHT11_CheckResponse()
{
    uint8_t response = 0;
	UT_Delay_MicroSeconds(40);      //Wait 40us

	if(!(HAL_GPIO_ReadPin(DATA)))   //If the pin is low
	{
		UT_Delay_MicroSeconds(80);  //Wait 80us

		if(HAL_GPIO_ReadPin(DATA)) //If the pin is high -> response is ok
			response = 1;
		else
			response = -1;
	}

	while(HAL_GPIO_ReadPin(DATA));  //Wait for pin to go low
	return response;
}

uint16_t DHT11_Read_2Byte()
{
    uint16_t val = 0, j;
	for(j = 0; j < 16; j++)
	{
		while(!(HAL_GPIO_ReadPin(DATA)));  //Wait for pin to go high
		UT_Delay_MicroSeconds(40);         //Wait 40 us
		if (!(HAL_GPIO_ReadPin(DATA)))     //If the pin is low
		{
			val &= ~(1 << (15 - j));   //Write 0
		}
		else 
        {
            val |= (1 << (15 - j));    //Write 1
		    while ((HAL_GPIO_ReadPin(DATA)));  //Wait for the pin to go low
        }
    }
	return val;
}