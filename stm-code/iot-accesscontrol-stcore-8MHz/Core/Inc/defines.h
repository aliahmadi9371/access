/*
 * defines.h
 *
 *  Created on: Jan 20, 2024
 *      Author: dasha
 */

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

// ###################################################### Defines ##########################################################
// #########################################################################################################################

// ###################################################### Includes #########################################################
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

//#include "i2c.h"
#include "usart.h"
#include "gpio.h"
// #########################################################################################################################
#define APP_VERSION                         	"Brd-V3.0-App-V2.0-(and-inps)"
#define MAIN_ID                         		0x00
// ---------------------------- FOTA Defines ----------------------------

#define FOTA_STOP                           	 0
#define FOTA_START                          	 1
#define FOTA_ROW_SIZE                       	 32 // FOR STM .hex file ! JUST DATA BYTES
#define FOTA_MAX_FILE_LINE                  	 30 // FOR STM .hex file

// ---------------------------- FOTA Variables ----------------------------

extern uint8_t fota_flag_start;

extern uint16_t fota_file_numbers;
extern uint16_t fota_row_counter;
extern uint16_t fota_row_sequence;
extern uint16_t fota_recv_byte_count;

extern uint32_t server_fota_App_size;
extern uint32_t server_fota_App_CRC;
extern char server_fota_folder_name[15];
extern uint16_t server_fota_file_numbers;
extern uint16_t server_fota_App_lines;

extern uint8_t int_byte;
extern uint8_t int_byte_ctr;

extern uint8_t RGB_Color;
extern uint8_t RGB_Color_Old;
// ###################################################### Prototypes #######################################################
extern uint8_t df_hexstring_hex(char character);
extern char df_hex_hexstring(uint8_t data);

extern char* strremove(char *str, const char *sub);
extern void byte_to_binary(uint8_t byte, char* str_binary);

// SORTING ALGORITHM
extern void swap(uint8_t* xp, uint8_t* yp);
extern	void selectionSort(uint8_t arr[], uint8_t arr_len);
// #########################################################################################################################
#define Extra_Size                  4
extern OS_Q _Queue_Main;
#define Message_Cnt_Main_Queue       3
#define Queue_Len_Main               256
#define Queue_Size_Main              (Message_Cnt_Main_Queue * (Queue_Len_Main + OS_Q_SIZEOF_HEADER + Extra_Size))
extern char _QBuffer_Queue_Main[Queue_Size_Main];

extern OS_Q _Queue_Send;
#define Message_Cnt_Send_Queue       3
#define Queue_Len_Send               256
#define Queue_Size_Send              (Message_Cnt_Send_Queue * (Queue_Len_Send + OS_Q_SIZEOF_HEADER + Extra_Size))
extern char _QBuffer_Queue_Send[Queue_Size_Send];

extern uint8_t q_put_message(OS_QUEUE* queue, char* message, int lng);
extern void* q_get_message(OS_QUEUE* queue);

#endif /* INC_DEFINES_H_ */
