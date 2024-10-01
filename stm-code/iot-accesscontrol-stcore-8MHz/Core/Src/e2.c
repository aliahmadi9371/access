/*
 * IE2.c
 *
 *  Created on: Jun 6, 2023
 *      Author: IroTeam
 */

#include <e2.h>
// ###################################################### EEProm Functional Functions ######################################
// use INTERNAL FLASH as Internal EEPROM
HAL_StatusTypeDef internal_flash_ret = HAL_ERROR;

HAL_StatusTypeDef internal_flash_write_byte(uint32_t Flash_Address, uint32_t data)
{
	// clear all flags before you write it to flash
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

	HAL_StatusTypeDef ret_write = HAL_ERROR;
	HAL_StatusTypeDef ret_lock = HAL_ERROR;

	ret_lock = HAL_FLASH_Unlock();
	if(ret_lock == HAL_OK){
		ret_write = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Address, (uint64_t)data);

		ret_lock = HAL_FLASH_Lock();

		if(ret_lock != HAL_OK){
			UART_Debug_PutString("internal_flash_write_byte: HAL_FLASH_Lock >> FAILED	*****	\r\n");
		}
	}
	else{
		UART_Debug_PutString("internal_flash_write_byte: HAL_FLASH_Unlock >> FAILED\r\n");
	}

	return ret_write;
}

uint16_t internal_flash_read_byte(uint32_t Flash_Address){
	__IO uint32_t read_data = *(__IO uint32_t *)Flash_Address;
	return (uint16_t)read_data;
}

uint8_t internal_flash_erase()
{
	UART_Debug_PutString("internal_flash_erase:Erasing FLASH Memory, Please wait...\r\n");

	uint8_t ret = 1;
	HAL_StatusTypeDef status;
	status = HAL_FLASH_Unlock();

	if(status == HAL_OK){
		FLASH_EraseInitTypeDef EraseInitStruct;
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = IE2_FOTA_START_ADDRESS; // first address of the SECOND half of INTERNAL flash (for FOTA data)
		EraseInitStruct.NbPages = 100;							// 100 * 2K Page

		uint32_t PageError;
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){		//Erase the Page Before a Write Operation
			UART_Debug_PutString("internal_flash_erase: ERASE FLASH >> FAILED\r\n");
			ret = 0;
		}
		else{
			UART_Debug_PutString("internal_flash_erase: ERASE FLASH >> DONE\r\n");
		}
	}

	status = HAL_FLASH_Lock();

	if(status != HAL_OK){
		ret = 0;
	}

	return ret;
}
// #########################################################################################################################
// ###################################################### External EEProm Functions ######################################
HAL_StatusTypeDef ee2_read_byte(uint8_t chipAddr, uint8_t* byte, uint16_t addr)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t i = 0;

	while(status != HAL_OK && i < 1000){
		status = HAL_I2C_Mem_Read(&hi2c1, chipAddr, addr, 2, byte, 1, 500);
		if(status != HAL_OK){
			UART_Debug_PutString("ee2_read_byte: HAL_I2C_Mem_Read >> status != HAL_OK\r\n");
		}
		i++;
		//HAL_IWDG_Refresh(&hiwdg);
	}

    return status;
}
HAL_StatusTypeDef ee2_write_byte(uint8_t chipAddr, uint8_t byte, uint16_t addr)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t i = 0;

	while(status != HAL_OK && i < 200){
		status = HAL_I2C_IsDeviceReady(&hi2c1, chipAddr, 0x00000001, 500);

		if(status == HAL_OK){
			status = HAL_ERROR;
			status = HAL_I2C_Mem_Write(&hi2c1, chipAddr, addr, 2, &byte, 1, 500);  // write the data to the EEPROM
			if(status != HAL_OK){
				UART_Debug_PutString("ee2_write_byte:  HAL_I2C_Mem_Write: status != HAL_OK\r\n");
			}
//			OS_TASK_Delay(5);   // delay for writing data
		}
		else if(i == 0){
				UART_Debug_PutString("ee2_write_byte:  HAL_I2C_IsDeviceReady: status != HAL_OK\r\n");
//				OS_TASK_Delay(5);
		}

		i++;
		//HAL_IWDG_Refresh(&hiwdg);
	}

    return status;
}

