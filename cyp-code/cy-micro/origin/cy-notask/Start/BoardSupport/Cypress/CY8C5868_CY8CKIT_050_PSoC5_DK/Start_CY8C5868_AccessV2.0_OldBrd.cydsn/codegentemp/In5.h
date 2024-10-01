/*******************************************************************************
* File Name: In5.h  
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

#if !defined(CY_PINS_In5_H) /* Pins In5_H */
#define CY_PINS_In5_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "In5_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 In5__PORT == 15 && ((In5__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    In5_Write(uint8 value);
void    In5_SetDriveMode(uint8 mode);
uint8   In5_ReadDataReg(void);
uint8   In5_Read(void);
void    In5_SetInterruptMode(uint16 position, uint16 mode);
uint8   In5_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the In5_SetDriveMode() function.
     *  @{
     */
        #define In5_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define In5_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define In5_DM_RES_UP          PIN_DM_RES_UP
        #define In5_DM_RES_DWN         PIN_DM_RES_DWN
        #define In5_DM_OD_LO           PIN_DM_OD_LO
        #define In5_DM_OD_HI           PIN_DM_OD_HI
        #define In5_DM_STRONG          PIN_DM_STRONG
        #define In5_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define In5_MASK               In5__MASK
#define In5_SHIFT              In5__SHIFT
#define In5_WIDTH              1u

/* Interrupt constants */
#if defined(In5__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in In5_SetInterruptMode() function.
     *  @{
     */
        #define In5_INTR_NONE      (uint16)(0x0000u)
        #define In5_INTR_RISING    (uint16)(0x0001u)
        #define In5_INTR_FALLING   (uint16)(0x0002u)
        #define In5_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define In5_INTR_MASK      (0x01u) 
#endif /* (In5__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define In5_PS                     (* (reg8 *) In5__PS)
/* Data Register */
#define In5_DR                     (* (reg8 *) In5__DR)
/* Port Number */
#define In5_PRT_NUM                (* (reg8 *) In5__PRT) 
/* Connect to Analog Globals */                                                  
#define In5_AG                     (* (reg8 *) In5__AG)                       
/* Analog MUX bux enable */
#define In5_AMUX                   (* (reg8 *) In5__AMUX) 
/* Bidirectional Enable */                                                        
#define In5_BIE                    (* (reg8 *) In5__BIE)
/* Bit-mask for Aliased Register Access */
#define In5_BIT_MASK               (* (reg8 *) In5__BIT_MASK)
/* Bypass Enable */
#define In5_BYP                    (* (reg8 *) In5__BYP)
/* Port wide control signals */                                                   
#define In5_CTL                    (* (reg8 *) In5__CTL)
/* Drive Modes */
#define In5_DM0                    (* (reg8 *) In5__DM0) 
#define In5_DM1                    (* (reg8 *) In5__DM1)
#define In5_DM2                    (* (reg8 *) In5__DM2) 
/* Input Buffer Disable Override */
#define In5_INP_DIS                (* (reg8 *) In5__INP_DIS)
/* LCD Common or Segment Drive */
#define In5_LCD_COM_SEG            (* (reg8 *) In5__LCD_COM_SEG)
/* Enable Segment LCD */
#define In5_LCD_EN                 (* (reg8 *) In5__LCD_EN)
/* Slew Rate Control */
#define In5_SLW                    (* (reg8 *) In5__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define In5_PRTDSI__CAPS_SEL       (* (reg8 *) In5__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define In5_PRTDSI__DBL_SYNC_IN    (* (reg8 *) In5__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define In5_PRTDSI__OE_SEL0        (* (reg8 *) In5__PRTDSI__OE_SEL0) 
#define In5_PRTDSI__OE_SEL1        (* (reg8 *) In5__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define In5_PRTDSI__OUT_SEL0       (* (reg8 *) In5__PRTDSI__OUT_SEL0) 
#define In5_PRTDSI__OUT_SEL1       (* (reg8 *) In5__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define In5_PRTDSI__SYNC_OUT       (* (reg8 *) In5__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(In5__SIO_CFG)
    #define In5_SIO_HYST_EN        (* (reg8 *) In5__SIO_HYST_EN)
    #define In5_SIO_REG_HIFREQ     (* (reg8 *) In5__SIO_REG_HIFREQ)
    #define In5_SIO_CFG            (* (reg8 *) In5__SIO_CFG)
    #define In5_SIO_DIFF           (* (reg8 *) In5__SIO_DIFF)
#endif /* (In5__SIO_CFG) */

/* Interrupt Registers */
#if defined(In5__INTSTAT)
    #define In5_INTSTAT            (* (reg8 *) In5__INTSTAT)
    #define In5_SNAP               (* (reg8 *) In5__SNAP)
    
	#define In5_0_INTTYPE_REG 		(* (reg8 *) In5__0__INTTYPE)
#endif /* (In5__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_In5_H */


/* [] END OF FILE */
