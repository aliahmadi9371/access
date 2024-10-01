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
// #########################################################################################################################
    
// ###################################################### Variables ########################################################
OS_TIMER Timer_Read_Linux;
OS_TIMER Timer_RGB_LED;
OS_TIMER Timer_Read_W_Port1;
OS_TIMER Timer_Read_W_Port2;
OS_TIMER Timer_AfterRead_In1;
OS_TIMER Timer_AfterRead_Port1;
OS_TIMER Timer_AfterRead_In2; 
OS_TIMER Timer_AfterRead_Port2;
OS_TIMER Timer_WD;
// #########################################################################################################################
    
// ###################################################### Prototypes #######################################################
//Job of each Port
void Port1_Job(uint8 intr);
void Port2_Job(uint8 intr);
    

//Main Job
void CmdTask(char* msg);


//Interrupt Service Routine
CY_ISR(UART_Linux_Rx_ISR);
CY_ISR(Port1_ISR);
CY_ISR(Port2_ISR);


//Callbacks
void Timer_Read_Linux_Callback();
void Timer_Read_W_Port1_Callback();
void Timer_Read_W_Port2_Callback();
void Timer_AfterRead_Input1_Callback();
void Timer_AfterRead_Input2_Callback();
void Timer_AfterRead_Port1_Callback();
void Timer_AfterRead_Port2_Callback();

//Init Functions
void intrrupt_init();
void init_vars();
void timers_create();
// #########################################################################################################################
#endif  // OS_MAIN_H
/* [] END OF FILE */
