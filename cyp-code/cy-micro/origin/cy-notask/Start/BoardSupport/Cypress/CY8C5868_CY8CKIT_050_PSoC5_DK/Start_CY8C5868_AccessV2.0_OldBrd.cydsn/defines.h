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
#ifndef DEFINES_H
#define DEFINES_H

    
    
// ###################################################### Includes #########################################################   
#include "RTOS.h"
#include "BSP.h"
#include "OS_Config.h"
#include "device.h"
#include "project.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    
// ###################################################### Defines ##########################################################
#define APP_VERSION                         "Brd-V3.0-App-V2.0-(and-inps)"
#define MAIN_ID                         	0x00    
    
//RGB 3 pins define for LED_RGB port
#define RED                                 0b00000110
#define GRE                                 0b00000101
#define BLU                                 0b00000011
#define WHI                                 0b00000000
#define CYA                                 0b00000001
#define PUR                                 0b00000010
#define YEL                                 0b00000100
#define BLC                                 0b00000000
    
// Reset Status Metadata                                          
#define IE2_Loc_Reset_Status                0x12
#define IE2_Loc_Reset_Code_Address          0x13
// #########################################################################################################################

// #########################################################################################################################
// ---------------------------- FOTA Variables ----------------------------
extern uint8 fota_flag_start;

extern uint16 fota_file_numbers;
extern uint16 fota_row_counter;
extern uint16 fota_row_sequence;
extern uint16 fota_recv_byte_count;

extern uint32 server_fota_App_size;
extern uint32 server_fota_App_CRC;
extern char server_fota_folder_name[15];
extern uint16 server_fota_file_numbers;
extern uint16 server_fota_App_lines;

extern uint8 int_byte;
extern uint8 int_byte_ctr;

extern uint8 RGB_Color;
extern uint8 RGB_Color_Old;

// ###################################################### Prototypes #######################################################
extern uint8 df_hexstring_hex(char character);
extern char df_hex_hexstring(uint8 data);
extern void byte_to_binary(uint8 byte, char* str_binary);

// SORTING ALGORITHM
extern void swap(uint8* xp, uint8* yp);
extern	void selectionSort(uint8 arr[], uint8 arr_len);

extern char* strremove(char *str, const char *sub);

#define Extra_Size                   4

extern OS_Q _Queue_Main;
#define Message_Cnt_Main_Queue       5
#define Queue_Len_Main               1024
#define Queue_Size_Main              (Message_Cnt_Main_Queue * (Queue_Len_Main + OS_Q_SIZEOF_HEADER + Extra_Size))
extern char _QBuffer_Queue_Main[Queue_Size_Main];

extern OS_Q _Queue_Send;
#define Message_Cnt_Send_Queue       3
#define Queue_Len_Send               256
#define Queue_Size_Send              (Message_Cnt_Send_Queue * (Queue_Len_Send + OS_Q_SIZEOF_HEADER + Extra_Size))
extern char _QBuffer_Queue_Send[Queue_Size_Send];

extern uint8 q_put_message(OS_QUEUE* queue, char* message, int lng);
extern void* q_get_message(OS_QUEUE* queue);
// #########################################################################################################################
    
#endif  // DEFINES_H
/* [] END OF FILE */
