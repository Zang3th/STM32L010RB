#include "sevenSegment.h"

// ----- Defines -----

#define SEGMENT_A GPIOC, GPIO_PIN_0
#define SEGMENT_B GPIOC, GPIO_PIN_1
#define SEGMENT_C GPIOC, GPIO_PIN_2
#define SEGMENT_D GPIOC, GPIO_PIN_3
#define SEGMENT_E GPIOC, GPIO_PIN_4
#define SEGMENT_F GPIOC, GPIO_PIN_5
#define SEGMENT_G GPIOC, GPIO_PIN_6
#define SEGMENT_DP GPIOC, GPIO_PIN_7 

// ----- Private functions ----- 

void Segment_Reset()
{
    HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEGMENT_DP, GPIO_PIN_SET);
}

// ----- Public Functions -----

void Segment_Init()
{
    //Create init struct
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
             GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Segment_Display(char character)
{
    Segment_Reset();

    switch(character)
	{
		case('1'):
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
			break;

        case('2'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('3'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('4'):
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('5'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('6'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('7'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
			break;

        case('8'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('9'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_G,  GPIO_PIN_RESET);
			break;

        case('0'):
            HAL_GPIO_WritePin(SEGMENT_A,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_B,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_C,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_D,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_E,  GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SEGMENT_F,  GPIO_PIN_RESET);
			break;

		default:
			break;
	}
}