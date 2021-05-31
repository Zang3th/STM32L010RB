#ifndef TFT
#define TFT

#include "stm32l0xx_hal.h"
#include "utility.h"

// ----- Defines -----

#define RST GPIOA, GPIO_PIN_5   //Reset
#define CS  GPIOA, GPIO_PIN_6   //Chip select (active low)
#define RS  GPIOA, GPIO_PIN_7   //Register select (1 := RAM data or command parameter and 0 := command)
#define WR  GPIOA, GPIO_PIN_8   //Write (on rising edge)
#define RD  GPIOA, GPIO_PIN_9   //Read (on rising edge)

#define WIDTH  320
#define HEIGHT 240

#define LCD_RED     0xf800
#define LCD_GREEN   0x07e0
#define LCD_BLUE    0x001f
#define LCD_BLACK   0x0000
#define LCD_WHITE   0xffff
#define LCD_GREY    0x8410
#define LCD_YELLOW  0xFFE0
#define LCD_ORANGE  0xFD20
#define LCD_CYAN    0x07FF
#define LCD_PINK    0xF81F
#define LCD_PURPLE  0x8010

void TFT_Init();
void TFT_TurnDisplayOn();
void TFT_TurnDisplayOff();
void TFT_SetDisplayColor24(uint8_t r, uint8_t g, uint8_t b);
void TFT_SetDisplayColor16(uint16_t color);
void TFT_TestDisplayColors();
void TFT_DrawRect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color);
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

#endif