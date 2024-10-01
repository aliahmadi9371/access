/*
 * proc.c
 *
 *  Created on: Jul 22, 2023
 *      Author: IroTeam
 */
#include <chip_update.h>
#include <IO.h>

uint16_t line_num = 0;
uint16_t i2c_counter = 0;

unsigned int data_length = 0;
uint8_t fota_data_bytes[16 * FOTA_MAX_FILE_LINE];
uint16_t fota_data_index = 0;
int	data_index = 0;


uint8_t last_percent = 0;
// ###################################################### Init Functions ###################################################
//void proc_init()
//{
//    df_init_vars_all();
//
//    uint8_t byte = 0;
//	if(ee2_read_byte(EE2_1_CHIP_ADDR, &byte, 0x00) == HAL_OK){
//		if(byte == 0xFF){
//			ee2_write_init(0, 1024);
//		}
//		else{
//			UART_Debug_PutString("External EEPROM is OK\r\n");
//		}
//	}
//
//
//    // check if wdt
//
////    init_info(0x0000, 0x07D0);
//
////    proc_wdt();
//    UART_Debug_PutString("\r\n");
//
//
//    UART_Debug_PutString("\r\n ############################ Configurations Set ############################\r\n");
//
//    led_set_color(CYA);
//
//    return;
//}

//void proc_wdt()
//{
//
//	if(ee2_read_wdt() == 1 && ee2_read_swt() == 1){
//
//		uint8_t res = cm_get_last_wtd();
//
//		if(res == 1){
//			cm_wtd_cnt++;
//
//			char bf[100];
//			sprintf(bf, "proc_wdt: watchdog counter = %05lu \r\n", cm_wtd_cnt);
//			UART_Debug_PutString(bf, 0);
//
//			ee2_write_wdt();
//		}
//		else if(res == 2){
//			cm_swt_cnt++;
//
//			char bf[100];
//			sprintf(bf, "proc_swt: software counter = %05lu \r\n", cm_swt_cnt);
//			UART_Debug_PutString(bf, 0);
//
//			ee2_write_swt();
//		}
//	}
//
//    return;
//}

// #########################################################################################################################
// #########################################################################################################################
// ###################################################### For DAS ##########################################################

// ------------------------------------------------------ OTHER func ---------------------------------------------------------
//void proc_reset_wdt(uint8_t reset)
//{
//    if(reset){
//        cm_wtd_cnt = 0;
//        cm_swt_cnt = 0;
//        ee2_write_swt();
//        ee2_write_wdt();
//    }
//
//    return;
//}


void proc_reset_chip(uint8_t reset_code)
{
    HAL_IWDG_Refresh(&hiwdg);
    ee2_write_byte(EE2_1_CHIP_ADDR, reset_code, EE2_Loc_Reset_Code_Address);
//    HAL_Delay(500);
//    cm_read_reset_code();
//    HAL_Delay(1000);
    NVIC_SystemReset();

    return;
}
// #########################################################################################################################
// #########################################################################################################################
// ------------------------------------------------------ UPDATE proc --------------------------------------------------------
            void update_check_packet(char* msg)
            {
                //from LINUX --> {SET_FOT}:{"id":"00","data":["-","???","--","--","--------"]}
                if((memcmp((char*)&msg[0], "{SET_FOT}:", 10) == 0)){
//                    OS_TIMER_Restart(&Timer_Update);
                    char fota[128];
                    sprintf(fota, "{SET_FOT}:{\"id\":\"%02d\",\"data\":[", (unsigned int)MAIN_ID);
                    if((memcmp((char*)&msg[0], fota, 29) == 0)){
                        if(!fota_flag_start){
                            update_start(1, &((char*)msg)[29]);
                        }
                        // STOP & RESET FOTA STATE
						else{
							UART_Debug_PutString("\r\n #### NOTE: FOTA procedure STOPPED ####\r\n");
//							qctl_ftp_reset();
						}
                    }
                }
                //from LINUX --> {FOT_FOR}:{"[LINE_NUM]","[CHECKSUM"],"[DATA]"}
//                else if((memcmp((char*)&msg[0], "{FOT_FOT}:", 10) == 0)){
////                    OS_TIMER_Restart(&osTimer_reset_state);
//                    proc_fota_procedure(&((char*)msg)[10]);
//                }
                else{
                    UART_Debug_PutString("update_check_packet: wrong msg: ");
                    UART_Debug_PutString(msg);
                    UART_Debug_PutString("\r\n");
                }

                return;
            }
