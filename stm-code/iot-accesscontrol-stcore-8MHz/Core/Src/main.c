/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
//Port 1 Weignad:
//                  W0 -> In1 -> PC.0
//                  W1 -> In2 -> PC.1
//                  In -> In3 -> PC.2

//Port 2 Weigand:
//                  W0 -> In4 -> PA.5  XX1 XXX
//                  W1 -> In5 -> PA.6  X1X XXX
//                  In -> In6 -> PA.7  1XX XXX

//Inputs:
//                  In7  -> pB.12
//                  In8  -> pB.13
//                  In9  -> pB.14
//                  In10 -> pB.15
//                  In11 -> pC.9
//                  In12 -> pA.8

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Scheduler includes. */
#include "RTOS.h"

#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


#define STACK_TASK_Main_SIZE        2048
#define TASK_DELAY_Main_MS			10

///* USER CODE BEGIN Declarations_Task0 */
OS_TASK         TCB_Task_Main;                              // Control block for Task
OS_STACKPTR int Stack_Task_Main[STACK_TASK_Main_SIZE];            // Stack for Task

void Task_Main();
void Proc_Task_main();


OS_TIMER Timer_Read_Linux;
OS_TIMER Timer_RGB_LED;
OS_TIMER Timer_Read_W_Port1;
OS_TIMER Timer_Read_W_Port2;
OS_TIMER Timer_AfterRead_In1;
OS_TIMER Timer_AfterRead_Port1;
OS_TIMER Timer_AfterRead_In2;
OS_TIMER Timer_AfterRead_Port2;
OS_TIMER Timer_WD;
OS_TIMER Timer_Inps;
OS_TIMER Timer_Update;

//OS_TIMER Timer_RP1;
//OS_TIMER Timer_RP2;
OS_TIMER Timer_intr;

//static uint8_t debug_rx[1] = {'\0'};
uint8_t linux_rx[1] = {'\0'};

//static uint32_t read_counter_port1 = 0;
//static uint32_t read_counter_port2 = 0;
//static uint32_t read_counter_total = 0;

uint32_t read_failure_port1 = 0;
uint32_t read_failure_port2 = 0;
uint32_t read_failure_total = 0;

//static uint32_t linux_read_counter_port1 = 0;
//static uint32_t linux_read_counter_port2 = 0;
//static uint32_t linux_read_counter_total = 0;

//uint32_t linux_read_failure_port1 = 0;
//uint32_t linux_read_failure_port2 = 0;
//uint32_t linux_read_failure_total = 0;

//static uint32_t read_input_port1 = 0;
//static uint32_t read_input_port2 = 0;
//static uint32_t read_input_total = 0;

//uint32_t linux_read_input_failure_port1 = 0;
//uint32_t linux_read_input_failure_port2 = 0;
//uint32_t linux_read_input_failure_total = 0;

uint32_t linux_uart_failure = 0;

uint8_t Port1_Relay = 0;
uint8_t Port1_Relay_Old = 0;
uint8_t Port1_Locked = 0;
uint8_t Port1_Locked_Old = 0;
uint8_t Port1_w_Start = 0;
uint8_t Port1_w_have = 0;
char Port1_W[129];
char In1[3];
uint8_t Port1_W_Counter = 0;
uint8_t Port1_W_bits = 26;
uint8_t Port1_W_bits_Old = 26;

uint8_t Port2_Relay = 0;
uint8_t Port2_Relay_Old = 0;
uint8_t Port2_Locked = 0;
uint8_t Port2_Locked_Old = 0;
uint8_t Port2_w_Start = 0;
uint8_t Port2_w_have = 0;
char Port2_W[129];
char In2[3];
uint8_t Port2_W_Counter = 0;
uint8_t Port2_W_bits = 26;
uint8_t Port2_W_bits_Old = 26;

//static uint8_t UART_Linux_StartBit = 0;
char UART_Linux_Rx_Buffer[256];
uint16_t UART_Linux_Rx_Counter = 0;

//uint8_t RGB_Color = 0;
//uint8_t RGB_Color_Old = 0;

char Inputs[14] = "{\"inps\":\"FF\"}";
//char inp_old[14] = "{\"inps\":\"FF\"}";
uint8_t inps_old = 0b11111111;
//char Inputs_Old[20] = "";

uint8_t intr[512];
uint16_t intr_counter = 0;

uint8_t inp_1_old = 1;
uint8_t inp_2_old = 1;
///* USER CODE END Declarations_Task0 */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void UART_Debug_PutString(const char* msg)
{
	HAL_UART_Transmit(&huart3, (const uint8_t*)msg, strlen(msg), 500);
	return;
}

void UART_Linux_PutString(const char* msg)
{
//	for(uint16_t i = 0; i < strlen(msg) + 1; i++){
//		HAL_UART_Transmit(&huart1, (const uint8_t*)&msg[i], 1, 1);
//	}
	HAL_UART_Transmit(&huart1, (const uint8_t*)msg, strlen(msg) + 1, 500);
	return;
}

void UART_Linux_PutChar(char c)
{
	HAL_UART_Transmit(&huart1, ((const uint8_t*)&c), 1, 500);
	return;
}

void Ports_Job(uint8_t intr)
{
//	char data[100];
//	sprintf(data, "intr = 0x%02X\r\n", intr);
//	UART_Debug_PutString(data);

//	uint8_t Port1 = 0;
	uint8_t Port1_W0 = 0;
	uint8_t Port1_W1 = 0;
	//uint8_t Port1_In = 0;

//	uint8_t Port2 = 0;
	uint8_t Port2_W0 = 0;
	uint8_t Port2_W1 = 0;
	//uint8_t Port2_In = 0;

//	Port1 = intr & 0b00000111;
//	Port2 = (intr & 0b11100000) >>  5;

	check_port_1(intr & 0b00000111, &Port1_W0, &Port1_W1);
//    HAL_IWDG_Refresh(&hiwdg);;
//
    check_port_2((intr & 0b11100000) >>  5, &Port2_W0, &Port2_W1);
//    HAL_IWDG_Refresh(&hiwdg);

    //check_port_input_1(Port1_In);
//    HAL_IWDG_Refresh(&hiwdg);

    //check_port_input_2(Port2_In);
//    HAL_IWDG_Refresh(&hiwdg);

    check_port_wiegand_1(Port1_W0, Port1_W1);
//    HAL_IWDG_Refresh(&hiwdg);

    check_port_wiegand_2(Port2_W0, Port2_W1);
//    HAL_IWDG_Refresh(&hiwdg);



    //char s1[100];
    //sprintf(s1, "OS_TIMER_GetStatus(&Timer_AfterRead_Port1) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port1));
    //UART_Debug_PutString(s1);

    //char s2[100];
    //sprintf(s2, "OS_TIMER_GetStatus(&Timer_AfterRead_Port2) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port2));
    //UART_Debug_PutString(s2);

    return;
}

