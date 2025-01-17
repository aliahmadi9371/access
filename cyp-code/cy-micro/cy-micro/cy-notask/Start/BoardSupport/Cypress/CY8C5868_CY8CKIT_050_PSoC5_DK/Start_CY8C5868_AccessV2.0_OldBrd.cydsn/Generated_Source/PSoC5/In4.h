/*******************************************************************************
* File Name: In4.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_In4_H) /* Pins In4_H */
#define CY_PINS_In4_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "In4_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 In4__PORT == 15 && ((In4__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    In4_Write(uint8 value);
void    In4_SetDriveMode(uint8 mode);
uint8   In4_ReadDataReg(void);
uint8   In4_Read(void);
void    In4_SetInterruptMode(uint16 position, uint16 mode);
uint8   In4_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the In4_SetDriveMode() function.
     *  @{
     */
        #define In4_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define In4_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define In4_DM_RES_UP          PIN_DM_RES_UP
        #define In4_DM_RES_DWN         PIN_DM_RES_DWN
        #define In4_DM_OD_LO           PIN_DM_OD_LO
        #define In4_DM_OD_HI           PIN_DM_OD_HI
        #define In4_DM_STRONG          PIN_DM_STRONG
        #define In4_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define In4_MASK               In4__MASK
#define In4_SHIFT              In4__SHIFT
#define In4_WIDTH              1u

/* Interrupt constants */
#if defined(In4__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in In4_SetInterruptMode() function.
     *  @{
     */
        #define In4_INTR_NONE      (uint16)(0x0000u)
        #define In4_INTR_RISING    (uint16)(0x0001u)
        #define In4_INTR_FALLING   (uint16)(0x0002u)
        #define In4_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define In4_INTR_MASK      (0x01u) 
#endif /* (In4__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define In4_PS                     (* (reg8 *) In4__PS)
/* Data Register */
#define In4_DR                     (* (reg8 *) In4__DR)
/* Port Number */
#define In4_PRT_NUM                (* (reg8 *) In4__PRT) 
/* Connect to Analog Globals */                                                  
#define In4_AG                     (* (reg8 *) In4__AG)                       
/* Analog MUX bux enable */
#define In4_AMUX                   (* (reg8 *) In4__AMUX) 
/* Bidirectional Enable */                                                        
#define In4_BIE                    (* (reg8 *) In4__BIE)
/* Bit-mask for Aliased Register Access */
#define In4_BIT_MASK               (* (reg8 *) In4__BIT_MASK)
/* Bypass Enable */
#define In4_BYP                    (* (reg8 *) In4__BYP)
/* Port wide control signals */                                                   
#define In4_CTL                    (* (reg8 *) In4__CTL)
/* Drive Modes */
#define In4_DM0                    (* (reg8 *) In4__DM0) 
#define In4_DM1                    (* (reg8 *) In4__DM1)
#define In4_DM2                    (* (reg8 *) In4__DM2) 
/* Input Buffer Disable Override */
#define In4_INP_DIS                (* (reg8 *) In4__INP_DIS)
/* LCD Common or Segment Drive */
#define In4_LCD_COM_SEG            (* (reg8 *) In4__LCD_COM_SEG)
/* Enable Segment LCD */
#define In4_LCD_EN                 (* (reg8 *) In4__LCD_EN)
/* Slew Rate Control */
#define In4_SLW                    (* (reg8 *) In4__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define In4_PRTDSI__CAPS_SEL       (* (reg8 *) In4__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define In4_PRTDSI__DBL_SYNC_IN    (* (reg8 *) In4__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define In4_PRTDSI__OE_SEL0        (* (reg8 *) In4__PRTDSI__OE_SEL0) 
#define In4_PRTDSI__OE_SEL1        (* (reg8 *) In4__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define In4_PRTDSI__OUT_SEL0       (* (reg8 *) In4__PRTDSI__OUT_SEL0) 
#define In4_PRTDSI__OUT_SEL1       (* (reg8 *) In4__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define In4_PRTDSI__SYNC_OUT       (* (reg8 *) In4__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(In4__SIO_CFG)
    #define In4_SIO_HYST_EN        (* (reg8 *) In4__SIO_HYST_EN)
    #define In4_SIO_REG_HIFREQ     (* (reg8 *) In4__SIO_REG_HIFREQ)
    #define In4_SIO_CFG            (* (reg8 *) In4__SIO_CFG)
    #define In4_SIO_DIFF           (* (reg8 *) In4__SIO_DIFF)
#endif /* (In4__SIO_CFG) */

/* Interrupt Registers */
#if defined(In4__INTSTAT)
    #define In4_INTSTAT            (* (reg8 *) In4__INTSTAT)
    #define In4_SNAP               (* (reg8 *) In4__SNAP)
    
	#define In4_0_INTTYPE_REG 		(* (reg8 *) In4__0__INTTYPE)
#endif /* (In4__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_In4_H */


/* [] END OF FILE */
