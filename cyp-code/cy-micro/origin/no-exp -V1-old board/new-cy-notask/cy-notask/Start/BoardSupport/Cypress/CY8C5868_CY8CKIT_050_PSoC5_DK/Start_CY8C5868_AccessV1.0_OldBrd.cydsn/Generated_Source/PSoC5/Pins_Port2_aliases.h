/*******************************************************************************
* File Name: Pins_Port2.h  
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

#if !defined(CY_PINS_Pins_Port2_ALIASES_H) /* Pins Pins_Port2_ALIASES_H */
#define CY_PINS_Pins_Port2_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Pins_Port2_0			(Pins_Port2__0__PC)
#define Pins_Port2_0_INTR	((uint16)((uint16)0x0001u << Pins_Port2__0__SHIFT))

#define Pins_Port2_1			(Pins_Port2__1__PC)
#define Pins_Port2_1_INTR	((uint16)((uint16)0x0001u << Pins_Port2__1__SHIFT))

#define Pins_Port2_2			(Pins_Port2__2__PC)
#define Pins_Port2_2_INTR	((uint16)((uint16)0x0001u << Pins_Port2__2__SHIFT))

#define Pins_Port2_INTR_ALL	 ((uint16)(Pins_Port2_0_INTR| Pins_Port2_1_INTR| Pins_Port2_2_INTR))

#endif /* End Pins Pins_Port2_ALIASES_H */


/* [] END OF FILE */
