/*******************************************************************************
* File Name: Internal_EEPROM.h
* Version 3.0
*
*  Description:
*   Provides the function definitions for the EEPROM APIs.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EEPROM_Internal_EEPROM_H)
#define CY_EEPROM_Internal_EEPROM_H

#include "cydevice_trm.h"
#include "CyFlash.h"

#if !defined(CY_PSOC5LP)
    #error Component EEPROM_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Internal_EEPROM_Enable(void) ;
void Internal_EEPROM_Start(void) ;
void Internal_EEPROM_Stop (void) ;
cystatus Internal_EEPROM_WriteByte(uint8 dataByte, uint16 address) \
                                            ;
uint8 Internal_EEPROM_ReadByte(uint16 address) ;
uint8 Internal_EEPROM_UpdateTemperature(void) ;
cystatus Internal_EEPROM_EraseSector(uint8 sectorNumber) ;
cystatus Internal_EEPROM_Write(const uint8 * rowData, uint8 rowNumber) ;
cystatus Internal_EEPROM_StartWrite(const uint8 * rowData, uint8 rowNumber) \
                                                ;
cystatus Internal_EEPROM_StartErase(uint8 sectorNumber) ;
cystatus Internal_EEPROM_Query(void) ;
cystatus Internal_EEPROM_ByteWritePos(uint8 dataByte, uint8 rowNumber, uint8 byteNumber) \
                                                ;


/****************************************
*           API Constants
****************************************/

#define Internal_EEPROM_EEPROM_SIZE            CYDEV_EE_SIZE
#define Internal_EEPROM_SPC_BYTE_WRITE_SIZE    (0x01u)

#define Internal_EEPROM_SECTORS_NUMBER         (CYDEV_EE_SIZE / CYDEV_EEPROM_SECTOR_SIZE)

#define Internal_EEPROM_AHB_REQ_SHIFT          (0x00u)
#define Internal_EEPROM_AHB_REQ                ((uint8)(0x01u << Internal_EEPROM_AHB_REQ_SHIFT))
#define Internal_EEPROM_AHB_ACK_SHIFT          (0x01u)
#define Internal_EEPROM_AHB_ACK_MASK           ((uint8)(0x01u << Internal_EEPROM_AHB_ACK_SHIFT))


/***************************************
* Registers
***************************************/
#define Internal_EEPROM_SPC_EE_SCR_REG                 (*(reg8 *) CYREG_SPC_EE_SCR)
#define Internal_EEPROM_SPC_EE_SCR_PTR                 ( (reg8 *) CYREG_SPC_EE_SCR)



/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/
#define Internal_EEPROM_ByteWrite                  Internal_EEPROM_ByteWritePos
#define Internal_EEPROM_QueryWrite                 Internal_EEPROM_Query

#endif /* CY_EEPROM_Internal_EEPROM_H */

/* [] END OF FILE */