//-
//--
    //-
    //--
        //-
        //--
//-
//--
            // FYI >> WORKING
            void update_start(uint8_t fota_flag, char* fota_data)
            {
            //    [fota_status],[folder_name],[number_of_file],[number_of_all_lines],[Application_CRC]
            //    ["-","------------","---","---","--------"]}
            	// get FOTA command from server and parse it
            	strremove(fota_data, "[");
            	strremove(fota_data, "\"");
            	strremove(fota_data, "]");
            	strremove(fota_data, "}");

            	char* ret = NULL;

            	ret = strtok(fota_data, ",");
            	fota_flag_start = strtol(ret, NULL, 16);

                if(fota_flag_start == 1){

                    ret = strtok(NULL, ",");
                    strcpy(server_fota_folder_name, ret);

                    ret = strtok(NULL, ",");
                    server_fota_file_numbers = strtol(ret, NULL, 10);

                    ret = strtok(NULL, ",");
                    server_fota_App_size = strtoul(ret, NULL, 10);

                    ret = strtok(NULL, ",");

                    server_fota_App_CRC = strtoul(ret, NULL, 16);

                    if(fota_flag_start && server_fota_file_numbers && server_fota_App_size && server_fota_App_CRC ){
                    	if(internal_flash_erase() == 1){
                    		last_percent = 0;
                    		HAL_IWDG_Refresh(&hiwdg);
                    		char bf[200];
            				sprintf(bf, "fota_flag_start: %01X\r\nfota_folder_name: ", fota_flag_start);
            				strcat(bf, server_fota_folder_name);
            				strcat(bf, "\r\n");
            				UART_Debug_PutString(bf);

            				sprintf(bf, "fota_file_numbers: %03d\r\nfota_App_lines: %03d\r\nfota_App_size: %ld\r\nfota_App_CRC: %8lX\r\n",
            						server_fota_file_numbers,
            						server_fota_file_numbers,
            						server_fota_App_size,
            						server_fota_App_CRC);
            				UART_Debug_PutString(bf);

                    		UART_Debug_PutString("\r\n #### Note: FOTA Procedure Started!!!\r\n");
//                    		UART_Debug_PutString("{SET_FOT}:{\"status\":\"done\"}\r\n");
//                    		UART_Linux_PutString("{SET_FOT}:{\"status\":\"done\"}\r\n");

                    		HAL_IWDG_Refresh(&hiwdg);
                    		UART_Debug_PutString("{FOT_REQ}:{\"num\":\"001\"}\r\n");
//                			RGB_Color = 5;
//                			RGB_Color_Old = 5;
//                			RGB(RGB_Color); //Cypress run
//                    		UART_Linux_PutString("{FOT_REQ}:{\"num\":\"001\"}\r\n");
                    		if(!q_put_message(&_Queue_Send, "{FOT_REQ}:{\"num\":\"001\"}\r\n", strlen("{FOT_REQ}:{\"num\":\"001\"}\r\n") + 1)){
								UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
								UART_Debug_PutString("{FOT_REQ}:{\"num\":\"001\"}\r\n");
								UART_Debug_PutString("\r\n");
							}

                    	}
                    	else{
                        	UART_Debug_PutString("\r\n #### Note: FOTA Procedure FAILED!!!\r\n");
            //            	qctl_ftp_reset();
                    	}
                    }
                    else{
                    	UART_Debug_PutString("\r\n #### Note: FOTA Procedure FAILED!!!\r\n");
            //        	qctl_ftp_reset();
                    }

                }
                else{
                	UART_Debug_PutString("\r\n #### Note: FOTA Procedure STOPPED!!!\r\n");
            //    	qctl_ftp_reset();
                }


                return;
            }
uint8_t check_checksum(char* data, uint16_t data_checksum)
{
	uint8_t ret = 0;
	uint16_t our_checksum = 0;

	if(strlen(data) % 2 == 0){
		for(uint16_t i = 0; i < strlen(data); i = i+2){
			uint16_t byte = 0;
			byte = (uint16_t)data[i] << 8 | (uint16_t)data[i + 1];

			our_checksum ^= byte;
		}
	}

	if(our_checksum == data_checksum){
		ret = 1;
	}
	else{
		char print[100];
		sprintf(print, "our_checksum = %04X, data_checksum = %04X\r\n", our_checksum, data_checksum);
		UART_Debug_PutString(print);
	}

	return ret;
}

