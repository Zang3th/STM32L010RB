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
			PA7:		VCC
			PA6:		Data-bus
*/

// ----- Defines -----

#define DATA GPIOA, GPIO_PIN_6

// ----- Variables -----

static uint8_t byteBuffer[32];

typedef enum{
	OUTPUT = 0,
	INPUT = 1
} PIN_MODE_t;

// ----- Private functions ----- 

//Change pin mode
static void setPinMode(PIN_MODE_t mode)
{
	//Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	if(mode == OUTPUT)
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	else if(mode == INPUT)
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void startTransmission()
{
	//Set pin low
	HAL_GPIO_WritePin(DATA, 0);

	//Wait 1ms
	UT_Delay_MicroSeconds(1000);

	//Set pin high
	HAL_GPIO_WritePin(DATA, 1);

	//Wait 30us
	UT_Delay_MicroSeconds(30);

	//Change pin to input
	setPinMode(INPUT);
}

static bool readDataLine()
{
	return (1 & (HAL_GPIO_ReadPin(DATA)));
}

// ----- Public Functions -----

void DHT_Init()
{
	//Set pin as output
	setPinMode(OUTPUT);

	//Set pin high -> default state
	HAL_GPIO_WritePin(DATA, 1);
}

void DHT_ReadData()
{
	__disable_irq();

	startTransmission();

	//Wait 40us
	UT_Delay_MicroSeconds(40);

 	//Wait while data line is low
	while(!readDataLine());
	
	//Wait while data line is high
	while(readDataLine());
	/*
	for(uint8_t i = 0; i < 32; i++)
	{
		//Wait while data line is low
		while(!readDataLine());

		//Wait 40us
		UT_Delay_MicroSeconds(40);

		//Read data line
		GPIO_PinState state = HAL_GPIO_ReadPin(DATA);

		//If line is still high then its 1, else its 0
		if(state == 1)
			byteBuffer[i] = 1;
		else
			byteBuffer[i] = 0;

		//Wait while data line is high		
		while(readDataLine());	
	}  */

	__enable_irq();

	UT_Delay_MicroSeconds(20000);
	DHT_Init();	

	/* for(uint8_t i = 0; i < 32; i++)
	{
		UT_printf("Bit %d = %d\r\n", i, byteBuffer[i]);
	} */	
}