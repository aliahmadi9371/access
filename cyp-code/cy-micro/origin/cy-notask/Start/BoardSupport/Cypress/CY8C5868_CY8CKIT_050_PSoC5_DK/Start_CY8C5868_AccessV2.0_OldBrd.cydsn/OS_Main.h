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
#ifndef OS_MAIN_H
#define OS_MAIN_H

// ###################################################### Includes #########################################################
#include "defines.h"
#include "IO.h"
#include "ChipMonitor.h"
#include "ChipUpdate.h"
// #########################################################################################################################
    
// ###################################################### Variables ########################################################
extern OS_TIMER Timer_Read_Linux;
extern OS_TIMER Timer_RGB_LED;
extern OS_TIMER Timer_Read_W_Port1;
extern OS_TIMER Timer_Read_W_Port2;
extern OS_TIMER Timer_AfterRead_In1;
extern OS_TIMER Timer_AfterRead_Port1;
extern OS_TIMER Timer_AfterRead_In2;
extern OS_TIMER Timer_AfterRead_Port2;
extern OS_TIMER Timer_WD;
extern OS_TIMER Timer_Inps;
extern OS_TIMER Timer_Update;

    
extern OS_TIMER Timer_intr1;
extern OS_TIMER Timer_intr2;
// #########################################################################################################################
    
// ###################################################### Prototypes #######################################################
//Job of each Port
void Port1_Job(uint8 intr);
void Port2_Job(uint8 intr);
void Inputs_Job(); 
void Inps();

//Main Job
void CmdTask(char* msg);


//Interrupt Service Routine
extern CY_ISR(UART_Linux_Rx_ISR);
extern CY_ISR(Port1_ISR);
extern CY_ISR(Port2_ISR);


//Callbacks
//Callbacks
extern void Timer_Read_Linux_Callback();
extern void Timer_Read_W_Port1_Callback();
extern void Timer_Read_W_Port2_Callback();
extern void Timer_AfterRead_Input1_Callback();
extern void Timer_AfterRead_Input2_Callback();
extern void Timer_AfterRead_Port1_Callback();
extern void Timer_AfterRead_Port2_Callback();

//extern void Timer_R_Port1_Callback();
//extern void Timer_R_Port2_Callback();

extern void Timer_RGB_LED_Callback();
extern void Timer_WD_Callback();
extern void Timer_Inps_Callback();

extern void Timer_Update_Callback();

extern void Timer_Intr1_Callback();
extern void Timer_Intr2_Callback();

//Init Functions
void intrrupt_init();
void init_vars();
void timers_create();

void check_port_1(uint8 Port1, uint8* p1_0, uint8* p1_1);
void check_port_2(uint8 Port2, uint8* p2_0, uint8* p2_1);

void check_port_input_1(uint8 Port1_In);
void check_port_input_2(uint8 Port2_In);

void check_port_wiegand_1(uint8 Port1_W0, uint8 Port1_W1);
void check_port_wiegand_2(uint8 Port2_W0, uint8 Port2_W1);


#define STACK_TASK_Main_SIZE        2048
#define TASK_DELAY_Main_MS			10

///* USER CODE BEGIN Declarations_Task0 */
OS_TASK         TCB_Task_Main;                              // Control block for Task
OS_STACKPTR int Stack_Task_Main[STACK_TASK_Main_SIZE];            // Stack for Task

void Task_Main();
void Proc_Task_main();

extern uint8 inps_old;

extern uint8 inp_1_old;
extern uint8 inp_2_old;
// #########################################################################################################################
#endif  // OS_MAIN_H
/* [] END OF FILE */
