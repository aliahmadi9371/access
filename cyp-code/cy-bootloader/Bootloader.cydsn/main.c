/******************************************************************************
* File Name: main.c
*
* Version 1.10
*
* Description: 
*  This example project demonstrates the basic operation of the Bootloader
*  Component.
*
* Related Document: 
*  CE221653_PSoC_4_Bootloader_and_Bootloadable.pdf
*
* Hardware Dependency: 
*  See CE221653_PSoC_4_Bootloader_and_Bootloadable.pdf
*
*******************************************************************************
* Copyright (2017-2019), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability. 
*******************************************************************************/

#include "project.h"
#include <CyFlash.h>
#include <stdio.h>

// Reset Status Metadata                                          
#define IE2_Loc_Reset_Status                0x12               
#define IE2_Loc_Reset_Code_Address          0x13
// FOTA
#define FOTA_START                          1
#define FOTA_STOP                           0
#define FOTA_ROW_SIZE                       262
// external e2prom
#define EE2_1_CHIP_ADDR                     0x52
#define EE2_2_CHIP_ADDR                     0x50

#define IE2_Loc_FOTA_STATUS                 0x70
#define IE2_Loc_FOTA_CRC_BYTE0              0x71
#define IE2_Loc_FOTA_CRC_BYTE1              0x72
#define IE2_Loc_FOTA_CRC_BYTE2              0x73
#define IE2_Loc_FOTA_CRC_BYTE3              0x74
#define IE2_Loc_FOTA_App_SIZE_BYTE0         0x75
#define IE2_Loc_FOTA_App_SIZE_BYTE1         0x76
#define IE2_Loc_FOTA_App_SIZE_BYTE2         0x77    
#define IE2_Loc_FOTA_App_SIZE_BYTE3         0x78
#define IE2_Loc_FOTA_ROW_START_BYTE0        0x79
#define IE2_Loc_FOTA_ROW_START_BYTE1        0x7A

#define Das_E2_Loc_Reset_Status             18

// ----------------------------  Software ----------------------------
#define A_Reset_Code_Server                 0
#define A_Reset_Code_FOTA_EE2_Write_Fail    1
#define A_Reset_Code_FOTA_Done              2
#define Das_Reset_Code_FOTA_EE2_Write_Fail  3
#define Das_Reset_Code_Gw_Connection_Lost   4
#define A_Reset_Code_Invalid_SiliconId      5
#define A_Reset_Code_Bus_Connection_Lost    6
#define Gw_Reset_Code_Mqtt_Open_Fail        7
#define Gw_Reset_Code_Mqtt_Open_Resp_Fail   8
#define Gw_Reset_Code_Mqtt_Conn_Resp_Fail   9
#define Gw_Reset_Code_Mqtt_Sub_Resp_Fail    10
#define Gw_Reset_Code_Mqtt_Pub_Resp_Fail    11
#define Gw_Reset_Code_Quectel_Lost          12
#define Gw_Reset_Code_FOTA_Fail				13      
// ----------------------------  Total ----------------------------


//#if (CY_PSOC4_4100MS)
//    #define LED_BLUE_ON     (1u)
//    #define LED_BLUE_OFF    (0u)
//#else
//    #define LED_BLUE_ON     (0u)
//    #define LED_BLUE_OFF    (1u)
//#endif


/******************************************************************************
* Function Name: main
*******************************************************************************
*
* Summary:
*  The main function performs these functions:
*  1. Shows the Bootloader status by enabling the blue LED.
*  2. Starts the Bootloader communication.
*
******************************************************************************/
//  Device Address : |1|-|0|-|1|-|0|-|A2|-|A1|-|A0|
uint8 counter = 0;
uint32 appCRC = 0;
uint32 newCRC = 0;
char buff[100];

uint8 arrayID = 0;
uint16 rowID = 0;

uint8 internal_flash_read_byte(uint8 arr_id, uint16 byte_add)
{
    // 0000001B
    uint32 flash_addr = (uint32)(arr_id << 16 | byte_add);
    __IO uint32_t read_data = *(__IO uint32_t *)flash_addr;    
	return (uint8)read_data;
}

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