uint8_t ee2_full_write(uint8_t chipAddr, uint8_t byte, uint16_t add)
{
	uint8_t ret = 0;
	HAL_StatusTypeDef status = ee2_write_byte(chipAddr , byte, add);
    if(status == HAL_OK){
    	uint8_t data;
    	status = ee2_read_byte(chipAddr, &data, add);
    	if(status == HAL_OK){
    		if(data == byte){
    			ret = 1;
    		}
    	}
    }

    return ret;
}
// ###################################################### EEProm My Id Functions ###########################################
uint8_t ee2_read_myId()
{
	uint8_t id = 0;
	uint8_t ret = 0;
    if(ee2_read_byte(EE2_1_CHIP_ADDR, &id, EE2_Loc_MyId) == HAL_OK){
    	ret = 1;
    	char data[100];
    	sprintf(data, "ee2_read_myId: read << My ID = %02u >> from location %05u\r\n", ((unsigned int)(id)), ((unsigned int)(EE2_Loc_MyId)));
    	UART_Debug_PutString(data);
    }
    else{
    	UART_Debug_PutString("ee2_read_myId: failed\r\n");
    }

    return ret;
}

void ee2_write_myId()
{
	HAL_StatusTypeDef status = HAL_ERROR;
	status = ee2_write_byte(EE2_1_CHIP_ADDR, MAIN_ID, EE2_Loc_MyId);
    if(status != HAL_OK){
    	char data[100];
        sprintf(data, "ee2_write_myId: failed write << My ID : %02u >> to location %05u\r\n", ((unsigned int)(MAIN_ID)), ((unsigned int)(EE2_Loc_MyId)));
        UART_Debug_PutString(data);

    }
    else{
    	char data[100];
        sprintf(data, "ee2_write_myId: successful write << My ID : %02u >> to location %05u\r\n", ((unsigned int)(MAIN_ID)), ((unsigned int)(EE2_Loc_MyId)));
        UART_Debug_PutString(data);
    }

    return;
}
// #########################################################################################################################

