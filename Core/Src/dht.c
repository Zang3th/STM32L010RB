#include "dht.h"

// ----- Private functions ----- 

static void DHT_PinAsOutput(dht_t* dht)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = dht->pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;	
	
	HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
}

static void DHT_PinAsInput(dht_t* dht)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = dht->pin;	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
}

static void DHT_WritePin(dht_t* dht, GPIO_PinState state)
{
    HAL_GPIO_WritePin(dht->port, dht->pin, state);
}

static GPIO_PinState DHT_ReadPin(dht_t* dht)
{
    return HAL_GPIO_ReadPin(dht->port, dht->pin);
}

static uint8_t DHT_Read_Byte(dht_t* dht)
{
    uint8_t val = 0, j;
	for(j = 0; j < 8; j++)
	{
		while(!(DHT_ReadPin(dht)));  		//Wait for pin to go high
		UT_Delay_MicroSeconds(50);         		//Wait 50 us
		if (!(DHT_ReadPin(dht)))     		//If the pin is low
		{
			val &= ~(1 << (7 - j));  			//Write 0
		}
		else 
        {
            val |= (1 << (7 - j));    			//Write 1
		    while ((DHT_ReadPin(dht)));  	//Wait for the pin to go low
        }
    }
	return val;
}

static void DHT_ReadByteToBuffer_Debug(dht_t* dht, uint8_t* buffer)
{
	for(uint8_t j = 0; j < 8; j++)
	{
		while(!(DHT_ReadPin(dht)));  		//Wait for pin to go high
		UT_Delay_MicroSeconds(50);         	//Wait 50 us

		if (!(DHT_ReadPin(dht)))     		//If the pin is low
		{
			buffer[j] = 0;  				//Write 0
		}
		else 
        {
            buffer[j] = 1;    				//Write 1			
		    while ((DHT_ReadPin(dht)));  	//Wait for the pin to go low
        }
    }
}

static void DHT_PrintByteBuffer_Debug(uint8_t* buffer, const char* name)
{
	UT_printf("%s: ", name);
	for(uint8_t j = 0; j < 8; j++)
	{
		UT_printf("%d", buffer[j]);
	}

	UT_printf("\n\r");
}

static uint8_t DHT_ConvertByteBuffer_Debug(uint8_t* buffer)
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

// ----- Public Functions -----

void DHT_StartTransmission(dht_t* dht)
{
    DHT_PinAsOutput(dht);          	//Set pin as output
	DHT_WritePin(dht, 0);   	        //Set pin low	
    UT_Delay_MicroSeconds(2000);   	//Wait 2ms
    DHT_PinAsInput(dht);            //Set pin as input
	UT_Delay_MicroSeconds(20);      //Wait 20us
}

uint8_t DHT_CheckResponse(dht_t* dht)
{
    uint8_t response = 1;
	UT_Delay_MicroSeconds(40);      	//Wait 40us

	if(!(DHT_ReadPin(dht)))   	//If the pin is low
	{
		UT_Delay_MicroSeconds(80); 		//Wait 80us
		 
		if((DHT_ReadPin(dht))) 	//If the pin is high -> response is ok
		{
			response = 0;
		}			
		else
		{
			response = 2;
		}
	} 

	while(DHT_ReadPin(dht));  	//Wait for pin to go low
	return response;
}

uint8_t DHT_ReadData(dht_t* dht, uint16_t* humidity, uint16_t* temperature)
{
    uint8_t h1, h2, t1, t2, cs;

	//Read raw binary data
	h1 = DHT_Read_Byte(dht);
	h2 = DHT_Read_Byte(dht);
	t1 = DHT_Read_Byte(dht);
	t2 = DHT_Read_Byte(dht);
	cs = DHT_Read_Byte(dht);

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

void DHT_ReadData_Debug(dht_t* dht)
{
    uint8_t h1[8], h2[8], t1[8], t2[8], cs[8];

	//Read raw binary data
	DHT_ReadByteToBuffer_Debug(dht, h1);
	DHT_ReadByteToBuffer_Debug(dht, h2);
	DHT_ReadByteToBuffer_Debug(dht, t1);
	DHT_ReadByteToBuffer_Debug(dht, t2);
	DHT_ReadByteToBuffer_Debug(dht, cs);	

	//Print raw binary data  
	UT_printf("\n\r--------- DEBUG -----------\n\r");
	UT_printf("----- %s -----\n\r", dht->name);  
	DHT_PrintByteBuffer_Debug(h1, "h1_bin");
	DHT_PrintByteBuffer_Debug(h2, "h2_bin");
	DHT_PrintByteBuffer_Debug(t1, "t1_bin");
	DHT_PrintByteBuffer_Debug(t2, "t2_bin");
	DHT_PrintByteBuffer_Debug(cs, "cs_bin");

	//Convert data to uint8_t
	uint8_t h1_dec, h2_dec, t1_dec, t2_dec, cs_dec;
	h1_dec = DHT_ConvertByteBuffer_Debug(h1);
	h2_dec = DHT_ConvertByteBuffer_Debug(h2);
	t1_dec = DHT_ConvertByteBuffer_Debug(t1);
	t2_dec = DHT_ConvertByteBuffer_Debug(t2);	
	cs_dec = DHT_ConvertByteBuffer_Debug(cs);

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
	UT_printf("---------------------------\n\r");
}