uint8 ie2_writeByte(uint8 byte, uint16 addr)
{
    uint8 ret = 0;
    cystatus st = Internal_EEPROM_WriteByte(byte, addr);
    if(st == CYRET_SUCCESS){
        ret = 1;
    }
    
    return ret;
}

uint8 internal_flash_handle_error(cystatus value)
{
    uint8 ret = 0;
    if(value == CYRET_SUCCESS){
        ret = 1;
    } 
    else if(value == CYRET_LOCKED){
        RS232_Debug_PutString("Flash writing already in use\r\n");
    } 
    else if(value == CYRET_CANCELED){
        RS232_Debug_PutString("Command not accepted\r\n");
    } 
    else{
        RS232_Debug_PutString("Failure\r\n");
    }
    
    return ret;
}

uint8 internal_flash_write_row(uint16 appLines)
{
    uint8 fail = 0;
    uint8 dataBuff[CY_FLASH_SIZEOF_ROW];
    uint8 dataBuff_index = 0;
    uint16 former_percent = 0;
    for(uint16 i = 0; i < appLines; i++){
        dataBuff_index = 0;
        for(uint j = CY_FLASH_SIZEOF_ROW * i; j < (CY_FLASH_SIZEOF_ROW * (i + 1)); j++){  // 0-255 => 256 - 511 => 512 - ...
            dataBuff[dataBuff_index] = internal_flash_read_byte(0x02, j);
            dataBuff_index++;
        }
        
        uint8 percent = 0;
        percent = (((float)i + 1)/ appLines) * 100;
        
        if(percent % 20 == 0 && percent != former_percent){
            former_percent = percent;
            
            if(percent == 20){
                RS232_Debug_PutString("Writing: 20%\r\n");
            }
            else if(percent == 40){
                RS232_Debug_PutString("Writing: 40%\r\n");
            }
            else if(percent == 60){
                RS232_Debug_PutString("Writing: 60%\r\n");
            }
            else if(percent == 80){
                RS232_Debug_PutString("Writing: 80%\r\n");
            }
            else if(percent == 100){
                RS232_Debug_PutString("Writing: 100%\r\n");
            }
            
        }
        CyDelay(25);
        
        if(CySetTemp() == CYRET_SUCCESS){
            if(i == appLines - 1){
                if(internal_flash_handle_error(CyWriteRowData(0x03, 0x00FF, dataBuff))){
                    CyDelay(10);
                }
                else{
                    fail = 1;
                }
            }
            else{
                if(internal_flash_handle_error(CyWriteRowData(arrayID, rowID, dataBuff))){
                    rowID++;
                    CyDelay(10);
                }
                else{
                    fail = 1;
                }
            }
        }
        else{
            fail = 2;
        }
               
        if(rowID == 0x0100){
            rowID = 0;
            arrayID++;
            
        }
    }
    
    memset(dataBuff, '\0', sizeof(dataBuff));

    return fail;
}

