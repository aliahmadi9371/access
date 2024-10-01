/*
 * IE2.h
 *
 *  Created on: Jun 6, 2023
 *      Author: IroTeam
 */

#ifndef INC_E2_H_
#define INC_E2_H_


// ###################################################### Includes #########################################################
#include "defines.h"
#include "i2c.h"
//#include "iwdg.h"
#include "stm32f1xx_hal_flash_ex.h"
// #########################################################################################################################


// ###################################################### Defines ########################################################
#define EE2_1_CHIP_ADDR       	0x56 << 1
#define EE2_2_CHIP_ADDR       	0x50 << 1
#define ETX_APP_START_ADDRESS   0x0800B400

// ---------------------------- EXTERNAL EEPROM METADATA ----------------------------
#define EE2_METADATA_START_ADDRESS			0
#define EE2_Loc_MyId                        (EE2_METADATA_START_ADDRESS + 0)

// IO EXPANDERS INFO
#define EE2_Loc_IOX_NUM                     (EE2_METADATA_START_ADDRESS + 1)
#define EE2_Loc_START_IOX_ID                (EE2_METADATA_START_ADDRESS + 2)  // MAX : 16 IDs >> ADDRESS: IE2_METADATA_START_ADDRESS + 18

#define EE2_Loc_OUTPUTS_NUM					(EE2_METADATA_START_ADDRESS + 8)

// Default Sensor Status
#define EE2_Loc_Default_Sensor_Status       (EE2_METADATA_START_ADDRESS + 19)

// FOTA METADATA
#define EE2_Loc_FOTA_STATUS                 (EE2_METADATA_START_ADDRESS + 20)
#define EE2_Loc_FOTA_CRC_BYTE0              (EE2_METADATA_START_ADDRESS + 21)
#define EE2_Loc_FOTA_CRC_BYTE1              (EE2_METADATA_START_ADDRESS + 22)
#define EE2_Loc_FOTA_CRC_BYTE2              (EE2_METADATA_START_ADDRESS + 23)
#define EE2_Loc_FOTA_CRC_BYTE3              (EE2_METADATA_START_ADDRESS + 24)
#define EE2_Loc_FOTA_App_SIZE_BYTE0         (EE2_METADATA_START_ADDRESS + 25)
#define EE2_Loc_FOTA_App_SIZE_BYTE1         (EE2_METADATA_START_ADDRESS + 26)
#define EE2_Loc_FOTA_App_SIZE_BYTE2         (EE2_METADATA_START_ADDRESS + 27)
#define EE2_Loc_FOTA_App_SIZE_BYTE3         (EE2_METADATA_START_ADDRESS + 28)
#define EE2_Loc_FOTA_ROW_START              (EE2_METADATA_START_ADDRESS + 29)

// WDT INFO
#define EE2_Loc_Wdt_Num_Byte3            	(EE2_METADATA_START_ADDRESS + 30)
#define EE2_Loc_Wdt_Num_Byte2            	(EE2_METADATA_START_ADDRESS + 31)
#define EE2_Loc_Wdt_Num_Byte1            	(EE2_METADATA_START_ADDRESS + 32)
#define EE2_Loc_Wdt_Num_Byte0            	(EE2_METADATA_START_ADDRESS + 33)

// SWT INFO
#define EE2_Loc_Swt_Num_Byte3            	(EE2_METADATA_START_ADDRESS + 34)
#define EE2_Loc_Swt_Num_Byte2            	(EE2_METADATA_START_ADDRESS + 35)
#define EE2_Loc_Swt_Num_Byte1            	(EE2_METADATA_START_ADDRESS + 36)
#define EE2_Loc_Swt_Num_Byte0            	(EE2_METADATA_START_ADDRESS + 37)

// RESET_STATUS INFO
#define EE2_Loc_Reset_Status             	(EE2_METADATA_START_ADDRESS + 38)
#define EE2_Loc_Reset_Code_Address         	(EE2_METADATA_START_ADDRESS + 39)

// MQTT INFO
#define EE2_Loc_EMQ_USERNAME				(EE2_METADATA_START_ADDRESS + 40)
#define EE2_Loc_EMQ_PASS					(EE2_METADATA_START_ADDRESS + 60)

// FTP INFO
#define EE2_Loc_FTP_USERNAME				(EE2_METADATA_START_ADDRESS + 80)
#define EE2_Loc_FTP_PASS					(EE2_METADATA_START_ADDRESS + 100)

// SENSOR INFO
#define EE2_Loc_Sen_Num						(EE2_METADATA_START_ADDRESS + 120)
#define EE2_Loc_Sen_Str						(EE2_METADATA_START_ADDRESS + 121)

// Internal FLASH Addresses
#define	IE2_FOTA_START_ADDRESS				0x8040400

// ---------------------------- EXTERNAL EEPROM METADATAS ----------------------------
// ---------------------------- RESET REASON ----------------------------
#define Reset_Code_Server          		  	0
#define Reset_Code_FOTA_Fail				1
#define Reset_Code_FOTA_Done              	2
#define Reset_Code_Invalid_SiliconId      	3
#define Reset_Code_Mqtt_Open_Fail        	4
#define Reset_Code_Mqtt_Open_Resp_Fail   	5
#define Reset_Code_Mqtt_Conn_Resp_Fail   	6
#define Reset_Code_Mqtt_Sub_Resp_Fail    	7
#define Reset_Code_Mqtt_Pub_Resp_Fail    	8
#define Reset_Code_Quectel_Lost          	9
#define Reset_Code_I2C_Lost          	 	10
// ---------------------------- RESET REASON ----------------------------
// #########################################################################################################################
// ###################################################### Variables ########################################################
// #########################################################################################################################

// ###################################################### Prototypes #######################################################
// ###################################################### Internal FLASH ###################################################
extern uint16_t internal_flash_read_byte(uint32_t Flash_Address);
extern HAL_StatusTypeDef internal_flash_write_byte(uint32_t Flash_Address, uint32_t data);
extern uint8_t internal_flash_erase();
// #########################################################################################################################
// ###################################################### External EE ######################################################
extern HAL_StatusTypeDef ee2_read_byte(uint8_t chipAddr, uint8_t* byte, uint16_t addr);
extern HAL_StatusTypeDef ee2_write_byte(uint8_t chipAddr, uint8_t byte, uint16_t addr);
extern uint8_t ee2_full_write(uint8_t chipAddr, uint8_t byte, uint16_t add);

extern uint8_t ee2_read_myId();
extern void ee2_write_myId();

extern uint8_t ee2_read_wdt();
extern void ee2_write_wdt();

extern uint8_t ee2_read_swt();
extern void ee2_write_swt();

extern void ee2_write_init(uint16_t from, uint16_t to);

extern void init_info(uint16_t start, uint16_t stop);
// #########################################################################################################################

#endif /* INC_E2_H_ */
