/*******************************************************************************
* File Name: Bootloader_StatusLED.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Bootloader_StatusLED.h"

static Bootloader_StatusLED_BACKUP_STRUCT  Bootloader_StatusLED_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Bootloader_StatusLED_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet Bootloader_StatusLED_SUT.c usage_Bootloader_StatusLED_Sleep_Wakeup
*******************************************************************************/
void Bootloader_StatusLED_Sleep(void)
{
    #if defined(Bootloader_StatusLED__PC)
        Bootloader_StatusLED_backup.pcState = Bootloader_StatusLED_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Bootloader_StatusLED_backup.usbState = Bootloader_StatusLED_CR1_REG;
            Bootloader_StatusLED_USB_POWER_REG |= Bootloader_StatusLED_USBIO_ENTER_SLEEP;
            Bootloader_StatusLED_CR1_REG &= Bootloader_StatusLED_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Bootloader_StatusLED__SIO)
        Bootloader_StatusLED_backup.sioState = Bootloader_StatusLED_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Bootloader_StatusLED_SIO_REG &= (uint32)(~Bootloader_StatusLED_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Bootloader_StatusLED_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to Bootloader_StatusLED_Sleep() for an example usage.
*******************************************************************************/
void Bootloader_StatusLED_Wakeup(void)
{
    #if defined(Bootloader_StatusLED__PC)
        Bootloader_StatusLED_PC = Bootloader_StatusLED_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Bootloader_StatusLED_USB_POWER_REG &= Bootloader_StatusLED_USBIO_EXIT_SLEEP_PH1;
            Bootloader_StatusLED_CR1_REG = Bootloader_StatusLED_backup.usbState;
            Bootloader_StatusLED_USB_POWER_REG &= Bootloader_StatusLED_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Bootloader_StatusLED__SIO)
        Bootloader_StatusLED_SIO_REG = Bootloader_StatusLED_backup.sioState;
    #endif
}


/* [] END OF FILE */
