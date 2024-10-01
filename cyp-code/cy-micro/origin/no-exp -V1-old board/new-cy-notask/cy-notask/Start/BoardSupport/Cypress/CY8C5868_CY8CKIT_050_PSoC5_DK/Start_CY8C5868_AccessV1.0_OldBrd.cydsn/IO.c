/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

// ###################################################### Includes #########################################################
#include "IO.h"
// #########################################################################################################################

// ###################################################### Functions ########################################################
//LED_RGB 3 pins
void RGB(uint8 color)
{
    switch (color){
        case 0:{
            //Error MSG
            LED_RGB_Write(WHI);
            break;
        }
        case 1:{
            //Cy Up
            LED_RGB_Write(RED);
            break;
        }
        case 2:{
            //Linux Up
            LED_RGB_Write(GRE);
            break;
        }
        case 3:{
            //Server Connected
            LED_RGB_Write(BLU);
            break;
        }
        case 4:{
            LED_RGB_Write(CYA);
            break;
        }
        case 5:{
            LED_RGB_Write(PUR);
            break;
        }
        case 6:{
            LED_RGB_Write(YEL);
            break;
        }
        default:{
            LED_RGB_Write(BLC);
            break;
        }
    }
    
    return;
}


//Relays
extern void Relay_Port1(uint8 on_off) 
{
    if(on_off)
        Relay_Port1_Write(1);
    else
        Relay_Port1_Write(0);
    return;
}

extern void Relay_Port2(uint8 on_off)
{
    if(on_off)
        Relay_Port2_Write(1);
    else
        Relay_Port2_Write(0);
    
    return;
}
// #########################################################################################################################