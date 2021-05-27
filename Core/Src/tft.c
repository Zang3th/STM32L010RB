#include "tft.h"

// ----- Defines -----

#define RST GPIOA, GPIO_PIN_5   //Reset
#define CS  GPIOA, GPIO_PIN_6   //Chip select (active low)
#define RS  GPIOA, GPIO_PIN_7   //Register select / Data or Command
#define WR  GPIOA, GPIO_PIN_8   //Write (on rising edge)
#define RD  GPIOA, GPIO_PIN_9   //Read (on rising edge)
#define D0  GPIOB, GPIO_PIN_0
#define D1  GPIOB, GPIO_PIN_1
#define D2  GPIOB, GPIO_PIN_2
#define D3  GPIOB, GPIO_PIN_3
#define D4  GPIOB, GPIO_PIN_4
#define D5  GPIOB, GPIO_PIN_5
#define D6  GPIOB, GPIO_PIN_6
#define D7  GPIOB, GPIO_PIN_7

// ----- Private functions ----- 

void TFT_Reset()
{
    HAL_GPIO_WritePin(RST, 1);
    HAL_GPIO_WritePin(CS, 1);
    HAL_GPIO_WritePin(RS, 1);
    HAL_GPIO_WritePin(WR, 1);
    HAL_GPIO_WritePin(RD, 1);
    HAL_GPIO_WritePin(D0, 0);
    HAL_GPIO_WritePin(D1, 0);
    HAL_GPIO_WritePin(D2, 0);
    HAL_GPIO_WritePin(D3, 0);
    HAL_GPIO_WritePin(D4, 0);
    HAL_GPIO_WritePin(D5, 0);
    HAL_GPIO_WritePin(D6, 0);
    HAL_GPIO_WritePin(D7, 0);
}

void TFT_PortInit()
{
    //Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port A
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port B
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// ----- Public Functions ----- 

void TFT_Init()
{
    TFT_PortInit();
    TFT_Reset();

    //Software reset
    HAL_GPIO_WritePin(CS, 0);   //Begin transmission
    HAL_GPIO_WritePin(RS, 0);   //Begin writing command 
    HAL_GPIO_WritePin(WR, 0);
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(D0, 1);   //Set data
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(WR, 1);   //Send data    
    HAL_GPIO_WritePin(RS, 1);   //End writing command 
    UT_Delay_MicroSeconds(20);
    HAL_GPIO_WritePin(CS, 1);   //End transmission    
    TFT_Reset();

    HAL_Delay(50);

    //Exit sleep
    HAL_GPIO_WritePin(CS, 0);   //Begin transmission
    HAL_GPIO_WritePin(RS, 0);   //Begin writing command 
    HAL_GPIO_WritePin(WR, 0);
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(D0, 1);   //Set data
    HAL_GPIO_WritePin(D4, 1);   //Set data
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(WR, 1);   //Send data    
    HAL_GPIO_WritePin(RS, 1);   //End writing command 
    UT_Delay_MicroSeconds(20);
    HAL_GPIO_WritePin(CS, 1);   //End transmission    
    TFT_Reset();

    HAL_Delay(50);
}

void TFT_TurnDisplayOn()
{
    HAL_GPIO_WritePin(CS, 0);   //Begin transmission
    HAL_GPIO_WritePin(RS, 0);   //Begin writing command 
    HAL_GPIO_WritePin(WR, 0);
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(D0, 1);   //Set data
    HAL_GPIO_WritePin(D3, 1);   //Set data
    HAL_GPIO_WritePin(D5, 1);   //Set data
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(WR, 1);   //Send data    
    HAL_GPIO_WritePin(RS, 1);   //End writing command 
    UT_Delay_MicroSeconds(20);
    HAL_GPIO_WritePin(CS, 1);   //End transmission    
    TFT_Reset();
}

void TFT_TurnDisplayOff()
{
    HAL_GPIO_WritePin(CS, 0);   //Begin transmission
    HAL_GPIO_WritePin(RS, 0);   //Begin writing command 
    HAL_GPIO_WritePin(WR, 0);
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(D3, 1);   //Set data
    HAL_GPIO_WritePin(D5, 1);   //Set data
    UT_Delay_MicroSeconds(20);

    HAL_GPIO_WritePin(WR, 1);   //Send data    
    HAL_GPIO_WritePin(RS, 1);   //End writing command 
    UT_Delay_MicroSeconds(20);
    HAL_GPIO_WritePin(CS, 1);   //End transmission    
    TFT_Reset();
}