/*******************************************************************************
* File Name: Pins_Port1.h  
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

#if !defined(CY_PINS_Pins_Port1_ALIASES_H) /* Pins Pins_Port1_ALIASES_H */
#define CY_PINS_Pins_Port1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Pins_Port1_0			(Pins_Port1__0__PC)
#define Pins_Port1_0_INTR	((uint16)((uint16)0x0001u << Pins_Port1__0__SHIFT))

#define Pins_Port1_1			(Pins_Port1__1__PC)
#define Pins_Port1_1_INTR	((uint16)((uint16)0x0001u << Pins_Port1__1__SHIFT))

#define Pins_Port1_INTR_ALL	 ((uint16)(Pins_Port1_0_INTR| Pins_Port1_1_INTR))

#endif /* End Pins Pins_Port1_ALIASES_H */


/* [] END OF FILE */
