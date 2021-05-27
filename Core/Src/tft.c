#include "tft.h"

// ----- Defines -----

#define RST GPIOA, GPIO_PIN_5   //Reset
#define CS  GPIOA, GPIO_PIN_6   //Chip select (active low)
#define RS  GPIOA, GPIO_PIN_7   //Register select (1 := RAM data or command parameter and 0 := command)
#define WR  GPIOA, GPIO_PIN_8   //Write (on rising edge)
#define RD  GPIOA, GPIO_PIN_9   //Read (on rising edge)

#define WIDTH  320
#define HEIGHT 240

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
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)

    //End transmission
    HAL_GPIO_WritePin(WR, 1);
    HAL_GPIO_WritePin(RS, 1);
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_SendData(uint8_t command)
{
    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);
    HAL_GPIO_WritePin(WR, 0);

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)

    //End transmission
    HAL_GPIO_WritePin(WR, 1);
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

    //Memory access control
    TFT_SendCmd(0x36);
    TFT_SendData(0x48); //Set RGB

    //Pixel format
    TFT_SendCmd(0x3A);
    TFT_SendData(0x66); //262K color: 18-bit/pixel (RGB666)
}

void TFT_TurnDisplayOn()
{
    TFT_SendCmd(0x29);
}

void TFT_TurnDisplayOff()
{
    TFT_SendCmd(0x28);
}

void TFT_SetColor(uint8_t r, uint8_t g, uint8_t b)
{

}

void TFT_TestDisplay()
{
    TFT_SendCmd(0x2C);

    //Send 3 dummy bytes
    TFT_SendData(0x00);
    TFT_SendData(0x00);    
    TFT_SendData(0x00);  

    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH - 210; j++)
        {
            TFT_SendData(252 & 0xfc);
            TFT_SendData(0x00);
            TFT_SendData(0x00);
        }
    }

    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = WIDTH - 210; j < WIDTH - 110; j++)
        {
            TFT_SendData(0x00);
            TFT_SendData(252 & 0xfc);
            TFT_SendData(0x00);
        }
    }

    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = WIDTH - 110; j < WIDTH; j++)
        {
            TFT_SendData(0x00);
            TFT_SendData(0x00);
            TFT_SendData(252 & 0xfc);
        }
    }
}