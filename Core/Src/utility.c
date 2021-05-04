#include "utility.h"

void send_to_UART(char* msg)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void UT_printf(const char* format, ...)
{
	va_list args;
	char buffer[32];	
	memset(buffer, 0, 32);

	va_start(args, format);	
	vsprintf(buffer, format, args);
	send_to_UART(buffer);
	va_end(args);
}

void UT_Error_Handler(char* err_msg)
{
    __disable_irq();
    while (1)
	{
    	send_to_UART(err_msg);
	}
}

void UT_Delay_MicroSeconds(uint32_t uSec)
{
	uint32_t uSecWait = uSec * ((SystemCoreClock / 1000000) / 3);
	while(uSecWait--);
}