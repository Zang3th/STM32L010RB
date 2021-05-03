#include "dht.h"

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

		Temperature Sensor (AM2302):
			PA7:		VCC
			PA6:		Data-bus
*/

// ----- Defines -----

// ----- Variables -----

// ----- Private functions ----- 

// ----- Public Functions -----