// ------------------------------------------------------ UPDATE proc -----------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------
// FYI >> WORKING
void update_procedure(char* buff)
{
    // Gateway
//	char data[50];
//	sprintf(data, "proc_Gw_fota_procedure >> file length = %d\r\n", strlen(buff));

	char*	linux_fotu_packet = buff;
	char*	ret = NULL;
	char	req_line_buf[100] = {'\0'};

	strremove(linux_fotu_packet, "[");
	strremove(linux_fotu_packet, "\"");
	strremove(linux_fotu_packet, "]");
	strremove(linux_fotu_packet, "}");
	strremove(linux_fotu_packet, "{");

	ret = strtok(linux_fotu_packet, ",");
	uint16_t linux_line_num = strtol(ret, NULL, 10);

	ret = strtok(NULL, ",");
	uint16_t linux_data_checksum = strtol(ret, NULL, 16);

	ret = strtok(NULL, ",");
	char* linux_update_data = ret;

	uint8_t checksum_rez = check_checksum(linux_update_data, linux_data_checksum);

	if(checksum_rez == 1){
		//char buf[100];
		//sprintf(buf, "LINUX_LINE_NUM: %03d >> CHECKSUM >> OK\r\n",linux_line_num);
		//UART_Debug_PutString(buf, 0);
	}else{
		char buf[100];
		sprintf(buf, "LINUX_LINE_NUM: %03d >> CHECKSUM >> FAILED\r\n",linux_line_num);
//		led_set_color(RED);
		sprintf(req_line_buf,"{FOT_REQ}:{\"num\":\"%05d\"}\r\n",fota_row_counter + 1);
		UART_Debug_PutString(req_line_buf);
//		RGB_Color = 5;
//		RGB_Color_Old = 5;
//		RGB(RGB_Color); //Cypress run
//		UART_Linux_PutString(req_line_buf);
		if(!q_put_message(&_Queue_Send, req_line_buf, strlen(req_line_buf) + 1)){
			UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(req_line_buf);
			UART_Debug_PutString("\r\n");
		}

		return;
	}


	if(linux_line_num != fota_file_numbers){
		UART_Debug_PutString("REQUESTED_LINE_NUM >> FALSE\r\n"); // file number is wrong, request the same file from LINUX
//		led_set_color(RED);
		sprintf(req_line_buf,"{FOT_REQ}:{\"num\":\"%05d\"}\r\n",fota_row_counter + 1);
		UART_Debug_PutString(req_line_buf);
//		RGB_Color = 5;
//		RGB_Color_Old = 5;
//		RGB(RGB_Color); //Cypress run
//		UART_Linux_PutString(req_line_buf);
		if(!q_put_message(&_Queue_Send, req_line_buf, strlen(req_line_buf) + 1)){
			UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(req_line_buf);
			UART_Debug_PutString("\r\n");
		}


		return;
	}
	else{
		// UART_Debug_PutString("REQUESTED_LINE_NUM >> TRUE\r\n", 0);
//		led_set_color(OFF);
	}

//	UART_Debug_PutString(linux_update_data);
//	UART_Debug_PutString("\r\n");
	// convert a couple of FOTA data characters to a single byte
	fota_data_index = 0;

	if(strlen(linux_update_data) % 2 == 0){
		for (uint32_t i = 0; i < strlen(linux_update_data); i++){
			HAL_IWDG_Refresh(&hiwdg);

			int_byte = (int_byte << 4) | df_hexstring_hex(linux_update_data[i]);
			int_byte_ctr++;
			if(int_byte_ctr == 2){
				fota_data_bytes[fota_data_index] = int_byte;
				fota_data_index++;
				int_byte = 0;
				int_byte_ctr = 0;
			}
		}
	}
	else{
		UART_Debug_PutString("update_procedure >> wrong data >> odd \r\n");
		sprintf(req_line_buf,"{FOT_REQ}:{\"num\":\"%05d\"}\r\n",fota_row_counter + 1);
		UART_Debug_PutString(req_line_buf);
//		RGB_Color = 5;
//		RGB_Color_Old = 5;
//		RGB(RGB_Color); //Cypress run
//		UART_Linux_PutString(req_line_buf);
		if(!q_put_message(&_Queue_Send, req_line_buf, strlen(req_line_buf) + 1)){
			UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(req_line_buf);
			UART_Debug_PutString("\r\n");
		}
		return;
	}

	// ##################################################


//	char buf_buf[100];
	// program FOTA data into Internal Flash
	if(strlen(linux_update_data) % 2 == 0 && (strlen(linux_update_data) / 2) % 2 == 0){
		for(uint16_t k = 0; k < strlen(linux_update_data) / 2 ; k = k + 4){
			// must check size of FOTA Data and External E2 to change the Address if needed.
			HAL_IWDG_Refresh(&hiwdg);

			uint32_t four_bytes = fota_data_bytes[k + 3] << 24 | fota_data_bytes[k + 2] << 16 | fota_data_bytes[k + 1] << 8 | fota_data_bytes[k];

			internal_flash_write_byte(IE2_FOTA_START_ADDRESS + data_index + k, four_bytes);

		}
	}
	else{
		UART_Debug_PutString("update_procedure >> wrong data >> odd 2\r\n");
		sprintf(req_line_buf,"{FOT_REQ}:{\"num\":\"%05d\"}\r\n",fota_row_counter + 1);
		UART_Debug_PutString(req_line_buf);
//		RGB_Color = 5;
//		RGB_Color_Old = 5;
//		RGB(RGB_Color); //Cypress run
//		UART_Linux_PutString(req_line_buf);
		if(!q_put_message(&_Queue_Send, req_line_buf, strlen(req_line_buf) + 1)){
			UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(req_line_buf);
			UART_Debug_PutString("\r\n");
		}
		return;
	}

	data_index += (strlen(linux_update_data) / 2);


	fota_row_counter++;




	// ##################################################

	if(fota_row_counter == server_fota_file_numbers){

//		led_set_color(BLU);
//	    OS_TIMER_Stop(&Timer_Update);

		fota_row_sequence = 0;
		fota_row_counter = 0;

		// set FOTA FLAG to E2PROM for Bootloader and Reset Application and go to Bootloader Mode
		ee2_write_byte(EE2_1_CHIP_ADDR, FOTA_START, EE2_Loc_FOTA_STATUS);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_CRC >> 0, EE2_Loc_FOTA_CRC_BYTE0);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_CRC >> 8, EE2_Loc_FOTA_CRC_BYTE1);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_CRC >> 16, EE2_Loc_FOTA_CRC_BYTE2);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_CRC >> 24, EE2_Loc_FOTA_CRC_BYTE3);

		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_size >> 0, EE2_Loc_FOTA_App_SIZE_BYTE0);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_size >> 8, EE2_Loc_FOTA_App_SIZE_BYTE1);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_size >> 16, EE2_Loc_FOTA_App_SIZE_BYTE2);
		ee2_write_byte(EE2_1_CHIP_ADDR, server_fota_App_size >> 24, EE2_Loc_FOTA_App_SIZE_BYTE3);

