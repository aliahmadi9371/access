/*
 * proc.h
 *
 *  Created on: Jul 22, 2023
 *      Author: IroTeam
 */

#ifndef INC_PROC_H_
#define INC_PROC_H_

// ###################################################### Headers ########################################################
#include "e2.h"
#include "defines.h"
#include "OS_Main.h"

//#include "microchipMonitoring.h"
//#include "iwdg.h"

// #########################################################################################################################

#define FOTA_MAX_FILE_LINE                  1 // FOR STM .hex file

// ###################################################### Variables ########################################################
extern uint8 former_percent;
extern uint16 line_num;
extern uint16 i2c_counter;

extern unsigned int data_length;
extern uint8 fota_data_bytes[FOTA_ROW_SIZE * FOTA_MAX_FILE_LINE];
extern uint16 fota_data_index;
extern int data_index;

//uint8 fb[FOTA_ROW_SIZE];
// #########################################################################################################################

// ###################################################### Prototypes #######################################################
//extern void proc_init();
//    extern void proc_wdt();

//extern void proc_task_operator();
//    extern void proc_task_operator_QueueSet();
//        extern void proc_task_operator_QueueSet_separate(char* msg);
//extern void proc_separate_fota_resp(char* data);
//extern void proc_rtc();

extern void update_check_packet();
extern void update_start(uint8 fota_flag, char* fota_data);
extern void update_procedure(char* msg);

extern void proc_reset_wdt(uint8 reset);
extern void proc_reset_chip(uint8 reset_code);

extern uint8 check_checksum(char* data, uint16 data_checksum);

extern uint8 internal_flash_write_row();
extern uint8 internal_flash_handle_error(cystatus value);

// #########################################################################################################################

#endif /* INC_PROC_H_ */
