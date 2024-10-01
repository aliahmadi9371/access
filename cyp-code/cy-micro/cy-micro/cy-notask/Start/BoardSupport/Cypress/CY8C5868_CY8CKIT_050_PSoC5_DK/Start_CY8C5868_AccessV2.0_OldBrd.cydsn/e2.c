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
#include "e2.h"

// ###################################################### EEProm Functional Functions ######################################
uint8 ie2_read_byte(uint16 addr)
{
    uint8 ret = Internal_EEPROM_ReadByte(addr); 

    return ret;
}

uint8 ie2_write_byte(uint8 byte, uint16 addr)
{
    uint8 ret = 0;
    cystatus st = Internal_EEPROM_WriteByte(byte, addr);
    
    if(st == CYRET_SUCCESS){
        ret = 1;
    }
    
    return ret;
}

/* [] END OF FILE */
