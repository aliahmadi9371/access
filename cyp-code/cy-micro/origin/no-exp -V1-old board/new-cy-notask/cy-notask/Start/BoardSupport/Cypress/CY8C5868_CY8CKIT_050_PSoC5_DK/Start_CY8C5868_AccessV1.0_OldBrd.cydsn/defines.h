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
#ifndef DEFINES_H
#define DEFINES_H

// ###################################################### Defines ##########################################################
//RGB 3 pins define for LED_RGB port
#define RED 0b00000110
#define GRE 0b00000101
#define BLU 0b00000011
#define WHI 0b00000000
#define CYA 0b00000001
#define PUR 0b00000010
#define YEL 0b00000100
#define BLC 0b00000000
// #########################################################################################################################
    
// ###################################################### Includes #########################################################   
#include "RTOS.h"
#include "BSP.h"
#include "OS_Config.h"
#include "device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #########################################################################################################################
    
// ###################################################### Prototypes #######################################################
extern char* strremove(char *str, const char *sub);
// #########################################################################################################################
    
#endif  // DEFINES_H
/* [] END OF FILE */
