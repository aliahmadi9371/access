/*
 * IO.c
 *
 *  Created on: Jan 20, 2024
 *      Author: dasha
 */
// ###################################################### Includes #########################################################
#include "IO.h"
// #########################################################################################################################

// ###################################################### Functions ########################################################
//LED_RGB 3 pins
void RGB(uint8_t color)
{
    switch (color){
        case 0:{
            //Error MSG
        	//White
        	RGB_RED(1);
        	RGB_GREEN(1);
        	RGB_BLUE(1);
            break;
        }
        case 1:{
            //Cy Up
        	//Red
        	RGB_RED(1);
			RGB_GREEN(0);
			RGB_BLUE(0);
            break;
        }
        case 2:{
            //Linux Up
        	//Green
        	RGB_RED(0);
			RGB_GREEN(1);
			RGB_BLUE(0);
            break;
        }
        case 3:{
            //Server Connected
        	//Blue
        	RGB_RED(0);
			RGB_GREEN(0);
			RGB_BLUE(1);
            break;
        }
        case 4:{
        	//Cyan
        	RGB_RED(0);
			RGB_GREEN(1);
			RGB_BLUE(1);
            break;
        }
        case 5:{
        	//Violet
        	RGB_RED(1);
			RGB_GREEN(0);
			RGB_BLUE(1);
            break;
        }
        case 6:{
        	//Yellow
        	RGB_RED(1);
			RGB_GREEN(1);
			RGB_BLUE(0);
            break;
        }
        default:{
        	//Off
        	RGB_RED(0);
			RGB_GREEN(0);
			RGB_BLUE(0);
            break;
        }
    }

    return;
}

//Relays
extern void Relay_Port1(uint8_t on_off)
{
    if(on_off)
    	HAL_GPIO_WritePin(GPIOC, GPIO_Output_Relay_1_Pin, GPIO_PIN_SET);
    else
    	HAL_GPIO_WritePin(GPIOC, GPIO_Output_Relay_1_Pin, GPIO_PIN_RESET);
    return;
}

extern void Relay_Port2(uint8_t on_off)
{
	if(on_off)
		HAL_GPIO_WritePin(GPIOC, GPIO_Output_Relay_2_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOC, GPIO_Output_Relay_2_Pin, GPIO_PIN_RESET);
	return;
}



uint8_t Pins_Inputs_Read()
{
	uint8_t inps = 0b11000000;
	inps |= (HAL_GPIO_ReadPin(GPIOB, GPIO_Input_7_Pin) << 0)  |
		    (HAL_GPIO_ReadPin(GPIOB, GPIO_Input_8_Pin) << 1)  |
		    (HAL_GPIO_ReadPin(GPIOB, GPIO_Input_9_Pin) << 2)  |
		    (HAL_GPIO_ReadPin(GPIOB, GPIO_Input_10_Pin) << 3) |
		    (HAL_GPIO_ReadPin(GPIOC, GPIO_Input_11_Pin) << 4) |
		    (HAL_GPIO_ReadPin(GPIOA, GPIO_Input_12_Pin) << 5);

	return inps;
}



void RGB_RED(uint8_t on_off)
{
	if(on_off)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
	return;
}

void RGB_GREEN(uint8_t on_off)
{
	if(on_off)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	return;
}

void RGB_BLUE(uint8_t on_off)
{
	if(on_off)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	return;
}
