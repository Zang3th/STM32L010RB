#include "dht.h"

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

		Temperature Sensor (AM2302):
			PA6:		Data-bus
*/

// ----- Defines -----

#define DATA GPIOA, GPIO_PIN_6

// ----- Variables -----

typedef enum {
	OUTPUT = 0,
	INPUT = 1
} PIN_MODE_t;

// ----- Private functions ----- 

static void setPinAsOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void setPinAsInput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void startTransmission()
{		
	HAL_GPIO_WritePin(DATA, 0); //Set pin low	
	UT_Delay_MicroSeconds(1000); //Wait 1ms	

	HAL_GPIO_WritePin(DATA, 1); //Set pin high	
	UT_Delay_MicroSeconds(20); //Wait 30us
}

static bool readDataLine()
{
	return (1 & (HAL_GPIO_ReadPin(DATA)));
}

static uint8_t checkResponse()
{		
	setPinAsInput(); //Change pin to input

	uint8_t response = 0;

	UT_Delay_MicroSeconds(40); //Wait 40us

	if(!(readDataLine())) //If the pin is low
	{
		UT_Delay_MicroSeconds(80); //Wait 80 us

		if((readDataLine())) //If the pin is high -> response is ok
			response = 1;
		else
			response = -1;
	}

	while(readDataLine()); //Wait for pin to go low
	return response;
}

// ----- Public Functions -----

void DHT_Init()
{	
	setPinAsOutput(); //Set pin as output	
	HAL_GPIO_WritePin(DATA, 1); //Set pin high -> default state
}

void DHT_ReadData(uint16_t* temp, uint16_t* humidity)
{
	startTransmission();

	uint8_t response = checkResponse();

	if(response == 0)
		UT_printf("Pin war nach 40usec nicht low!\r\n");
	else if(response == 1)
		UT_printf("Alles ok!\r\n");
	else if(response == -1)
		UT_printf("Pin war nach 120usec nicht high!\r\n");		

	DHT_Init();
}