//		UART_Debug_PutString("{SET_FOT}:{\"status\":\"install\"}\r\n");
//		UART_Linux_PutString("{SET_FOT}:{\"status\":\"install\"}\r\n");

		proc_reset_chip(Reset_Code_FOTA_Done);

		return;
	}

	if(fota_file_numbers != server_fota_file_numbers){
		fota_file_numbers++;
//		led_set_color(GRE);

		sprintf(req_line_buf,"{FOT_REQ}:{\"num\":\"%05d\"}\r\n",fota_file_numbers);


		uint8_t percent = (((double)fota_file_numbers) / (double)server_fota_file_numbers) * 100;
		if(last_percent != percent){
			last_percent = percent;
			char data[100];
			sprintf(data, "update_procedure >> written lines : %d/%d\r\n", fota_row_counter, server_fota_file_numbers);
			UART_Debug_PutString(data);
			UART_Debug_PutString(req_line_buf);
		}

		if(percent % 2 == 0){
			RGB_Color = 5;
			RGB_Color_Old = 5;
			RGB(RGB_Color); //Cypress run
		}
		else{
			RGB_Color = 6;
			RGB_Color_Old = 6;
			RGB(RGB_Color); //Cypress run
		}
//		UART_Linux_PutString(req_line_buf);
		if(!q_put_message(&_Queue_Send, req_line_buf, strlen(req_line_buf) + 1)){
			UART_Debug_PutString("update_procedure >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(req_line_buf);
			UART_Debug_PutString("\r\n");
		}




	}
	else {
//		qctl_ftp_state = FTP_STATE_IDLE; // CHANGE FTP_STATE to IDLE
	}

    return;
}

// -------------------------------------------------------------------------------------------------------------------------
// #########################################################################################################################
// #########################################################################################################################
// #########################################################################################################################
