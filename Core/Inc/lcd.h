#ifndef LCD
#define LCD

#include "stm32l0xx_hal.h"
#include "utility.h"
#include <string.h>
#include <stdarg.h>

void LCD_PortInit();
void LCD_Set8BitMode();
void LCD_ClearDisplay();
void LCD_ReturnHome();
void LCD_TurnDisplayOn();
void LCD_TurnDisplayOff();
void LCD_DisplayChar(char c);
void LCD_Print(const char* string);
void LCD_printf(const char* format, ...);

#endif