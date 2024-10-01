/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RTOS.h"

#include "defines.h"
#include "IO.h"
#include "chip_update.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
///* USER CODE END Declarations_Task0 */
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
extern OS_TIMER Timer_intr;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
///* USER CODE BEGIN Declarations_Task0 */
//extern  OS_TASK         TCB_Task_Start;                     // Control block for Task
//extern  OS_STACKPTR int Stack_Task_Start[1024];            // Stack for Task
//void Task_Start();
///* USER CODE END Declarations_Task0 */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

//Init
void Inits();
//void init_vars();
//void timers_create();

void UART_Debug_PutString(const char* msg);
void UART_Linux_PutString(const char* msg);
void UART_Linux_PutChar(char c);

//Job of each Port
void Ports_Job(uint8_t intr);
void Inputs_Job();
void Inps();

extern uint8_t inp_1_old;
extern uint8_t inp_2_old;

//Main Job
void CmdTask(char* msg);


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
extern void Timer_Intr_Callback();

//Init Functions
void intrrupt_init();
void init_vars();
void timers_create();

void check_port_1(uint8_t Port1, uint8_t* p1_0, uint8_t* p1_1);
void check_port_2(uint8_t Port2, uint8_t* p2_0, uint8_t* p2_1);

void check_port_input_1(uint8_t Port1_In);
void check_port_input_2(uint8_t Port2_In);

void check_port_wiegand_1(uint8_t Port1_W0, uint8_t Port1_W1);
void check_port_wiegand_2(uint8_t Port2_W0, uint8_t Port2_W1);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_Input_1_Pin GPIO_PIN_0
#define GPIO_Input_1_GPIO_Port GPIOC
#define GPIO_Input_1_EXTI_IRQn EXTI0_IRQn
#define GPIO_Input_2_Pin GPIO_PIN_1
#define GPIO_Input_2_GPIO_Port GPIOC
#define GPIO_Input_2_EXTI_IRQn EXTI1_IRQn
#define GPIO_inp_1_Pin GPIO_PIN_2
#define GPIO_inp_1_GPIO_Port GPIOC
#define GPIO_Output_Relay_1_Pin GPIO_PIN_3
#define GPIO_Output_Relay_1_GPIO_Port GPIOC
#define GPIO_Input_4_Pin GPIO_PIN_5
#define GPIO_Input_4_GPIO_Port GPIOA
#define GPIO_Input_4_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_Input_5_Pin GPIO_PIN_6
#define GPIO_Input_5_GPIO_Port GPIOA
#define GPIO_Input_5_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_inp_2_Pin GPIO_PIN_7
#define GPIO_inp_2_GPIO_Port GPIOA
#define GPIO_Output_Relay_2_Pin GPIO_PIN_5
#define GPIO_Output_Relay_2_GPIO_Port GPIOC
#define GPIO_Input_7_Pin GPIO_PIN_12
#define GPIO_Input_7_GPIO_Port GPIOB
#define GPIO_Input_8_Pin GPIO_PIN_13
#define GPIO_Input_8_GPIO_Port GPIOB
#define GPIO_Input_9_Pin GPIO_PIN_14
#define GPIO_Input_9_GPIO_Port GPIOB
#define GPIO_Input_10_Pin GPIO_PIN_15
#define GPIO_Input_10_GPIO_Port GPIOB
#define GPIO_Input_11_Pin GPIO_PIN_9
#define GPIO_Input_11_GPIO_Port GPIOC
#define GPIO_Input_12_Pin GPIO_PIN_8
#define GPIO_Input_12_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
