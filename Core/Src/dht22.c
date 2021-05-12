#include "dht22.h"

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

		Temperature Sensor (DHT22):
			PA6:		Data-bus
*/

// ----- Defines -----

#define DATA GPIOA, GPIO_PIN_6

// ----- Private functions ----- 

static void DHT22_PinAsOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;	
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void DHT22_PinAsInput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_6;	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void DHT22_PrintByteBuffer(uint8_t* buffer, const char* name)
{
	UT_printf("%s: ", name);
	for(uint8_t j = 0; j < 8; j++)
	{
		UT_printf("%d", buffer[j]);
	}

	UT_printf("\n\r");
}

uint8_t DHT22_ConvertByteBuffer(uint8_t* buffer)
{
	uint8_t val = 0, i = 0;
	for(i; i < 8; i++)
	{		
		if(buffer[i] == 1)
		{
			val |= (1 << (7 - i));			
		}
	}
	return val;
}

// ----- Public Functions -----

void DHT22_StartTransmission()
{		
    DHT22_PinAsOutput();          	//Set pin as output
	HAL_GPIO_WritePin(DATA, 0);   	//Set pin low	
    UT_Delay_MicroSeconds(2000);   	//Wait ms
    DHT22_PinAsInput();             //Set pin as input
	UT_Delay_MicroSeconds(20);      //Wait 20us
}

int8_t DHT22_CheckResponse()
{
    int8_t response = 0;
	UT_Delay_MicroSeconds(40);      	//Wait 40us

	if(!(HAL_GPIO_ReadPin(DATA)))   	//If the pin is low
	{
		UT_Delay_MicroSeconds(80); 		//Wait 40us

		if((HAL_GPIO_ReadPin(DATA))) 	//If the pin is high -> response is ok
		{
			response = 1;
		}			
		else
		{
			response = -1;
		}
	}

	while(HAL_GPIO_ReadPin(DATA));  	//Wait for pin to go low

	return response;
}

uint8_t DHT22_Read_Byte()
{
    uint8_t val = 0, j;
	for(j = 0; j < 8; j++)
	{
		while(!(HAL_GPIO_ReadPin(DATA)));  		//Wait for pin to go high
		UT_Delay_MicroSeconds(50);         		//Wait 50 us
		if (!(HAL_GPIO_ReadPin(DATA)))     		//If the pin is low
		{
			val &= ~(1 << (7 - j));  			//Write 0
		}
		else 
        {
            val |= (1 << (7 - j));    			//Write 1
		    while ((HAL_GPIO_ReadPin(DATA)));  	//Wait for the pin to go low
        }
    }
	return val;
}

void DHT22_ReadByteToBuffer(uint8_t* buffer)
{
	for(uint8_t j = 0; j < 8; j++)
	{
		while(!(HAL_GPIO_ReadPin(DATA)));  		//Wait for pin to go high
		UT_Delay_MicroSeconds(50);         		//Wait 50 us

		if (!(HAL_GPIO_ReadPin(DATA)))     		//If the pin is low
		{
			buffer[j] = 0;  					//Write 0
		}
		else 
        {
            buffer[j] = 1;    					//Write 1			
		    while ((HAL_GPIO_ReadPin(DATA)));  	//Wait for the pin to go low
        }
    }
}

void DHT22_ReadDataToBuffersDebug(uint8_t* h1, uint8_t* h2, uint8_t* t1, uint8_t* t2, uint8_t* cS)
{
	//Read raw binary data
	DHT22_ReadByteToBuffer(h1);
	DHT22_ReadByteToBuffer(h2);
	DHT22_ReadByteToBuffer(t1);
	DHT22_ReadByteToBuffer(t2);
	DHT22_ReadByteToBuffer(cS);

	//Print raw binary data
	UT_printf("\n\rOK!\n\r");
	DHT22_PrintByteBuffer(h1, "h1");
	DHT22_PrintByteBuffer(h2, "h2");
	DHT22_PrintByteBuffer(t1, "t1");
	DHT22_PrintByteBuffer(t2, "t2");
	DHT22_PrintByteBuffer(cS, "cS");

	//Convert data to uint8_t
	uint8_t h1_val, h2_val, t1_val, t2_val;
	h1_val = DHT22_ConvertByteBuffer(h1);
	UT_printf("h1_val: %d\n\r", h1_val);
	h2_val = DHT22_ConvertByteBuffer(h2);
	UT_printf("h2_val: %d\n\r", h2_val);
	t1_val = DHT22_ConvertByteBuffer(t1);
	UT_printf("t1_val: %d\n\r", t1_val);
	t2_val = DHT22_ConvertByteBuffer(t2);
	UT_printf("t2_val: %d\n\r", t2_val);

	//Combine data to uint16_t
	uint16_t humi_uint = 0, temp_uint = 0;
	humi_uint = (h1_val << 8) | h2_val;
	temp_uint = (t1_val << 8) | t2_val;

	//Print values
	UT_printf("Humidity: %d.%d%%\n\r", humi_uint / 10, humi_uint % 10);
	UT_printf("Temperature: %d.%d\n\r", temp_uint / 10, temp_uint % 10);
}