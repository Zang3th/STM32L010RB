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
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port B
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
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

void TFT_SendDataWithCS(uint8_t command)
{
    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    //Begin write
    HAL_GPIO_WritePin(WR, 0);
    UT_Delay_MicroSeconds(10);

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)

    //End write
    HAL_GPIO_WritePin(WR, 1);

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_SendDataWithoutCS(uint8_t command)
{
    //Begin write
    GPIOA->BRR = GPIO_PIN_8;

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)
   
    //End write
    GPIOA->BSRR = GPIO_PIN_8;
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

    //Display function control
    TFT_SendCmd(0xB6);
    TFT_SendDataWithCS(0x00); //Normal scan mode, positive polarity    
    TFT_SendDataWithCS(0x80); //Normally white LCD, 1 frame scan cycle

    //Memory access control
    TFT_SendCmd(0x36);
    TFT_SendDataWithCS(0x0A); //Set RGB

    //Pixel format
    TFT_SendCmd(0x3A);
    TFT_SendDataWithCS(0x55); //65K color: 16-bit/pixel (RGB565)
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
    TFT_SendCmd(0x2C);

    r >>= 3; //Red := Highest 5 bits
    g >>= 2; //Green := Highest 6 bits
    b >>= 3; //Blue := Highest 5 bits

    uint8_t byte_1 = (r << 3) | (g >> 3);   
    uint8_t byte_2 = (g & 0x07) << 5 | b;    

    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            TFT_SendDataWithoutCS(byte_1);
            TFT_SendDataWithoutCS(byte_2);
        }
    }

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}