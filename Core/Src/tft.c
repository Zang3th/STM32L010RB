#include "tft.h"

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

void TFT_SendData(uint8_t command)
{
    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    //Begin write
    HAL_GPIO_WritePin(WR, 0);

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)

    //End write
    HAL_GPIO_WritePin(WR, 1);

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_SendRaw(uint8_t command)
{
    //Begin write
    GPIOA->BRR = GPIO_PIN_8;

    //Set data
    GPIOB->BRR = 0x00ff;                //Clear lower 8 bits                (GPIO bit reset register)
    GPIOB->BSRR |= (command & 0x00ff);  //Set lower 8 bits to command       (GPIO port bit set/reset register)
   
    //End write
    GPIOA->BSRR = GPIO_PIN_8;
}

uint16_t TFT_ConvToRGB565(uint8_t r, uint8_t g, uint8_t b)
{
    r >>= 3; //Red := Highest 5 bits
    g >>= 2; //Green := Highest 6 bits
    b >>= 3; //Blue := Highest 5 bits

    uint8_t byte_1 = (r << 3) | (g >> 3);   
    uint8_t byte_2 = (g & 0x07) << 5 | b;  

    return (byte_1 << 8) | byte_2; 
}

void TFT_SetArea(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    TFT_SendCmd(0x2a);
    HAL_GPIO_WritePin(CS, 0); //Begin transmission
    TFT_SendRaw(xStart >> 8);
    TFT_SendRaw(xStart);
    TFT_SendRaw(xEnd >> 8);
    TFT_SendRaw(xEnd);
    HAL_GPIO_WritePin(CS, 1); //End transmission

    TFT_SendCmd(0x2b);
    HAL_GPIO_WritePin(CS, 0); //Begin transmission
    TFT_SendRaw(yStart >> 8);
    TFT_SendRaw(yStart);
    TFT_SendRaw(yEnd >> 8);
    TFT_SendRaw(yEnd);
    HAL_GPIO_WritePin(CS, 1); //End transmission
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
    TFT_SendData(0x00); //Normal scan mode, positive polarity    
    TFT_SendData(0x80); //Normally white LCD, 1 frame scan cycle

    //Memory access control
    TFT_SendCmd(0x36);
    TFT_SendData(0x3A); //Row/Column Exchange, Vertical Refresh Order, RGB

    //Pixel format
    TFT_SendCmd(0x3A);
    TFT_SendData(0x55); //65K color: 16-bit/pixel (RGB565)
}

void TFT_TurnDisplayOn()
{
    TFT_SendCmd(0x29);
}

void TFT_TurnDisplayOff()
{
    TFT_SendCmd(0x28);
}

void TFT_SetDisplayColor24(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = TFT_ConvToRGB565(r, g, b);

    //Set area to cover whole display
    TFT_SetArea(0, 0, WIDTH, HEIGHT);

    //Memory write
    TFT_SendCmd(0x2C); 

    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    for(uint16_t i = 0; i < HEIGHT; i++)
    {
        for(uint16_t j = 0; j < WIDTH; j++)
        {
            TFT_SendRaw(color >> 8);
            TFT_SendRaw(color);
        }
    }

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_SetDisplayColor16(uint16_t color)
{
    //Set area to cover whole display
    TFT_SetArea(0, 0, WIDTH, HEIGHT);

    //Memory write
    TFT_SendCmd(0x2C); 

    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    for(uint16_t i = 0; i < HEIGHT; i++)
    {
        for(uint16_t j = 0; j < WIDTH; j++)
        {
            TFT_SendRaw(color >> 8);
            TFT_SendRaw(color);
        }
    }

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_TestDisplayColors()
{
    TFT_SetDisplayColor16(LCD_RED);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_GREEN); 
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_BLUE);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_BLACK);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_WHITE);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_GREY);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_YELLOW);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_ORANGE);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_CYAN);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_PINK);
	HAL_Delay(500);
	TFT_SetDisplayColor16(LCD_PURPLE);
	HAL_Delay(500);
}

void TFT_DrawRect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color)
{
    TFT_SetArea(xStart, yStart, xStart + width - 1, yStart + height - 1);

    //Memory write
    TFT_SendCmd(0x2C); 

    //Begin transmission
    HAL_GPIO_WritePin(CS, 0);

    for(uint16_t i = 0; i < height; i++)
    {
        for(uint16_t j = 0; j < width; j++)
        {
            TFT_SendRaw(color >> 8);
            TFT_SendRaw(color);
        }
    }

    //End transmission
    HAL_GPIO_WritePin(CS, 1);
}

void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    TFT_DrawRect(x, y, 1, 1, color);
}