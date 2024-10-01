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

#ifndef IE2_H
#define IE2_H
    
// ###################################################### Includes #########################################################
#include "defines.h"
// #########################################################################################################################


// ###################################################### Defines ########################################################    
#define EE2_1_CHIP_ADDR                             0x52
#define EE2_2_CHIP_ADDR                             0x50
    
// ---------------------------- External EEPROM Metadata ----------------------------
#define IE2_Loc_MyId                                0x00
//#define IE2_Loc_DasNum                              0x01
//// IO Expander Metadata
//#define IE2_Loc_IOX_NUM                             0x02
//#define IE2_Loc_START_IOX_ID                        0x03       // maximum number for iox is 1 --> from location 3 to 4 reserved!
//// CCLK Metadata
//#define IE2_Loc_cclk_state                          0x07    
//// OUTPUT Numbers
//#define IE2_Loc_OUTPUTS_NUM                         0x08
// WDT Metadata
//#define IE2_Loc_Wdt_Num_Byte3                       0x0A
//#define IE2_Loc_Wdt_Num_Byte2                       0x0B
//#define IE2_Loc_Wdt_Num_Byte1                       0x0C
//#define IE2_Loc_Wdt_Num_Byte0                       0x0D
//// SWT Metadata                                                    
//#define IE2_Loc_Swt_Num_Byte3                       0x0E
//#define IE2_Loc_Swt_Num_Byte2                       0x0F
//#define IE2_Loc_Swt_Num_Byte1                       0x10
//#define IE2_Loc_Swt_Num_Byte0                       0x11
// Reset Status Metadata                                          
#define IE2_Loc_Reset_Status                        0x12
#define IE2_Loc_Reset_Code_Address                  0x13
    
   
// FOTA Metadata
#define FOTA_STOP                                   0
#define FOTA_START                                  1
#define FOTA_ROW_SIZE                               262
    
#define IE2_Loc_FOTA_STATUS                         0x70
#define IE2_Loc_FOTA_CRC_BYTE0                      0x71
#define IE2_Loc_FOTA_CRC_BYTE1                      0x72
#define IE2_Loc_FOTA_CRC_BYTE2                      0x73
#define IE2_Loc_FOTA_CRC_BYTE3                      0x74
#define IE2_Loc_FOTA_App_SIZE_BYTE0                 0x75
#define IE2_Loc_FOTA_App_SIZE_BYTE1                 0x76
#define IE2_Loc_FOTA_App_SIZE_BYTE2                 0x77  
#define IE2_Loc_FOTA_App_SIZE_BYTE3                 0x78
#define IE2_Loc_FOTA_ROW_START_BYTE0                0x79
#define IE2_Loc_FOTA_ROW_START_BYTE1                0x7A
    
 // ---------------------------- RESET REASON ----------------------------
#define Reset_Code_Server          		  	        11
#define Reset_Code_FOTA_Fail				        1
#define Reset_Code_FOTA_Done              	        2
#define Reset_Code_Invalid_SiliconId      	        3
#define Reset_Code_Mqtt_Open_Fail        	        4
#define Reset_Code_Mqtt_Open_Resp_Fail   	        5
#define Reset_Code_Mqtt_Conn_Resp_Fail   	        6
#define Reset_Code_Mqtt_Sub_Resp_Fail    	        7
#define Reset_Code_Mqtt_Pub_Resp_Fail    	        8
#define Reset_Code_Quectel_Lost          	        9
#define Reset_Code_I2C_Lost          	 	        10  
// ---------------------------- RESET REASON ----------------------------
// #########################################################################################################################
    
// ###################################################### Variables ########################################################    
// #########################################################################################################################
    
// ###################################################### Prototypes #######################################################
// ###################################################################
extern uint8 ie2_read_byte(uint16 addr);
extern uint8 ie2_write_byte(uint8 byte, uint16 addr);
// ###################################################################
// #########################################################################################################################
#endif

/* [] END OF FILE */