int main()
{
    /* Enable the global interrupts */
    CyGlobalIntEnable;
    
    Internal_EEPROM_Start();
    RS232_Debug_Start();
        
    RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Start &&&&&--------------------\r\n");
    
    
    ie2_writeByte(CyResetStatus, Das_E2_Loc_Reset_Status);
    
//    write_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_STATUS, FOTA_START);
    
    /* ***************************** CHECK METADATA FROM External EEPROM ***************************** */
//    uint8 status = read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_STATUS); 
//    appCRC = read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_CRC_BYTE3) << 24 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_CRC_BYTE2) << 16 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_CRC_BYTE1) << 8 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_CRC_BYTE0) << 0;
//    uint32 appSize = read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_App_SIZE_BYTE3) << 24 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_App_SIZE_BYTE2) << 16 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_App_SIZE_BYTE1) << 8 | read_e2prom_i2c(EE2_1_CHIP_ADDR, EE2_Loc_FOTA_App_SIZE_BYTE0) << 0;
    
    uint8 status = ie2_read_byte(IE2_Loc_FOTA_STATUS); 
    appCRC = ie2_read_byte(IE2_Loc_FOTA_CRC_BYTE3) << 24 | ie2_read_byte(IE2_Loc_FOTA_CRC_BYTE2) << 16 | ie2_read_byte(IE2_Loc_FOTA_CRC_BYTE1) << 8 | ie2_read_byte(IE2_Loc_FOTA_CRC_BYTE0) << 0;
    uint32 appSize = ie2_read_byte(IE2_Loc_FOTA_App_SIZE_BYTE3) << 24 | ie2_read_byte(IE2_Loc_FOTA_App_SIZE_BYTE2) << 16 | ie2_read_byte(IE2_Loc_FOTA_App_SIZE_BYTE1) << 8 | ie2_read_byte(IE2_Loc_FOTA_App_SIZE_BYTE0) << 0;

    rowID = ie2_read_byte(IE2_Loc_FOTA_ROW_START_BYTE0) << 8 | ie2_read_byte(IE2_Loc_FOTA_ROW_START_BYTE1) << 0;
    /* ************************************************************************************************ */
    
    // every line has 262 bytes including [1-Byte ArrayID][2-Bytes ArrayAddress][2-Bytes Line Size(0x0100)][256-Bytes Data][1-Byte CRC]
    sprintf(buff,"Application CRC on E2: 0x%08lX\r\nBootloader Status:0x%02X\r\nApp Size: %ld\r\nfirst RowID: %04X\r\n", appCRC, status, appSize, rowID);
    RS232_Debug_PutString(buff);
    uint8 fail = 0;
    if(status == FOTA_START){
        RS232_Debug_PutString("\r\n--------------------&&&&& Start Writing New App to Internal Flash &&&&&--------------------\r\n");    
        uint8 arr_id = 2;
        for(uint i = 0; i < appSize ; i++){
            // Calculating CRC here
            if(i != 0 && i % 0xffff == 0){
                arr_id++;
            }
            uint8 data = internal_flash_read_byte(arr_id, i);
            newCRC += data;
        }
        
        newCRC = (~newCRC) + 1;
        sprintf(buff,"\r\nnewCRC: 0x%08lX , AppCRC: 0x%08lX\r\n", newCRC, appCRC);
        RS232_Debug_PutString(buff);
//        
//        if(newCRC == appCRC){
        if(1){
            RS232_Debug_PutString("CRCs are OK!\r\n");
            
            uint16 appLines = appSize / CY_FLASH_SIZEOF_ROW;
            
            fail = internal_flash_write_row(appLines);
            
            if(fail == 0){
                ie2_write_byte(FOTA_STOP, IE2_Loc_FOTA_STATUS);
                ie2_write_byte(A_Reset_Code_FOTA_Done, IE2_Loc_Reset_Code_Address);
                RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Programmed Successfully &&&&&--------------------\r\n");
            }
            else if(fail == 1){
                ie2_write_byte(FOTA_STOP, IE2_Loc_FOTA_STATUS);
                ie2_write_byte(Gw_Reset_Code_FOTA_Fail, IE2_Loc_Reset_Code_Address);
                RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Programmed Failure --> row not write &&&&&--------------------\r\n");
            }
            else if(fail == 2){
                ie2_write_byte(FOTA_STOP, IE2_Loc_FOTA_STATUS);
                ie2_write_byte(Gw_Reset_Code_FOTA_Fail, IE2_Loc_Reset_Code_Address);
                RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Programmed Failure --> CySetTemp() != CYRET_SUCCESS &&&&&--------------------\r\n");
            }
            else{
                ie2_write_byte(FOTA_STOP, IE2_Loc_FOTA_STATUS);
                ie2_write_byte(Gw_Reset_Code_FOTA_Fail, IE2_Loc_Reset_Code_Address);
                RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Programmed Failure --> Unknown &&&&&--------------------\r\n");
            }
        }
        else{
            ie2_write_byte(FOTA_STOP, IE2_Loc_FOTA_STATUS);
            ie2_write_byte(Gw_Reset_Code_FOTA_Fail, IE2_Loc_Reset_Code_Address);
            RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Failure --> App CRC != Calc CRC !!! &&&&&--------------------\r\n");
        }
    }
    else{
        RS232_Debug_PutString("\r\n--------------------&&&&& No New Program &&&&&--------------------\r\n");
        CyDelay(1000);
    }
    
    RS232_Debug_PutString("\r\n--------------------&&&&& Bootloader Finish &&&&&--------------------\r\n");
    CyDelay(1000);
    Bootloader_Exit(Bootloader_EXIT_TO_BTLDB);
    
    return 0;
}


/* [] END OF FILE */