// ###################################################### EEProm Watchdog Functions ########################################
//uint8_t ee2_read_wdt()
//{
//	uint8_t ret = 0;
//
//    uint32_t w_num = 0;
//    uint8_t w_num1 = 0;
//    uint8_t w_num2 = 0;
//    uint8_t w_num3 = 0;
//    uint8_t w_num4 = 0;
//
//    if(ee2_read_byte(EE2_1_CHIP_ADDR, &w_num1, EE2_Loc_Wdt_Num_Byte3) == HAL_OK &&
//		ee2_read_byte(EE2_1_CHIP_ADDR, &w_num2, EE2_Loc_Wdt_Num_Byte2) == HAL_OK &&
//		ee2_read_byte(EE2_1_CHIP_ADDR, &w_num3, EE2_Loc_Wdt_Num_Byte1) == HAL_OK &&
//		ee2_read_byte(EE2_1_CHIP_ADDR, &w_num4, EE2_Loc_Wdt_Num_Byte0) == HAL_OK){
//
//    	ret = 1;
//    	w_num = ((uint32_t)w_num1 << 24) + ((uint32_t)w_num2 << 16) + ((uint32_t)w_num3 << 8) + ((uint32_t)w_num4 << 0);
//
//    	cm_wtd_cnt = w_num;
//
//    	char data[100];
//    	sprintf(data, "ee2_read_wdt: read << cy_wtd_cnt = %05lu >> \r\n", cm_wtd_cnt);
//    	UART_Debug_PutString(data, 0);
//    }
//    else{
//    	UART_Debug_PutString("ee2_read_wdt: failed\r\n", 0);
//    }
//
//    return ret;
//}
//
//void ee2_write_wdt()
//{
//    uint8_t w_num1 = (uint8_t)(cm_wtd_cnt >> 24);
//    uint8_t w_num2 = (uint8_t)(cm_wtd_cnt >> 16);
//    uint8_t w_num3 = (uint8_t)(cm_wtd_cnt >> 8);
//    uint8_t w_num4 = (uint8_t)(cm_wtd_cnt >> 0);
//
//    HAL_StatusTypeDef ret;
//    char buff[200];
//    ret = ee2_write_byte(EE2_1_CHIP_ADDR, w_num1, EE2_Loc_Wdt_Num_Byte3);
//    if(ret != HAL_OK){
//        sprintf(buff, "ee2_write_wdt: failed write 0x%02X >> to location %05u\r\n", w_num1, EE2_Loc_Wdt_Num_Byte3);
//        UART_Debug_PutString(buff, 0);
//    }
//    else{
//        ret = 0;
//        ret = ee2_write_byte(EE2_1_CHIP_ADDR, w_num2, EE2_Loc_Wdt_Num_Byte2);
//        if(ret != HAL_OK){
//            sprintf(buff,"ee2_write_wdt: failed write << 0x%02X >> to location %05u\r\n", w_num2, EE2_Loc_Wdt_Num_Byte2);
//            UART_Debug_PutString(buff, 0);
//        }
//        else{
//            ret = 0;
//            ret = ee2_write_byte(EE2_1_CHIP_ADDR, w_num3, EE2_Loc_Wdt_Num_Byte1);
//            if(ret != HAL_OK){
//                sprintf(buff, "ee2_write_wdt: failed write << 0x%02X >> to location %05u\r\n", w_num3, EE2_Loc_Wdt_Num_Byte1);
//                UART_Debug_PutString(buff, 0);
//            }
//            else{
//                ret = 0;
//                ret = ee2_write_byte(EE2_1_CHIP_ADDR, w_num4, EE2_Loc_Wdt_Num_Byte0);
//                if(ret != HAL_OK){
//                    sprintf(buff, "ee2_write_wdt: failed write << 0x%02X >> to location %05u\r\n", w_num4, EE2_Loc_Wdt_Num_Byte0);
//                    UART_Debug_PutString(buff, 0);
//                }
//                else{
//                    sprintf(buff, "ee2_write_wdt: << cy_wtd_cnt = %04lX >> \r\n", cm_wtd_cnt);
//                    UART_Debug_PutString(buff, 0);
//                }
//            }
//        }
//    }
//
//    ee2_read_wdt();
//
//    return;
//}
// #########################################################################################################################


