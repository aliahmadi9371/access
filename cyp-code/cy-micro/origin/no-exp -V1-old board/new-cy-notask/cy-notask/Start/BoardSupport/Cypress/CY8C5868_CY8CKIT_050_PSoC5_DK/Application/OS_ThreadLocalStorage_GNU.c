/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2022 SEGGER Microcontroller GmbH                  *
*                                                                    *
*       Internet: segger.com  Support: support_embos@segger.com      *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system                           *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: V5.16.1.0                                        *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------
File    : OS_ThreadLocalStorage_GNU.c
Purpose : embOS sample application to demonstrate the usage of TLS.
          TLS support is CPU and compiler specific and may not be
          implemented in all embOS ports.
Note    : Usage of thread local storage in a task requires a very
          large stack, because the _reent structure takes around
          1150 bytes!
*/

#include "RTOS.h"
#include <errno.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static OS_STACKPTR int StackHP[512], StackMP[512], StackLP[256];  // Task stacks
static OS_TASK         TCBHP, TCBMP, TCBLP;                       // Task control blocks

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       HPTask() with thread local storage
*/
static void HPTask(void) {
  struct _reent TaskReentStruct;

  //
  // Initialize TLS for this task
  //
  OS_TASK_SetContextExtensionTLS(&TaskReentStruct);
  while (errno != 0) {
    //
    // errno is local to this task, hence we should not arrive here
    //
  }
  //
  // Simulate a task specific error
  //
  errno = 3;
  while (1) {
    OS_TASK_Delay(10);
      while (errno != 3) {
        //
        // errno is local to this task, hence we should not arrive here
        //
      }
  }
}

/*********************************************************************
*
*       MPTask() with thread local storage
*/
static void MPTask(void) {
  struct _reent TaskReentStruct;

  //
  // Initialize TLS for this task
  //
  OS_TASK_SetContextExtensionTLS(&TaskReentStruct);
  while (errno != 0) {
    //
    // errno is local to this task, hence we should not arrive here
    //
  }
  //
  // Simulate a task specific error
  //
  errno = 2;
  while (1) {
    OS_TASK_Delay(10);
    while (errno != 2) {
        //
        // errno is local to this task, hence we should not arrive here
        //
    }
  }
}

/*********************************************************************
*
*       LPTask() without thread local storage
*/
static void LPTask(void) {
  while (errno != 1) {
    //
    // errno is not local to this task, hence we expect the global
    // value that was set in main() and should not arrive here
    //
  }
  while (1) {
    OS_TASK_Delay(50);
  }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       main()
*/
int main(void) {
  errno = 1;    // Simulate an error
  OS_Init();    // Initialize embOS
  OS_InitHW();  // Initialize required hardware
  OS_TASK_CREATE(&TCBHP, "HP Task", 100, HPTask, StackHP);
  OS_TASK_CREATE(&TCBMP, "MP Task",  70, MPTask, StackMP);
  OS_TASK_CREATE(&TCBLP, "LP Task",  50, LPTask, StackLP);
  OS_Start();   // Start embOS
  return 0;
}

/*************************** End of file ****************************/
