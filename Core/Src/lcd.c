#include "lcd.h"

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

#define RS 	GPIOA, GPIO_PIN_0
#define RW 	GPIOA, GPIO_PIN_1
#define E 	GPIOB, GPIO_PIN_8
#define DB7 GPIOB, GPIO_PIN_7
#define DB6 GPIOB, GPIO_PIN_6
#define DB5 GPIOB, GPIO_PIN_5
#define DB4 GPIOB, GPIO_PIN_4
#define DB3 GPIOB, GPIO_PIN_3
#define DB2 GPIOB, GPIO_PIN_2
#define DB1 GPIOB, GPIO_PIN_1
#define DB0 GPIOB, GPIO_PIN_0

// ----- Variables -----

static char byteBuffer[8];

// ----- Private functions ----- 

static void set_RS(int state)
{
    HAL_GPIO_WritePin(RS, state);
}
static void set_RW(int state)
{
    HAL_GPIO_WritePin(RW, state);
}
static void set_E(int state)
{
    HAL_GPIO_WritePin(E, state);
}
static void set_DB7(int state)
{
    HAL_GPIO_WritePin(DB7, state);
}
static void set_DB6(int state)
{
    HAL_GPIO_WritePin(DB6, state);
}
static void set_DB5(int state)
{
    HAL_GPIO_WritePin(DB5, state);
}
static void set_DB4(int state)
{
    HAL_GPIO_WritePin(DB4, state);
}
static void set_DB3(int state)
{
    HAL_GPIO_WritePin(DB3, state);
}
static void set_DB2(int state)
{
    HAL_GPIO_WritePin(DB2, state);
}
static void set_DB1(int state)
{
    HAL_GPIO_WritePin(DB1, state);
}
static void set_DB0(int state)
{
    HAL_GPIO_WritePin(DB0, state);
}

static int get_DB7()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(DB7);
    if(state == GPIO_PIN_SET)
        return 1;
    else
        return 0;
}

static void sendEnable()
{
    set_E(1);

    //Wait atleast 2 ticks
    uint32_t tick_start = HAL_GetTick();
    while((HAL_GetTick() - tick_start) <= 2){}

    set_E(0);
}
static void waitForBusyFlag()
{
    set_RS(0);
    set_RW(1);
    sendEnable();    

    int val = 1;
    while(val != 0)
    {
        val = get_DB7(); //Get busy flag
    }
}
static void sendCommand()
{
    sendEnable();
    waitForBusyFlag();
}
static void charToByteBuffer(char c)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        char val = (c >> i) & 0x01;
        byteBuffer[i] = val;
    }
}
static void sendByteBuffer()
{    
    //Write data
    set_RS(1);
    set_RW(0);

    //Address to write to correct pin
    uint16_t address = 0x0001;

    for(uint8_t i = 0; i < 8; i++)
    {        
        char val = byteBuffer[i];
        HAL_GPIO_WritePin(GPIOB, address, (int)val); 
        address = address << 1; //Go to next pin
    }
    sendCommand();
}

// ----- Public Functions ----- 

void LCD_Init()
{
    //Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Initialize all Output-Pins of Port A
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Initialize all Output-Pins of Port B
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //Wait after startup
    HAL_Delay(16);

    //Reset everything
    set_RS(0);
    set_RW(0);
    set_DB7(0);
    set_DB6(0);
    set_DB5(0);
    set_DB4(0);
    set_DB3(0);
    set_DB2(0);
    set_DB1(0);
    set_DB0(0);

    sendCommand();
}

void LCD_Set8BitMode()
{
    set_RS(0);
    set_RW(0);

    set_DB7(0);
    set_DB6(0);
    set_DB5(1); //Configure Function Set
    set_DB4(1); //8-Bit or 4-Bit
    set_DB3(1); //2 Lines or 1 Line
    set_DB2(0); //5x11 or 5x8 Pixel
    set_DB1(0);
    set_DB0(0);

    sendCommand();
}

void LCD_ClearDisplay()
{
    set_RS(0);
    set_RW(0);

    set_DB7(0);
    set_DB6(0);
    set_DB5(0);
    set_DB4(0);
    set_DB3(0);
    set_DB2(0);
    set_DB1(0);
    set_DB0(1);

    sendCommand();
}

void LCD_ReturnHome()
{
    set_RS(0);
    set_RW(0);

    set_DB7(0);
    set_DB6(0);
    set_DB5(0);
    set_DB4(0);
    set_DB3(0);
    set_DB2(0);
    set_DB1(1);
    set_DB0(0);

    sendCommand();
}

void LCD_TurnDisplayOn()
{
    set_RS(0);
    set_RW(0);

    set_DB7(0);
    set_DB6(0);
    set_DB5(0);
    set_DB4(0);
    set_DB3(1); //Configure Display
    set_DB2(1); //Turn On/Off
    set_DB1(0); //Cursor settings
    set_DB0(0); //Cursor settings

    sendCommand();
}

void LCD_TurnDisplayOff()
{
    set_RS(0);
    set_RW(0);

    set_DB7(0);
    set_DB6(0);
    set_DB5(0);
    set_DB4(0);
    set_DB3(1); //Configure Display
    set_DB2(0); //Turn On/Off
    set_DB1(0); //Cursor settings
    set_DB0(0); //Cursor settings

    sendCommand();
}

void LCD_DisplayChar(char c)
{
    charToByteBuffer(c);
    sendByteBuffer();
}

void LCD_Print(const char* string)
{
    //Iterate over every character     
    for(int i = 0; i < strlen(string); i++)
    {
        charToByteBuffer(string[i]); //Get binary representation of the character
        sendByteBuffer(); //Set pins accordingly
    }    
}

void LCD_printf(const char* format, ...)
{
	va_list args;
	char buffer[256];	
	memset(buffer, 0, 256);

	va_start(args, format);	
	vsprintf(buffer, format, args);
	LCD_Print(buffer);
	va_end(args);
}