void check_port_1(uint8_t check, uint8_t* p1_0, uint8_t* p1_1)
{
    switch(check){
        case 1:{
            //W1_0
            *p1_0 = 1;
            break;
        }
        case 2:{
            //W1_1
            *p1_1 = 1;
            break;
        }
//        case 3:{
//            //W1_0, W1_1
//        }
        //case 4:{
           // //In1
            //*in1 = 1;
             //break;
        //}
        //case 5:{
            ////W1_0, In1
            //*p1_0 = 1;
            //*in1 = 1;
            //break;
        //}
        //case 6:{
            ////W1_1, In1
            //*p1_0 = 1;
            //*in1 = 1;
             //break;
        //}
//        case 7:{
//            //W1_0, W1_1, In1
//             break;
//        }
        default:{
            break;
        }
    }
    return;
}

void check_port_2(uint8_t check, uint8_t* p2_0, uint8_t* p2_1)
{
    switch(check){
        case 1:{
            //W2_0
            *p2_0 = 1;
            break;
        }
        case 2:{
            //W2_1
            *p2_1 = 1;
            break;
        }
//        case 3:{
//            //W2_0, W2_1
//        }
        //case 4:{
            ////In2
            //*in2 = 1;
             //break;
        //}
        //case 5:{
            ////W2_0, In2
            //*p2_0 = 1;
            //*in2 = 1;
             //break;
        //}
        //case 6:{
            ////W1_1, In1
            //*p2_1 = 1;
            //*in2 = 1;
            //break;
        //}
//        case 7:{
//            //W2_0, W2_1, In2
//             break;
//        }
        default:{
            break;
        }
    }

    return;
}

void check_port_input_1(uint8_t Port1_In)
{
    if(Port1_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In1)){
        In1[0] = 'I';
        In1[1] = '1';
        In1[2] = '\0';


//        read_input_port1++;
//        read_input_total = read_input_port1 + read_input_port2;
//        char data[256];
//        sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
//
//        UART_Debug_PutString("\r\n----------------------------\r\n");
//        UART_Debug_PutString("Read Input from Port 1: ");
//        HAL_IWDG_Refresh(&hiwdg);
//        UART_Debug_PutString(In1);
//        UART_Debug_PutString("\r\n");
//        UART_Debug_PutString(data);
//        HAL_IWDG_Refresh(&hiwdg);

//		HAL_UART_Transmit(&huart1, (const uint8_t*)In1, strlen(In1) + 1, 500);


//        if(!q_put_message(&_Queue_Send, In1, 3)){
//			UART_Debug_PutString("check_port_input_1 >> failed to put to _Queue_Send >> ");
//			UART_Debug_PutString(In1);
//			UART_Debug_PutString("\r\n");
//		}
//		UART_Linux_PutString(In1);
//		UART_Linux_PutChar('\0');

//		UART_Debug_PutString("send I 1 linux: ");
//		UART_Debug_PutString(In1);
//		UART_Debug_PutString("\r\n");

//		UART_Debug_PutString("Read New Input from Port 1 with linux: ");
//		UART_Debug_PutString(In1);
//		HAL_IWDG_Refresh(&hiwdg);
//		UART_Debug_PutString("\r\n");

//		OS_TIMER_Stop(&Timer_AfterRead_In1);
//		In1[0] = '\0';
//		In1[1] = '\0';
//		In1[2] = '\0';
//				UART_Debug_PutString("----------------------------\r\n");
//				HAL_IWDG_Refresh(&hiwdg);

        OS_TIMER_Restart(&Timer_AfterRead_In1);
//        OS_TIMER_Stop(&Timer_AfterRead_In1);
    }

    return;
}

void check_port_input_2(uint8_t Port2_In)
{
    if(Port2_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In2)){
        In2[0] = 'I';
        In2[1] = '2';
        In2[2] = '\0';

//        read_input_port2++;
//        read_input_total = read_input_port1 + read_input_port2;
//        char data[256];
//        sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
//
//        UART_Debug_PutString("\r\n----------------------------\r\n");
//        UART_Debug_PutString("Read Input from Port 2: ");
//        HAL_IWDG_Refresh(&hiwdg);
//        UART_Debug_PutString(In2);
//        UART_Debug_PutString("\r\n");
//        UART_Debug_PutString(data);
//        HAL_IWDG_Refresh(&hiwdg);

//		HAL_UART_Transmit(&huart1, (const uint8_t*)In2, strlen(In2) + 1, 500);

//        if(!q_put_message(&_Queue_Send, In2, 3)){
//        	UART_Debug_PutString("check_port_input_2 >> failed to put to _Queue_Send >> ");
//        	UART_Debug_PutString(In2);
//        	UART_Debug_PutString("\r\n");
//        }
//		UART_Linux_PutString(In2);
//		UART_Linux_PutChar('\0');

//		UART_Debug_PutString("send I 2 linux: ");
////		UART_Debug_PutString(In2);
//		UART_Debug_PutString("\r\n");

//		UART_Debug_PutString("Read New Input from Port 2 with linux: ");
//		UART_Debug_PutString(In2);
//		HAL_IWDG_Refresh(&hiwdg);
//		UART_Debug_PutString("\r\n");

//		OS_TIMER_Stop(&Timer_AfterRead_In2);
		//In2[0] = '\0';
		//In2[1] = '\0';
		//In2[2] = '\0';
//		UART_Debug_PutString("----------------------------\r\n");
//		HAL_IWDG_Refresh(&hiwdg);



      OS_TIMER_Restart(&Timer_AfterRead_In2);
//      OS_TIMER_Stop(&Timer_AfterRead_In2);
    }

    return;
}

