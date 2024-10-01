/*******************************************************************************
* File Name: LED_RGB.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LED_RGB_ALIASES_H) /* Pins LED_RGB_ALIASES_H */
#define CY_PINS_LED_RGB_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define LED_RGB_0			(LED_RGB__0__PC)
#define LED_RGB_0_INTR	((uint16)((uint16)0x0001u << LED_RGB__0__SHIFT))

#define LED_RGB_1			(LED_RGB__1__PC)
#define LED_RGB_1_INTR	((uint16)((uint16)0x0001u << LED_RGB__1__SHIFT))

#define LED_RGB_2			(LED_RGB__2__PC)
#define LED_RGB_2_INTR	((uint16)((uint16)0x0001u << LED_RGB__2__SHIFT))

#define LED_RGB_INTR_ALL	 ((uint16)(LED_RGB_0_INTR| LED_RGB_1_INTR| LED_RGB_2_INTR))

#endif /* End Pins LED_RGB_ALIASES_H */


/* [] END OF FILE */
