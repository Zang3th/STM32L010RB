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

    //Set default states
    HAL_GPIO_WritePin(RST, 1);
    HAL_GPIO_WritePin(CS, 1);
    HAL_GPIO_WritePin(RS, 1);
    HAL_GPIO_WritePin(WR, 1);
    HAL_GPIO_WritePin(RD, 1);
}

void TFT_SendCmd(uint8_t command)
{
    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);
    HAL_GPIO_WritePin(RS, 0);
    HAL_GPIO_WritePin(WR, 0);

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR = (command & 0x00ff);   //Set lower 8 bits to command       (GPIO port bit set/reset register)

    //End transmission
    HAL_GPIO_WritePin(WR, 1);
    HAL_GPIO_WritePin(RS, 1);
    HAL_GPIO_WritePin(CS, 1);
}

// ----- Public Functions ----- 

void TFT_Init()
{
    TFT_PortInit();

    //Software reset
    TFT_SendCmd(0x01);
    HAL_Delay(50);

    //Exit sleep
    TFT_SendCmd(0x11);
    HAL_Delay(50);   
}

void TFT_TurnDisplayOn()
{
    TFT_SendCmd(0x29);
}

void TFT_TurnDisplayOff()
{
    TFT_SendCmd(0x28);
}