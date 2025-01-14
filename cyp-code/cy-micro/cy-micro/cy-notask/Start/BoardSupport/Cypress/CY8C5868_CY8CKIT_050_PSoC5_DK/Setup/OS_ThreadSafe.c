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
File    : OS_ThreadSafe.c
Purpose : Thread safe library functions

Additional information:
  This module enables thread and/or interrupt safety for e.g. malloc().
  Per default it ensures thread and interrupt safety by disabling/restoring
  embOS interrupts. Zero latency interrupts are not affected and protected.
  If you need to call e.g. malloc() also from within a zero latency interrupt
  additional handling needs to be added.
  If you don't call such functions from within embOS interrupts you can use
  thread safety instead. This reduces the interrupt latency because a mutex
  is used instead of disabling embOS interrupts.
*/

#include "RTOS.h"

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
#ifdef __cplusplus
  extern "C" {
#endif

void __malloc_lock  (struct _reent *_r) OS_TEXT_SECTION_ATTRIBUTE(__malloc_lock);
void __malloc_unlock(struct _reent *_r) OS_TEXT_SECTION_ATTRIBUTE(__malloc_unlock);

#ifdef __cplusplus
  }
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// When set to 1 thread and interrupt safety is guaranteed.
//
// When set to 0 only thread safety is guaranteed. In this case you
// must not call e.g. heap functions from ISRs, software timers or OS_Idle().
//
#ifndef   OS_INTERRUPT_SAFE
  #define OS_INTERRUPT_SAFE  1
#endif

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       __malloc_lock()
*/
void __malloc_lock (struct _reent *_r) {
  OS_USE_PARA(_r);
#if (OS_INTERRUPT_SAFE == 1)
  OS_InterruptSafetyLock();
#else
  OS_HeapLock();
#endif
}

/*********************************************************************
*
*       __malloc_unlock()
*/
void __malloc_unlock(struct _reent *_r) {
  OS_USE_PARA(_r);
#if (OS_INTERRUPT_SAFE == 1)
  OS_InterruptSafetyUnlock();
#else
  OS_HeapUnlock();
#endif
}

/*************************** End of file ****************************/
