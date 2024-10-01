/*******************************************************************************
* File Name: intr_Timer_Pin_Port2_W2.h
* Version 1.71
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_intr_Timer_Pin_Port2_W2_H)
#define CY_ISR_intr_Timer_Pin_Port2_W2_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void intr_Timer_Pin_Port2_W2_Start(void);
void intr_Timer_Pin_Port2_W2_StartEx(cyisraddress address);
void intr_Timer_Pin_Port2_W2_Stop(void);

CY_ISR_PROTO(intr_Timer_Pin_Port2_W2_Interrupt);

void intr_Timer_Pin_Port2_W2_SetVector(cyisraddress address);
cyisraddress intr_Timer_Pin_Port2_W2_GetVector(void);

void intr_Timer_Pin_Port2_W2_SetPriority(uint8 priority);
uint8 intr_Timer_Pin_Port2_W2_GetPriority(void);

void intr_Timer_Pin_Port2_W2_Enable(void);
uint8 intr_Timer_Pin_Port2_W2_GetState(void);
void intr_Timer_Pin_Port2_W2_Disable(void);

void intr_Timer_Pin_Port2_W2_SetPending(void);
void intr_Timer_Pin_Port2_W2_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the intr_Timer_Pin_Port2_W2 ISR. */
#define intr_Timer_Pin_Port2_W2_INTC_VECTOR            ((reg32 *) intr_Timer_Pin_Port2_W2__INTC_VECT)

/* Address of the intr_Timer_Pin_Port2_W2 ISR priority. */
#define intr_Timer_Pin_Port2_W2_INTC_PRIOR             ((reg8 *) intr_Timer_Pin_Port2_W2__INTC_PRIOR_REG)

/* Priority of the intr_Timer_Pin_Port2_W2 interrupt. */
#define intr_Timer_Pin_Port2_W2_INTC_PRIOR_NUMBER      intr_Timer_Pin_Port2_W2__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable intr_Timer_Pin_Port2_W2 interrupt. */
#define intr_Timer_Pin_Port2_W2_INTC_SET_EN            ((reg32 *) intr_Timer_Pin_Port2_W2__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the intr_Timer_Pin_Port2_W2 interrupt. */
#define intr_Timer_Pin_Port2_W2_INTC_CLR_EN            ((reg32 *) intr_Timer_Pin_Port2_W2__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the intr_Timer_Pin_Port2_W2 interrupt state to pending. */
#define intr_Timer_Pin_Port2_W2_INTC_SET_PD            ((reg32 *) intr_Timer_Pin_Port2_W2__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the intr_Timer_Pin_Port2_W2 interrupt. */
#define intr_Timer_Pin_Port2_W2_INTC_CLR_PD            ((reg32 *) intr_Timer_Pin_Port2_W2__INTC_CLR_PD_REG)


#endif /* CY_ISR_intr_Timer_Pin_Port2_W2_H */


/* [] END OF FILE */
