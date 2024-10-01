/*******************************************************************************
* File Name: Bootloader_StatusLED.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Bootloader_StatusLED_H) /* Pins Bootloader_StatusLED_H */
#define CY_PINS_Bootloader_StatusLED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Bootloader_StatusLED_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} Bootloader_StatusLED_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Bootloader_StatusLED_Read(void);
void    Bootloader_StatusLED_Write(uint8 value);
uint8   Bootloader_StatusLED_ReadDataReg(void);
#if defined(Bootloader_StatusLED__PC) || (CY_PSOC4_4200L) 
    void    Bootloader_StatusLED_SetDriveMode(uint8 mode);
#endif
void    Bootloader_StatusLED_SetInterruptMode(uint16 position, uint16 mode);
uint8   Bootloader_StatusLED_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Bootloader_StatusLED_Sleep(void); 
void Bootloader_StatusLED_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Bootloader_StatusLED__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Bootloader_StatusLED_DRIVE_MODE_BITS        (3)
    #define Bootloader_StatusLED_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Bootloader_StatusLED_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Bootloader_StatusLED_SetDriveMode() function.
         *  @{
         */
        #define Bootloader_StatusLED_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Bootloader_StatusLED_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Bootloader_StatusLED_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Bootloader_StatusLED_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Bootloader_StatusLED_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Bootloader_StatusLED_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Bootloader_StatusLED_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Bootloader_StatusLED_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Bootloader_StatusLED_MASK               Bootloader_StatusLED__MASK
#define Bootloader_StatusLED_SHIFT              Bootloader_StatusLED__SHIFT
#define Bootloader_StatusLED_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Bootloader_StatusLED_SetInterruptMode() function.
     *  @{
     */
        #define Bootloader_StatusLED_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Bootloader_StatusLED_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Bootloader_StatusLED_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Bootloader_StatusLED_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Bootloader_StatusLED__SIO)
    #define Bootloader_StatusLED_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Bootloader_StatusLED__PC) && (CY_PSOC4_4200L)
    #define Bootloader_StatusLED_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Bootloader_StatusLED_USBIO_DISABLE              ((uint32)(~Bootloader_StatusLED_USBIO_ENABLE))
    #define Bootloader_StatusLED_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Bootloader_StatusLED_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Bootloader_StatusLED_USBIO_ENTER_SLEEP          ((uint32)((1u << Bootloader_StatusLED_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Bootloader_StatusLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define Bootloader_StatusLED_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Bootloader_StatusLED_USBIO_SUSPEND_SHIFT)))
    #define Bootloader_StatusLED_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Bootloader_StatusLED_USBIO_SUSPEND_DEL_SHIFT)))
    #define Bootloader_StatusLED_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Bootloader_StatusLED__PC)
    /* Port Configuration */
    #define Bootloader_StatusLED_PC                 (* (reg32 *) Bootloader_StatusLED__PC)
#endif
/* Pin State */
#define Bootloader_StatusLED_PS                     (* (reg32 *) Bootloader_StatusLED__PS)
/* Data Register */
#define Bootloader_StatusLED_DR                     (* (reg32 *) Bootloader_StatusLED__DR)
/* Input Buffer Disable Override */
#define Bootloader_StatusLED_INP_DIS                (* (reg32 *) Bootloader_StatusLED__PC2)

/* Interrupt configuration Registers */
#define Bootloader_StatusLED_INTCFG                 (* (reg32 *) Bootloader_StatusLED__INTCFG)
#define Bootloader_StatusLED_INTSTAT                (* (reg32 *) Bootloader_StatusLED__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Bootloader_StatusLED_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Bootloader_StatusLED__SIO)
    #define Bootloader_StatusLED_SIO_REG            (* (reg32 *) Bootloader_StatusLED__SIO)
#endif /* (Bootloader_StatusLED__SIO_CFG) */

/* USBIO registers */
#if !defined(Bootloader_StatusLED__PC) && (CY_PSOC4_4200L)
    #define Bootloader_StatusLED_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Bootloader_StatusLED_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Bootloader_StatusLED_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Bootloader_StatusLED_DRIVE_MODE_SHIFT       (0x00u)
#define Bootloader_StatusLED_DRIVE_MODE_MASK        (0x07u << Bootloader_StatusLED_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Bootloader_StatusLED_H */


/* [] END OF FILE */