void check_port_wiegand_1(uint8_t Port1_W0, uint8_t Port1_W1)
{
    if(!Port1_w_have){
        if((Port1_W0 || Port1_W1) && !Port1_Locked && !OS_TIMER_GetStatus(&Timer_AfterRead_Port1)){
            if(Port1_w_Start){
                OS_TIMER_Restart(&Timer_Read_W_Port1);
                if(Port1_W0){
                    Port1_W[Port1_W_Counter] = '0';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }

                if(Port1_W1){
                    Port1_W[Port1_W_Counter] = '1';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }

                if(Port1_W_Counter >= (Port1_W_bits + 3)){
                    Port1_W[Port1_W_Counter] = '\0';
                    Port1_W_Counter = Port1_W_Counter + 1;

//                    Port1_W_Counter = 0;
//					Port1_w_Start = 0;
					Port1_w_have = 1;

                    OS_TIMER_Stop(&Timer_Read_W_Port1);

//                    UART_Debug_PutString("\r\n----------------------------\r\nRead W Port 1: ");
//                    UART_Debug_PutString(Port1_W);
//                    UART_Debug_PutString("\r\n");

//                    read_counter_port1++;
//                    read_counter_total = read_counter_port1 + read_counter_port2;
//                    char data[256];
//                    sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
//                    UART_Debug_PutString("\r\n----------------------------\r\n");
//                    UART_Debug_PutString("Read W Port 1: ");
//                    HAL_IWDG_Refresh(&hiwdg);
//                    UART_Debug_PutString(Port1_W);
//                    HAL_IWDG_Refresh(&hiwdg);
//                    UART_Debug_PutString("\r\n");
//
//                    UART_Debug_PutString(data);
//                    HAL_IWDG_Refresh(&hiwdg);

	//				HAL_UART_Transmit(&huart1, (const uint8_t*)Port1_W, strlen(Port2_W) + 1, 500);
	//				if(!q_put_message(&_Queue_Send, Port1_W, Port1_W_Counter)){
	//					UART_Debug_PutString("check_port_wiegand_1 >> failed to put to _Queue_Send >> ");
	//					UART_Debug_PutString(Port1_W);
	//					UART_Debug_PutString("\r\n");
	//				}
	//				UART_Linux_PutString(Port1_W);
	//				UART_Linux_PutChar('\0');

	//				UART_Debug_PutString("send W 1 linux: ");
	//				UART_Debug_PutString(Port1_W);
	//				UART_Debug_PutString("\r\n");
	//				linux_read_counter_port1++;
	//				linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
	//				char data[256];
	//				sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);

	//				UART_Debug_PutString("Read New W from Port 1 with linux: ");
	//				UART_Debug_PutString(Port1_W);
	//				HAL_IWDG_Refresh(&hiwdg);
	//				UART_Debug_PutString("\r\n");

	//				UART_Debug_PutString(data);
	//				HAL_IWDG_Refresh(&hiwdg);

	//				OS_TIMER_Stop(&Timer_AfterRead_Port1);
//					Port1_W_Counter = 0;
//					Port1_w_Start = 0;
//					Port1_W[0] = '\0';
//					Port1_w_have = 0;
	//				UART_Debug_PutString("----------------------------\r\n");
	//				HAL_IWDG_Refresh(&hiwdg);

	//				OS_TIMER_Stop(&Timer_AfterRead_Port1);
					OS_TIMER_Restart(&Timer_AfterRead_Port1);
                }

            }
            else{
                Port1_w_Start = 1;
                OS_TIMER_Restart(&Timer_Read_W_Port1);

                Port1_W_Counter = 0;
                Port1_W[0] = 'P';
                Port1_W[1] = '1';
                Port1_W[2] = 'W';
                Port1_W_Counter = 3;

                if(Port1_W0){
                    Port1_W[Port1_W_Counter] = '0';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }

                if(Port1_W1){
                    Port1_W[Port1_W_Counter] = '1';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }
            }
        }
    }

    return;
}

void check_port_wiegand_2(uint8_t Port2_W0, uint8_t Port2_W1)
{
    if(!Port2_w_have){
        if((Port2_W0 || Port2_W1) && !Port2_Locked && !OS_TIMER_GetStatus(&Timer_AfterRead_Port2)){
            if(Port2_w_Start){
                OS_TIMER_Restart(&Timer_Read_W_Port2);
                if(Port2_W0){
                    Port2_W[Port2_W_Counter] = '0';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }

                if(Port2_W1){
                    Port2_W[Port2_W_Counter] = '1';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }

                if(Port2_W_Counter >= (Port2_W_bits + 3)){
                    Port2_W[Port2_W_Counter] = '\0';
                    Port2_W_Counter = Port2_W_Counter + 1;

//                    Port2_W_Counter = 0;
//                    Port2_w_Start = 0;
                    Port2_w_have = 1;

                    OS_TIMER_Stop(&Timer_Read_W_Port2);

//                    UART_Debug_PutString("\r\n----------------------------\r\nRead W Port 2: ");
//                    UART_Debug_PutString(Port2_W);
//                    UART_Debug_PutString("\r\n");

//                    read_counter_port2++;
//                    read_counter_total = read_counter_port1 + read_counter_port2;
//                    char data[256];
//                    sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
//
//                    UART_Debug_PutString("\r\n----------------------------\r\n");
//                    UART_Debug_PutString("Read W Port 2: ");
//                    HAL_IWDG_Refresh(&hiwdg);
//                    UART_Debug_PutString(Port2_W);
//                    HAL_IWDG_Refresh(&hiwdg);
//                    UART_Debug_PutString("\r\n");
//
//                    HAL_IWDG_Refresh(&hiwdg);
//                    UART_Debug_PutString(data);

//				HAL_UART_Transmit(&huart1, (const uint8_t*)Port2_W, strlen(Port2_W) + 1, 500);
//				if(!q_put_message(&_Queue_Send, Port2_W, Port2_W_Counter)){
//					UART_Debug_PutString("check_port_wiegand_2 >> failed to put to _Queue_Send >> ");
//					UART_Debug_PutString(Port2_W);
//					UART_Debug_PutString("\r\n");
//				}
//				UART_Linux_PutString(Port2_W);
//				UART_Linux_PutChar('\0');

//				UART_Debug_PutString("send W 2 linux: ");
//				UART_Debug_PutString(Port2_W);
//				UART_Debug_PutString("\r\n");

//				linux_read_counter_port2++;
//				linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
//				char data[256];
//				sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);

//				UART_Debug_PutString("Read New W from Port 2 with linux: ");
//				UART_Debug_PutString(Port2_W);
//				HAL_IWDG_Refresh(&hiwdg);
//				UART_Debug_PutString("\r\n");

//				UART_Debug_PutString(data);
//				HAL_IWDG_Refresh(&hiwdg);

//				OS_TIMER_Stop(&Timer_AfterRead_Port2);
//				Port2_W_Counter = 0;
//				Port2_w_Start = 0;
//				Port2_W[0] = '\0';
//				Port2_w_have = 0;
//				UART_Debug_PutString("----------------------------\r\n");
//				HAL_IWDG_Refresh(&hiwdg);

//				OS_TIMER_Stop(&Timer_AfterRead_Port2);
                    OS_TIMER_Restart(&Timer_AfterRead_Port2);
                }

            }
            else{
                Port2_w_Start = 1;
                OS_TIMER_Restart(&Timer_Read_W_Port2);

                Port2_W_Counter = 0;
                Port2_W[0] = 'P';
                Port2_W[1] = '2';
                Port2_W[2] = 'W';
                Port2_W_Counter = 3;

                if(Port2_W0){
                    Port2_W[Port2_W_Counter] = '0';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }

                if(Port2_W1){
                    Port2_W[Port2_W_Counter] = '1';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }
            }
        }
    }

    return;
}

