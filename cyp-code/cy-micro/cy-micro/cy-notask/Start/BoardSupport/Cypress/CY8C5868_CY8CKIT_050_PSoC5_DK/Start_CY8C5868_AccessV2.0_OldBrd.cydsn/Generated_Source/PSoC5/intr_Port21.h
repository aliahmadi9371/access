/*******************************************************************************
* File Name: intr_Port21.h
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
#if !defined(CY_ISR_intr_Port21_H)
#define CY_ISR_intr_Port21_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void intr_Port21_Start(void);
void intr_Port21_StartEx(cyisraddress address);
void intr_Port21_Stop(void);

CY_ISR_PROTO(intr_Port21_Interrupt);

void intr_Port21_SetVector(cyisraddress address);
cyisraddress intr_Port21_GetVector(void);

void intr_Port21_SetPriority(uint8 priority);
uint8 intr_Port21_GetPriority(void);

void intr_Port21_Enable(void);
uint8 intr_Port21_GetState(void);
void intr_Port21_Disable(void);

void intr_Port21_SetPending(void);
void intr_Port21_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the intr_Port21 ISR. */
#define intr_Port21_INTC_VECTOR            ((reg32 *) intr_Port21__INTC_VECT)

/* Address of the intr_Port21 ISR priority. */
#define intr_Port21_INTC_PRIOR             ((reg8 *) intr_Port21__INTC_PRIOR_REG)

/* Priority of the intr_Port21 interrupt. */
#define intr_Port21_INTC_PRIOR_NUMBER      intr_Port21__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable intr_Port21 interrupt. */
#define intr_Port21_INTC_SET_EN            ((reg32 *) intr_Port21__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the intr_Port21 interrupt. */
#define intr_Port21_INTC_CLR_EN            ((reg32 *) intr_Port21__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the intr_Port21 interrupt state to pending. */
#define intr_Port21_INTC_SET_PD            ((reg32 *) intr_Port21__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the intr_Port21 interrupt. */
#define intr_Port21_INTC_CLR_PD            ((reg32 *) intr_Port21__INTC_CLR_PD_REG)


#endif /* CY_ISR_intr_Port21_H */


/* [] END OF FILE */
