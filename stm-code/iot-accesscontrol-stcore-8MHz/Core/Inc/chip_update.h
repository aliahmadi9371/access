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
#include "main.h"

//#include "microchipMonitoring.h"
#include "iwdg.h"

// #########################################################################################################################

#define FOTA_MAX_FILE_LINE                  30 // FOR STM .hex file

// ###################################################### Variables ########################################################
extern uint8_t former_percent;
extern uint16_t line_num;
extern uint16_t i2c_counter;

extern unsigned int data_length;
extern uint8_t fota_data_bytes[16 * FOTA_MAX_FILE_LINE];
extern uint16_t fota_data_index;
extern int data_index;
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
extern void update_start(uint8_t fota_flag, char* fota_data);
extern void update_procedure(char* msg);

extern void proc_reset_wdt(uint8_t reset);
extern void proc_reset_chip(uint8_t reset_code);

extern uint8_t check_checksum(char* data, uint16_t data_checksum);
// #########################################################################################################################

#endif /* INC_PROC_H_ */
