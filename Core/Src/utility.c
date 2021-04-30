#include "utility.h"

static char buffer[20];

void Print_msg(char* msg)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

void Print_int8_t(char* name, uint8_t value)
{
	sprintf(buffer, "%s = %d\r\n", name, value);
	Print_msg(buffer);
}

void Print_int16_t(char* name, uint16_t value)
{
	sprintf(buffer, "%s = %u\r\n", name, value);
	Print_msg(buffer);
}

void Print_int32_t(char* name, uint32_t value)
{
	sprintf(buffer, "%s = %lu\r\n", name, value);
	Print_msg(buffer);
}

void Error_Handler(char* err_msg)
{
     __disable_irq();
    while (1)
	{
    	Print_msg(err_msg);
	}
}