//uint8_t ee2_read_swt()
//{
//	uint8_t ret = 0;
//
//    uint32_t s_num = 0;
//    uint8_t s_num1;
//    uint8_t s_num2;
//    uint8_t s_num3;
//    uint8_t s_num4;
//
//    if( ee2_read_byte(EE2_1_CHIP_ADDR, &s_num1, EE2_Loc_Swt_Num_Byte3) == HAL_OK &&
//	ee2_read_byte(EE2_1_CHIP_ADDR, &s_num2, EE2_Loc_Swt_Num_Byte2) == HAL_OK &&
//	ee2_read_byte(EE2_1_CHIP_ADDR, &s_num3, EE2_Loc_Swt_Num_Byte1)  == HAL_OK &&
//	ee2_read_byte(EE2_1_CHIP_ADDR, &s_num4, EE2_Loc_Swt_Num_Byte0) == HAL_OK){
//
//    	ret = 1;
//    	s_num = ((uint32_t)s_num1 << 24) + ((uint32_t)s_num2 << 16) + ((uint32_t)s_num3 << 8) + ((uint32_t)s_num4 << 0);
//
//    	cm_swt_cnt = s_num;
//
//    	char buff[200];
//    	sprintf(buff, "ee2_read_swt: read << Das_cy_swt_cnt = %05lu >> \r\n", cm_swt_cnt);
//    	UART_Debug_PutString(buff, 0);
//    }
//    else{
//    	UART_Debug_PutString("ee2_read_swt: failed\r\n", 0);
//    }
//
//
//    return ret;
//}
//
//void ee2_write_swt()
//{
//    uint8_t s_num1 = (uint8_t)(cm_swt_cnt >> 24);
//    uint8_t s_num2 = (uint8_t)(cm_swt_cnt >> 16);
//    uint8_t s_num3 = (uint8_t)(cm_swt_cnt >> 8);
//    uint8_t s_num4 = (uint8_t)(cm_swt_cnt >> 0);
//
//    HAL_StatusTypeDef ret;
//    char buff[200];
//    ret = ee2_write_byte(EE2_1_CHIP_ADDR, s_num1, EE2_Loc_Swt_Num_Byte3);
//    if(ret != HAL_OK){
//        sprintf(buff, "ee2_write_swt: failed write 0x%02X >> to location %05u\r\n", s_num1, EE2_Loc_Swt_Num_Byte3);
//        UART_Debug_PutString(buff, 0);
//    }
//    else{
//        ret = 0;
//        ret = ee2_write_byte(EE2_1_CHIP_ADDR, s_num2, EE2_Loc_Swt_Num_Byte2);
//        if(ret != HAL_OK){
//            sprintf(buff, "ee2_write_swt: failed write << 0x%02X >> to location %05u\r\n", s_num2, EE2_Loc_Swt_Num_Byte2);
//            UART_Debug_PutString(buff, 0);
//
//        }
//        else{
//            ret = 0;
//            ret = ee2_write_byte(EE2_1_CHIP_ADDR, s_num3, EE2_Loc_Swt_Num_Byte1);
//            if(ret != HAL_OK){
//                sprintf(buff, "ee2_write_swt: failed write << 0x%02X >> to location %05u\r\n", s_num3, EE2_Loc_Swt_Num_Byte1);
//                UART_Debug_PutString(buff, 0);
//            }
//            else{
//                ret = 0;
//                ret = ee2_write_byte(EE2_1_CHIP_ADDR, s_num4, EE2_Loc_Swt_Num_Byte0);
//                if(ret != HAL_OK){
//                	sprintf(buff, "ee2_write_swt: failed write << 0x%02X >> to location %05u\r\n", s_num4, EE2_Loc_Swt_Num_Byte0);
//                	UART_Debug_PutString(buff, 0);
//                }
//                else{
//                	sprintf(buff, "ee2_write_swt: << Das_cy_swt_cnt = %04lX >> \r\n", cm_swt_cnt);
//                	UART_Debug_PutString(buff, 0);
//                }
//            }
//        }
//    }
//
//    return;
//}

void ee2_write_init(uint16_t from, uint16_t to)
{
	uint8_t ret = 0;
    if(from < to){
        for(uint16_t i = from; i < to; i++){
    		//HAL_IWDG_Refresh(&hiwdg);
            if(ee2_write_byte(EE2_1_CHIP_ADDR, 0, i) != HAL_OK){
            	UART_Debug_PutString("ee2_write_init: Error\r\n");
            	ret = 1;
            	break;
            }
        }
        if(ret != 1){
        	UART_Debug_PutString("ie2_write_init: Ok\r\n");
		}
    }
    else{
    	UART_Debug_PutString("ie2_write_init: Bad Address\r\n");
    }

    return;
}

void init_info(uint16_t start, uint16_t stop)
{
	uint16_t len = stop - start + 1;
	for(uint16_t i = start; i < start + len; i++){
		//HAL_IWDG_Refresh(&hiwdg);
		ee2_write_byte(EE2_1_CHIP_ADDR, 0x00, i);
	}
	UART_Debug_PutString("\r\n");

	return;
}
// #########################################################################################################################


/* [] END OF FILE */

