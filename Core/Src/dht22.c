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

static uint8_t DHT22_ConvertByteBuffer(uint8_t* buffer)
{
	uint8_t val = 0;
	for(uint8_t i = 0; i < 8; i++)
	{		
		if(buffer[i] == 1)
		{
			val |= (1 << (7 - i));			
		}
	}
	return val;
}

static uint8_t DHT22_Read_Byte()
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

static void DHT22_ReadByteToBuffer(uint8_t* buffer)
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

// ----- Public Functions -----

void DHT22_StartTransmission()
{		
    DHT22_PinAsOutput();          	//Set pin as output
	HAL_GPIO_WritePin(DATA, 0);   	//Set pin low	
    UT_Delay_MicroSeconds(2000);   	//Wait 2ms
    DHT22_PinAsInput();             //Set pin as input
	UT_Delay_MicroSeconds(20);      //Wait 20us
}

uint8_t DHT22_CheckResponse()
{
    uint8_t response = 1;
	UT_Delay_MicroSeconds(40);      	//Wait 40us

	if(!(HAL_GPIO_ReadPin(DATA)))   	//If the pin is low
	{
		UT_Delay_MicroSeconds(80); 		//Wait 80us
		 
		if((HAL_GPIO_ReadPin(DATA))) 	//If the pin is high -> response is ok
		{
			response = 0;
		}			
		else
		{
			response = 2;
		}
	} 

	while(HAL_GPIO_ReadPin(DATA));  	//Wait for pin to go low

	return response;
}

void DHT22_ReadDataDebug()
{
	uint8_t h1[8], h2[8], t1[8], t2[8], cs[8];

	//Read raw binary data
	DHT22_ReadByteToBuffer(h1);
	DHT22_ReadByteToBuffer(h2);
	DHT22_ReadByteToBuffer(t1);
	DHT22_ReadByteToBuffer(t2);
	DHT22_ReadByteToBuffer(cs);	

	//Print raw binary data
	UT_printf("\n\r----- DEBUG -----\n\r");
	DHT22_PrintByteBuffer(h1, "h1_bin");
	DHT22_PrintByteBuffer(h2, "h2_bin");
	DHT22_PrintByteBuffer(t1, "t1_bin");
	DHT22_PrintByteBuffer(t2, "t2_bin");
	DHT22_PrintByteBuffer(cs, "cs_bin");

	//Convert data to uint8_t
	uint8_t h1_dec, h2_dec, t1_dec, t2_dec, cs_dec;
	h1_dec = DHT22_ConvertByteBuffer(h1);
	h2_dec = DHT22_ConvertByteBuffer(h2);
	t1_dec = DHT22_ConvertByteBuffer(t1);
	t2_dec = DHT22_ConvertByteBuffer(t2);	
	cs_dec = DHT22_ConvertByteBuffer(cs);

	//Calculate checksum
	uint8_t myChecksum = h1_dec + h2_dec + t1_dec + t2_dec;

	//Print decimal data	
	UT_printf("\n\r");
	UT_printf("h1_dec: %d\n\r", h1_dec);
	UT_printf("h2_dec: %d\n\r", h2_dec);
	UT_printf("t1_dec: %d\n\r", t1_dec);
	UT_printf("t2_dec: %d\n\r", t2_dec);
	UT_printf("checksum: %d\n\r", cs_dec);	
	UT_printf("myChecksum: %d\n\r", myChecksum);

	//Combine data to uint16_t
	uint16_t humi_uint = 0, temp_uint = 0;
	humi_uint = (h1_dec << 8) | h2_dec;
	temp_uint = (t1_dec << 8) | t2_dec;

	//Print combined values		
	UT_printf("\n\r");
	UT_printf("Humidity: %d.%d%%\n\r", humi_uint / 10, humi_uint % 10);
	UT_printf("Temperature: %d.%d\n\r", temp_uint / 10, temp_uint % 10);	
	UT_printf("-----------------\n\r");
}

uint8_t DHT22_ReadData(uint16_t* humidity, uint16_t* temperature)
{
	uint8_t h1, h2, t1, t2, cs;

	//Read raw binary data
	h1 = DHT22_Read_Byte();
	h2 = DHT22_Read_Byte();
	t1 = DHT22_Read_Byte();
	t2 = DHT22_Read_Byte();
	cs = DHT22_Read_Byte();

	//Calculate checksum
	uint8_t myChecksum = h1 + h2 + t1 + t2;

	if(myChecksum == cs)
	{
		//Combine data to uint16_t
		*humidity = (h1 << 8) | h2;
		*temperature = (t1 << 8) | t2;

		return 0;
	}

	return 1;
}