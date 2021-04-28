#include "utility.h"

void Print(UART_HandleTypeDef* huart, char* msg)
{
	HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 100);
}

void Error_Handler(UART_HandleTypeDef* huart, char* err_msg)
{
     __disable_irq();
    while (1)
	{
    	Print(huart, err_msg);
	}
}