void Inputs_Job()
{
    uint8_t inps = Pins_Inputs_Read();
    HAL_IWDG_Refresh(&hiwdg);

    if(inps_old != inps){
    	inps_old = inps;
    	sprintf(Inputs, "{\"inps\":\"%02X\"}", inps);
		if(!q_put_message(&_Queue_Send, Inputs, 14)){
			UART_Debug_PutString("Inputs_Job >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(Inputs);
			UART_Debug_PutString("\r\n");
		}
//    	UART_Linux_PutString(Inputs);
    	HAL_IWDG_Refresh(&hiwdg);
    }


    //    UART_Debug_PutString(Inputs);
    //    UART_Debug_PutString("\r\n");

//    byte_to_binary(inps, str_binary);
//    strcat(Inputs, str_binary);
//    HAL_IWDG_Refresh(&hiwdg);
//    strcat(Inputs, "\"}");
//    HAL_IWDG_Refresh(&hiwdg);

//    UART_Debug_PutString("\r\n----------------------------\r\n");
//    UART_Debug_PutString("read inputs: ");
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString(Inputs);
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString("\r\n----------------------------\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
//
    return;
}

void Inps()
{
    uint8_t inp_1 = HAL_GPIO_ReadPin(GPIOC, GPIO_inp_1_Pin);
    uint8_t inp_2 = HAL_GPIO_ReadPin(GPIOA, GPIO_inp_2_Pin);

    if(inp_1_old != inp_1){
        inp_1_old = inp_1;
        if(!inp_1)
            check_port_input_1(1);
    }

    if(inp_2_old != inp_2){
        inp_2_old = inp_2;
        if(!inp_2)
            check_port_input_2(1);
    }

    return;
}

//Main Job
void CmdTask(char* msg)
{
//	UART_Debug_PutString(msg);
//	UART_Debug_PutString("\r\n");
//	return;
	if(fota_flag_start == 1){
		if((memcmp((char*)&msg[0], "{FOT_FOT}:", 10) == 0)){
//			OS_TIMER_Restart(&Timer_Update);
			update_procedure(&((char*)msg)[10]);
		}
		else{
			UART_Debug_PutString("UPDATE IN PROGRESS >> NOT AN UPDATE PACKET!!!\r\n");
		}
	}
	else{
		//read ports
//		if(memcmp(msg, "Port 1 Read", 11) == 0){
//			if(Port1_W[0] != '\0' && Port1_w_have){
////				HAL_UART_Transmit(&huart1, (const uint8_t*)Port1_W, strlen(Port2_W) + 1, 500);
//				UART_Linux_PutString(Port1_W);
////				UART_Linux_PutChar('\0');
//
//				UART_Debug_PutString("Read W 1 linux: ");
//				UART_Debug_PutString(Port1_W);
//				UART_Debug_PutString("\r\n");
////				linux_read_counter_port1++;
////				linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
////				char data[256];
////				sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);
//
////				UART_Debug_PutString("Read New W from Port 1 with linux: ");
////				UART_Debug_PutString(Port1_W);
////				HAL_IWDG_Refresh(&hiwdg);
////				UART_Debug_PutString("\r\n");
//
////				UART_Debug_PutString(data);
////				HAL_IWDG_Refresh(&hiwdg);
//
////				OS_TIMER_Stop(&Timer_AfterRead_Port1);
//				Port1_W_Counter = 0;
//				Port1_w_Start = 0;
//				Port1_W[0] = '\0';
//				Port1_w_have = 0;
////				UART_Debug_PutString("----------------------------\r\n");
////				HAL_IWDG_Refresh(&hiwdg);
//
//				OS_TIMER_Stop(&Timer_AfterRead_Port1);
//			}
//
//		}
//		else if(memcmp(msg, "Port 2 Read", 11) == 0){
//			if(Port2_W[0] != '\0' && Port2_w_have){
////				HAL_UART_Transmit(&huart1, (const uint8_t*)Port2_W, strlen(Port2_W) + 1, 500);
//				UART_Linux_PutString(Port2_W);
////				UART_Linux_PutChar('\0');
//
//				UART_Debug_PutString("Read W 2 linux: ");
//				UART_Debug_PutString(Port2_W);
//				UART_Debug_PutString("\r\n");
//
////				linux_read_counter_port2++;
////				linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
////				char data[256];
////				sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);
//
////				UART_Debug_PutString("Read New W from Port 2 with linux: ");
////				UART_Debug_PutString(Port2_W);
////				HAL_IWDG_Refresh(&hiwdg);
////				UART_Debug_PutString("\r\n");
//
////				UART_Debug_PutString(data);
////				HAL_IWDG_Refresh(&hiwdg);
//
////				OS_TIMER_Stop(&Timer_AfterRead_Port2);
//				Port2_W_Counter = 0;
//				Port2_w_Start = 0;
//				Port2_W[0] = '\0';
//				Port2_w_have = 0;
////				UART_Debug_PutString("----------------------------\r\n");
////				HAL_IWDG_Refresh(&hiwdg);
//
//				OS_TIMER_Stop(&Timer_AfterRead_Port2);
//			}
//		}
//		//read iputs
//		else if(memcmp(msg, "Port 1 Input", 12) == 0){
//			if(In1[0] != '\0'){
////				HAL_UART_Transmit(&huart1, (const uint8_t*)In1, strlen(In1) + 1, 500);
//				UART_Linux_PutString(In1);
////				UART_Linux_PutChar('\0');
//
//				UART_Debug_PutString("Read I 1 linux: ");
//				UART_Debug_PutString(In1);
//				UART_Debug_PutString("\r\n");
//
////				UART_Debug_PutString("Read New Input from Port 1 with linux: ");
////				UART_Debug_PutString(In1);
////				HAL_IWDG_Refresh(&hiwdg);
////				UART_Debug_PutString("\r\n");
//
////				OS_TIMER_Stop(&Timer_AfterRead_In1);
//				In1[0] = '\0';
//				In1[1] = '\0';
//				In1[2] = '\0';
////				UART_Debug_PutString("----------------------------\r\n");
////				HAL_IWDG_Refresh(&hiwdg);
//
//				OS_TIMER_Stop(&Timer_AfterRead_In1);
//			}
//		}
//		else if(memcmp(msg, "Port 2 Input", 12) == 0){
//			if(In2[0] != '\0'){
////				HAL_UART_Transmit(&huart1, (const uint8_t*)In2, strlen(In2) + 1, 500);
//				UART_Linux_PutString(In2);
////				UART_Linux_PutChar('\0');
//
//				UART_Debug_PutString("Read I 2 linux: ");
//				UART_Debug_PutString(In2);
//				UART_Debug_PutString("\r\n");
//
////				UART_Debug_PutString("Read New Input from Port 2 with linux: ");
////				UART_Debug_PutString(In2);
////				HAL_IWDG_Refresh(&hiwdg);
////				UART_Debug_PutString("\r\n");
//
////				OS_TIMER_Stop(&Timer_AfterRead_In2);
//				In2[0] = '\0';
//				In2[1] = '\0';
//				In2[2] = '\0';
////				UART_Debug_PutString("----------------------------\r\n");
////				HAL_IWDG_Refresh(&hiwdg);
//
//				OS_TIMER_Stop(&Timer_AfterRead_In2);
//			}
//		}
		//relay open close
		/*else*/ if(memcmp(msg, "Port 1 Open", 11) == 0){
			Port1_Relay = 1;
			if(Port1_Relay != Port1_Relay_Old){
				Port1_Relay_Old = Port1_Relay;
				UART_Debug_PutString("Port 1 Open\r\n");
				Relay_Port1(Port1_Relay);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 1 Close", 12) == 0){
			Port1_Relay = 0;
			if(Port1_Relay != Port1_Relay_Old){
				Port1_Relay_Old = Port1_Relay;
				UART_Debug_PutString("Port 1 Close\r\n");
				Relay_Port1(Port1_Relay);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 2 Open", 11) == 0){
			Port2_Relay = 1;
			if(Port2_Relay != Port2_Relay_Old){
				Port2_Relay_Old = Port2_Relay;
				UART_Debug_PutString("Port 2 Open\r\n");
				Relay_Port2(Port2_Relay);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 2 Close", 12) == 0){
			Port2_Relay = 0;
			if(Port2_Relay != Port2_Relay_Old){
				Port2_Relay_Old = Port2_Relay;
				UART_Debug_PutString("Port 2 Close\r\n");
				Relay_Port2(Port2_Relay);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		//port locks
		else if(memcmp(msg, "Port 1 Lock", 11) == 0){
			Port1_Locked = 1;
			if(Port1_Locked != Port1_Locked_Old){
				Port1_Locked_Old = Port1_Locked;
				UART_Debug_PutString("Port 1 Lock\r\n");
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 1 UnLock", 13) == 0){
			Port1_Locked = 0;
			if(Port1_Locked != Port1_Locked_Old){
				Port1_Locked_Old = Port1_Locked;
				UART_Debug_PutString("Port 1 UnLock\r\n");
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 2 Lock", 11) == 0){
			Port2_Locked = 1;
			if(Port2_Locked != Port2_Locked_Old){
				Port2_Locked_Old = Port2_Locked;
				UART_Debug_PutString("Port 2 Lock\r\n");
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 2 UnLock", 13) == 0){
			Port2_Locked = 0;
			if(Port2_Locked != Port2_Locked_Old){
				Port2_Locked_Old = Port2_Locked;
				UART_Debug_PutString("Port 2 UnLock\r\n");
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		//port w config bits
		else if(memcmp(msg, "Port 1 Wbits:", 13) == 0){
			//Port1_W_bits
			char* ret = NULL;
			ret = strremove((char*)msg, "Port 1 Wbits:");
			HAL_IWDG_Refresh(&hiwdg);

			Port1_W_bits = atoi(ret);
			HAL_IWDG_Refresh(&hiwdg);
			if(Port1_W_bits != Port1_W_bits_Old){
				Port1_W_bits_Old = Port1_W_bits;
				char ss[100];
				sprintf(ss, "Port 1 Wbits = %d\r\n", Port1_W_bits);
				UART_Debug_PutString(ss);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		else if(memcmp(msg, "Port 2 Wbits:", 13) == 0){
			//Port2_W_bits
			char* ret = NULL;
			ret = strremove((char*)msg, "Port 2 Wbits:");
			HAL_IWDG_Refresh(&hiwdg);
			Port2_W_bits = atoi(ret);
			HAL_IWDG_Refresh(&hiwdg);
			if(Port2_W_bits != Port2_W_bits_Old){
				Port2_W_bits_Old = Port2_W_bits;
				char ss[100];
				sprintf(ss, "Port 2 Wbits = %d\r\n", Port2_W_bits);
				UART_Debug_PutString(ss);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		//Cy RGB mode
		else if(memcmp(msg, "Cy RGB:", 7) == 0){
			//RGB
			OS_TIMER_Restart(&Timer_RGB_LED);
			char* ret = NULL;
			ret = strremove((char*)msg, "Cy RGB:");
			HAL_IWDG_Refresh(&hiwdg);
			uint32_t color = strtol(ret, NULL, 10);
			HAL_IWDG_Refresh(&hiwdg);
			RGB_Color = (uint8_t)color;
			if(RGB_Color != RGB_Color_Old){
				RGB_Color_Old = RGB_Color;
				if(RGB_Color > 6 || RGB_Color < 0){
					RGB_Color = 1;
					RGB_Color_Old = 1;
				}
				char ss[100];
				sprintf(ss, "RGB_Color = %d\r\n", RGB_Color);
				UART_Debug_PutString(ss);
				RGB(RGB_Color);
				HAL_IWDG_Refresh(&hiwdg);
			}
		}
		//Cy reset
		else if(memcmp(msg, "Cy Reset", 8) == 0){
			HAL_IWDG_Refresh(&hiwdg);
			NVIC_SystemReset();
		}
		else if(memcmp(msg, "Cy Connect", 10) == 0){
//			HAL_UART_Transmit(&huart1, (const uint8_t*)("Cy Connected"), strlen("Cy Connected") + 1, 500);
			UART_Linux_PutString("Cy Connected");
//			UART_Linux_PutChar('\0');
			HAL_IWDG_Refresh(&hiwdg);
		}
//		else if(memcmp(msg, "Inputs", 6) == 0){
//	//    	UART_Debug_PutString(Inputs);
//	//    	UART_Debug_PutString("\r\n");
////			HAL_UART_Transmit(&huart1, (const uint8_t*)(Inputs), strlen(Inputs) + 1, 1000);
//			UART_Linux_PutString(Inputs);
////			UART_Linux_PutChar('\0');
//			HAL_IWDG_Refresh(&hiwdg);
//		}
//		else if(memcmp(msg, "App Version", 11) == 0){
//			char app_version_buff[50];
//			sprintf(app_version_buff,"{\"App Version\":\"");
//			strcat(app_version_buff,APP_VERSION);
//			strcat(app_version_buff, "\"}\0");
////			HAL_UART_Transmit(&huart1, (const uint8_t*)(app_version_buff), strlen(app_version_buff) + 1, 500);
//			UART_Linux_PutString(app_version_buff);
////			UART_Linux_PutChar('\0');
//			HAL_IWDG_Refresh(&hiwdg);
//		}
		else if(memcmp(msg, "{SET_FOT}:", 10) == 0){
	    	UART_Debug_PutString("\r\nUPDATE COMMAND RECEIVED!!!\r\n");
//			OS_TIMER_Restart(&Timer_Update);
	    	RGB_Color = 5;
	    	RGB_Color_Old = 5;
	    	RGB(RGB_Color);
			update_check_packet(msg);
			HAL_IWDG_Refresh(&hiwdg);

		}
		else{
	//        UART_Linux_ClearRxBuffer();
	//        UART_Linux_ClearTxBuffer();
	//    	if(msg[0] == 'P')//UART Problem
	//    		CmdTask(&msg[1]);
	//    	else{
	//    	char dd[64];
	//    	sprintf(dd, "0: %d\r\n", msg[0]);
	//    	UART_Debug_PutString(dd);
//				if(strlen(msg) > 0){
					UART_Debug_PutString("---------- Wrong Msg: ");
					UART_Debug_PutString(msg);
					UART_Debug_PutString("\r\n");
					HAL_IWDG_Refresh(&hiwdg);
//				}
	//    	}
		}
	}

    return;
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

//  OS_TASK_Create(&TCB_Task_Start, "Task_Quectel", 100, Task_Start, Stack_Task_Start, sizeof(Stack_Task_Start), 2);  // Create & start
  //Inits();

    OS_Init();
    OS_InitHW();

    /* Start embOS */
    OS_TASK_Create(&TCB_Task_Main, "Task_Main", 400, Task_Main, Stack_Task_Main, sizeof(Stack_Task_Main), 2);
//    Inits();
    OS_Start();
  /* USER CODE END 2 */

  /* Initialize the embOS kernel and configure the hardware parameters for embOS */
  OS_Init();
  OS_InitHW();

  /* Start embOS */
  OS_Start();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Inits()
{
	UART_Debug_PutString("********************************* Inits(); *************************************************\r\n");

//	__HAL_AFIO_REMAP_SWJ_NOJTAG(); // FYI : fix inp_2 problem
	HAL_IWDG_Refresh(&hiwdg);

	UART_Debug_PutString("********************************* this is 8800  ********************************************\r\n");
	HAL_IWDG_Refresh(&hiwdg);
	UART_Debug_PutString("********************************* Start STM32 Access Slave 57600 *********************************\r\n");
	HAL_IWDG_Refresh(&hiwdg);
	UART_Debug_PutString("********************************* ");
	HAL_IWDG_Refresh(&hiwdg);
	UART_Debug_PutString(APP_VERSION);
	HAL_IWDG_Refresh(&hiwdg);
	UART_Debug_PutString(" *****************************\r\n");
	HAL_IWDG_Refresh(&hiwdg);
	//  HAL_Delay(100);
//	HAL_IWDG_Refresh(&hiwdg);


	OS_Q_Create(&_Queue_Main, &_QBuffer_Queue_Main, sizeof(_QBuffer_Queue_Main));
	OS_Q_Create(&_Queue_Send, &_QBuffer_Queue_Send, sizeof(_QBuffer_Queue_Send));



//	HAL_UART_Receive_IT(&huart1, linux_rx, sizeof(linux_rx));
	HAL_UART_Receive_DMA(&huart1, linux_rx, sizeof(linux_rx));
	HAL_IWDG_Refresh(&hiwdg);

	RGB_Color = 1;
	RGB_Color_Old = 1;
	RGB(RGB_Color); //Cypress run
	HAL_IWDG_Refresh(&hiwdg);

//	UART_Debug_PutString("********************************* Brd V3.0 App V2.0 (and-inps) *********************************\r\n");

	init_vars();
	timers_create();


	HAL_IWDG_Refresh(&hiwdg);
	Timer_Inps_Callback();

	HAL_IWDG_Refresh(&hiwdg);
	Timer_WD_Callback();

	HAL_IWDG_Refresh(&hiwdg);
	Timer_RGB_LED_Callback();

	UART_Debug_PutString("********************************* IEnds(); *************************************************\r\n");
	__enable_irq();
	return;
}

void init_vars()
{
    Port1_W[0] = '\0';
    In1[0] = '\0';
    HAL_IWDG_Refresh(&hiwdg);

    Port2_W[0] = '\0';
    In2[0] = '\0';
    HAL_IWDG_Refresh(&hiwdg);

	memset(intr, '\0', sizeof(intr));
	HAL_IWDG_Refresh(&hiwdg);

    memset(_QBuffer_Queue_Main, '\0', Queue_Size_Main);
    HAL_IWDG_Refresh(&hiwdg);

    memset(_QBuffer_Queue_Send, '\0', Queue_Size_Send);
	HAL_IWDG_Refresh(&hiwdg);

    return;
}

void timers_create()
{
    // Create without start.
    OS_TIMER_Create(&Timer_Read_Linux, Timer_Read_Linux_Callback, 10);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_RGB_LED, Timer_RGB_LED_Callback, 1000);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_Read_W_Port1, Timer_Read_W_Port1_Callback, 100);
    HAL_IWDG_Refresh(&hiwdg);
    OS_TIMER_Create(&Timer_Read_W_Port2, Timer_Read_W_Port2_Callback, 100);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_AfterRead_In1, Timer_AfterRead_Input1_Callback, 10);
    HAL_IWDG_Refresh(&hiwdg);
    OS_TIMER_Create(&Timer_AfterRead_Port1, Timer_AfterRead_Port1_Callback, 10);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_AfterRead_In2, Timer_AfterRead_Input2_Callback, 10);
    HAL_IWDG_Refresh(&hiwdg);
    OS_TIMER_Create(&Timer_AfterRead_Port2, Timer_AfterRead_Port2_Callback, 10);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_WD, Timer_WD_Callback, 100);
    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_Inps, Timer_Inps_Callback, 50);
    HAL_IWDG_Refresh(&hiwdg);

//    OS_TIMER_Create(&Timer_RP1, Timer_Inps_Callback, 1000);
//    HAL_IWDG_Refresh(&hiwdg);

//    OS_TIMER_Create(&Timer_RP2, Timer_Inps_Callback, 1000);
//    HAL_IWDG_Refresh(&hiwdg);


//    OS_TIMER_CREATE(&Timer_Update, Timer_Update_Callback, 1000);
//    OS_TIMER_Stop(&Timer_Update);
//    HAL_IWDG_Refresh(&hiwdg);

    OS_TIMER_Create(&Timer_intr, Timer_Intr_Callback, 200);
	HAL_IWDG_Refresh(&hiwdg);

    return;
}

void Task_Main()
{
	Inits();
	while(1){
		Proc_Task_main();
		HAL_IWDG_Refresh(&hiwdg);
		OS_TASK_Delay(TASK_DELAY_Main_MS);
	}

	return;
}

void Proc_Task_main()
{

	void* ret_send = NULL;
	ret_send = q_get_message(&_Queue_Send);
	while(ret_send){
		HAL_IWDG_Refresh(&hiwdg);
		UART_Linux_PutString((char*)ret_send);
		if(memcmp((char*)ret_send, "{FOT_REQ}:", 10) != 0){
			UART_Debug_PutString("Proc_Task_main -> send: ");
			UART_Debug_PutString((char*)ret_send);
			UART_Debug_PutString("\r\n");
		}
		OS_QUEUE_Purge(&_Queue_Send);

		OS_TASK_Delay(TASK_DELAY_Main_MS);
		ret_send = NULL;
		ret_send = q_get_message(&_Queue_Send);
	}

	void* ret = NULL;
	ret = q_get_message(&_Queue_Main);
	while(ret){
		HAL_IWDG_Refresh(&hiwdg);
		CmdTask((char*)ret);
		OS_QUEUE_Purge(&_Queue_Main);
		OS_TASK_Delay(TASK_DELAY_Main_MS);
		ret = NULL;
		ret = q_get_message(&_Queue_Main);
	}
	return;
}

//Callbacks
void Timer_Read_Linux_Callback()
{
//	OS_TIMER_Stop(&Timer_Read_Linux);
//	linux_uart_failure++;
//	char data[128];
//	sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
//	UART_Debug_PutString("****************************\r\n");
//	UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
//	UART_Debug_PutString(data);
//	UART_Debug_PutString(UART_Linux_Rx_Buffer);
//	UART_Debug_PutString("\r\n");
//	UART_Debug_PutString("****************************\r\n");

	if(UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter - 1] == '\0'){
		if(!q_put_message(&_Queue_Main, UART_Linux_Rx_Buffer, UART_Linux_Rx_Counter)){
			UART_Debug_PutString("HAL_UART_RxCpltCallback >> failed to put to _Queue_Main >> ");
			UART_Debug_PutString(UART_Linux_Rx_Buffer);
			UART_Debug_PutString("\r\n");
		}
	}
	else{
		linux_uart_failure++;
		char data[128];
		sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
		UART_Debug_PutString("****************************\r\n");
		UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
		UART_Debug_PutString(data);
		UART_Debug_PutString(UART_Linux_Rx_Buffer);
		UART_Debug_PutString("\r\n");
		UART_Debug_PutString("****************************\r\n");
	}

	memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
	UART_Linux_Rx_Counter = 0;


    return;
}

void Timer_RGB_LED_Callback()
{
    OS_TIMER_Stop(&Timer_RGB_LED);
    if(!fota_flag_start){
		RGB_Color = 1;
		if(RGB_Color != RGB_Color_Old){
			RGB_Color_Old = RGB_Color;
			RGB(RGB_Color);

			char ss[256];
			sprintf(ss, "Timer_RGB_LED_Callback --> RGB_Color = %d\r\n", RGB_Color);
			HAL_IWDG_Refresh(&hiwdg);
			UART_Debug_PutString(ss);
			HAL_IWDG_Refresh(&hiwdg);
		}
    }

    OS_TIMER_Restart(&Timer_RGB_LED);

    return;
}

void Timer_Read_W_Port1_Callback()
{
    OS_TIMER_Stop(&Timer_Read_W_Port1);

    read_failure_port1++;
    read_failure_total = read_failure_port1 + read_failure_port2;
    char data[256];
    sprintf(data, "read_failure_port1 = %lu, read_failure_port2 = %lu, read_failure_total = %lu\r\n", read_failure_port1, read_failure_port2, read_failure_total);
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_Read_W_Port1_Callback:");
    UART_Debug_PutString(Port1_W);
    UART_Debug_PutString("\r\n");
    HAL_IWDG_Refresh(&hiwdg);
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    HAL_IWDG_Refresh(&hiwdg);

    Port1_W_Counter = 0;
    Port1_w_Start = 0;
    Port1_W[0] = '\0';
    Port1_w_have = 0;

    return;
}

void Timer_Read_W_Port2_Callback()
{
    OS_TIMER_Stop(&Timer_Read_W_Port2);

    read_failure_port2++;
    read_failure_total = read_failure_port1 + read_failure_port2;
    char data[256];
    sprintf(data, "read_failure_port1 = %lu, read_failure_port2 = %lu, read_failure_total = %lu\r\n", read_failure_port1, read_failure_port2, read_failure_total);
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_Read_W_Port2_Callback:");
    UART_Debug_PutString(Port2_W);
    UART_Debug_PutString("\r\n");
    HAL_IWDG_Refresh(&hiwdg);
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    HAL_IWDG_Refresh(&hiwdg);

    Port2_W_Counter = 0;
    Port2_w_Start = 0;
    Port2_W[0] = '\0';
    Port2_w_have = 0;

    return;
}

void Timer_AfterRead_Input1_Callback()
{
//    OS_TIMER_Stop(&Timer_AfterRead_In1);
//
//    linux_read_input_failure_port1++;
//    linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
//    char data[256];
//    sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
//
//    UART_Debug_PutString("****************************\r\n");
//    UART_Debug_PutString("Timer_AfterRead_Input1_Callback\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString(data);
//    UART_Debug_PutString("****************************\r\n");
//    HAL_IWDG_Refresh(&hiwdg);

	if(!q_put_message(&_Queue_Send, In1, 3)){
		UART_Debug_PutString("Timer_AfterRead_Input1_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(In1);
		UART_Debug_PutString("\r\n");
	}

    In1[0] = '\0';
    In1[1] = '\0';
    In1[2] = '\0';

    return;
}

void Timer_AfterRead_Port1_Callback()
{
//    OS_TIMER_Stop(&Timer_AfterRead_Port1);

//    linux_read_failure_port1++;
//    linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
//    char data[256];
//    sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
//
//    UART_Debug_PutString("****************************\r\n");
//    UART_Debug_PutString("Timer_AfterRead_Port1_Callback\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString(data);
//    UART_Debug_PutString("****************************\r\n");
//    HAL_IWDG_Refresh(&hiwdg);

	if(!q_put_message(&_Queue_Send, Port1_W, Port1_W_Counter)){
		UART_Debug_PutString("Timer_AfterRead_Port1_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(Port1_W);
		UART_Debug_PutString("\r\n");
	}

    Port1_W_Counter = 0;
    Port1_w_Start = 0;
    Port1_W[0] = '\0';
    Port1_w_have = 0;

    return;
}

void Timer_AfterRead_Input2_Callback()
{
//    OS_TIMER_Stop(&Timer_AfterRead_In2);
//
//    linux_read_input_failure_port2++;
//    linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
//    char data[256];
//    sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
//
//    UART_Debug_PutString("****************************\r\n");
//    UART_Debug_PutString("Timer_AfterRead_Input2_Callback\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString(data);
//    UART_Debug_PutString("****************************\r\n");
//    HAL_IWDG_Refresh(&hiwdg);

	if(!q_put_message(&_Queue_Send, In2, 3)){
		UART_Debug_PutString("Timer_AfterRead_Input2_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(In2);
		UART_Debug_PutString("\r\n");
	}

    In2[0] = '\0';
    In2[1] = '\0';
    In2[2] = '\0';

    return;
}

void Timer_AfterRead_Port2_Callback()
{
//    OS_TIMER_Stop(&Timer_AfterRead_Port2);
//
//    linux_read_failure_port2++;
//    linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
//    char data[256];
//    sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
//
//    UART_Debug_PutString("****************************\r\n");
//    UART_Debug_PutString("Timer_AfterRead_Port2_Callback\r\n");
//    HAL_IWDG_Refresh(&hiwdg);
//    UART_Debug_PutString(data);
//    UART_Debug_PutString("****************************\r\n");
//    HAL_IWDG_Refresh(&hiwdg);

	if(!q_put_message(&_Queue_Send, Port2_W, Port2_W_Counter)){
		UART_Debug_PutString("Timer_AfterRead_Port2_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(Port2_W);
		UART_Debug_PutString("\r\n");
	}

    Port2_W_Counter = 0;
    Port2_w_Start = 0;
    Port2_W[0] = '\0';
    Port2_w_have = 0;

    return;
}

void Timer_WD_Callback()
{
    OS_TIMER_Stop(&Timer_WD);
    HAL_IWDG_Refresh(&hiwdg);
    OS_TIMER_Restart(&Timer_WD);

    return;
}

void Timer_Inps_Callback()
{
    OS_TIMER_Stop(&Timer_Inps);
    Inputs_Job();
    Inps();
//    UART_Debug_PutString("inps\r\n");
    OS_TIMER_Restart(&Timer_Inps);
    return;
}

//void Timer_Update_Callback()
//{
//    OS_TIMER_Stop(&Timer_Update);
//    UART_Debug_PutString("UPDATE FAILED >> Reset\r\n");
////    UART_Linux_PutString("UPDATE FAILED >> Reset\r\n");
//
////    NVIC_SystemReset();
//    return;
//}

void Timer_Intr_Callback()
{
	for(uint8_t i = 0; i < intr_counter; i++)
		Ports_Job(intr[i]);
	intr_counter = 0;
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	__disable_irq();
	OS_INT_Enter();


	if(huart->Instance == USART1){
		OS_TIMER_Restart(&Timer_Read_Linux);
		UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter] = huart1.Instance->DR;
		UART_Linux_Rx_Counter++;
//		if(UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter - 1] == '\0'){
//			OS_TIMER_Stop(&Timer_Read_Linux);
////			CmdTask(UART_Linux_Rx_Buffer);
//			if(!q_put_message(&_Queue_Get, UART_Linux_Rx_Buffer, UART_Linux_Rx_Counter)){
//				uart_debug_print("HAL_UART_RxCpltCallback >> failed to put to _Queue_Main >> ");
//				uart_debug_print(UART_Linux_Rx_Buffer);
//				uart_debug_print("\r\n");
//			}
//			memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
//			UART_Linux_Rx_Counter = 0;
//		}

		HAL_UART_Receive_DMA(&huart1, linux_rx, sizeof(linux_rx));
	}

    OS_INT_Leave();
//    __enable_irq();

	return;
}

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
////	__disable_irq();
//	OS_INT_Enter();
//
//
//	if(huart->Instance == USART1){
//		UART_Linux_Rx_Counter = Size;
//
//		if(UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter - 1] == '\0'){
//			UART_Debug_PutString("true: ");
//			UART_Debug_PutString(UART_Linux_Rx_Buffer);
//			UART_Debug_PutString("\r\n");
//			CmdTask(UART_Linux_Rx_Buffer);
//		}
//		else{
//			UART_Debug_PutString("false: ");
//			UART_Debug_PutString(UART_Linux_Rx_Buffer);
//			UART_Debug_PutString("\r\n");
//			Timer_Read_Linux_Callback();
//		}
//		memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
//		UART_Linux_Rx_Counter = 0;
//		UART_Linux_StartBit = 0;
//
//		if(UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter - 1] == '\0'){
//			CmdTask(UART_Linux_Rx_Buffer);
//		}
//		else{
//			linux_uart_failure++;
//			char data[128];
//			sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
//			UART_Debug_PutString("****************************\r\n");
//			UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
//			UART_Debug_PutString(data);
//			UART_Debug_PutString(UART_Linux_Rx_Buffer);
//			UART_Debug_PutString("\r\n");
//			UART_Debug_PutString("****************************\r\n");
//		}
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)UART_Linux_Rx_Buffer, sizeof(UART_Linux_Rx_Buffer));
//		HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t*)UART_Linux_Rx_Buffer, sizeof(UART_Linux_Rx_Buffer));
//	}
//
//
//    OS_INT_Leave();
////    __enable_irq();
//
//	return;
//}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
//	__disable_irq();
	OS_INT_Enter();

	OS_TIMER_Restart(&Timer_intr);
	intr[intr_counter] = (!HAL_GPIO_ReadPin(GPIOC, GPIO_Input_1_Pin)  << 0) | (!HAL_GPIO_ReadPin(GPIOC, GPIO_Input_2_Pin)  << 1) |
			             (!HAL_GPIO_ReadPin(GPIOA, GPIO_Input_4_Pin)  << 5) | (!HAL_GPIO_ReadPin(GPIOA, GPIO_Input_5_Pin)  << 6) ;
	intr_counter++;
//	Ports_Job(GPIO_Pin);



	OS_INT_Leave();
//	__enable_irq();
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
