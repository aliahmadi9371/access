/*******************************************************************************
* File Name: Timer_Pin_Port2_W2_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_Pin_Port2_W2.h"

static Timer_Pin_Port2_W2_backupStruct Timer_Pin_Port2_W2_backup;


/*******************************************************************************
* Function Name: Timer_Pin_Port2_W2_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Pin_Port2_W2_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Pin_Port2_W2_SaveConfig(void) 
{
    #if (!Timer_Pin_Port2_W2_UsingFixedFunction)
        Timer_Pin_Port2_W2_backup.TimerUdb = Timer_Pin_Port2_W2_ReadCounter();
        Timer_Pin_Port2_W2_backup.InterruptMaskValue = Timer_Pin_Port2_W2_STATUS_MASK;
        #if (Timer_Pin_Port2_W2_UsingHWCaptureCounter)
            Timer_Pin_Port2_W2_backup.TimerCaptureCounter = Timer_Pin_Port2_W2_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Pin_Port2_W2_UDB_CONTROL_REG_REMOVED)
            Timer_Pin_Port2_W2_backup.TimerControlRegister = Timer_Pin_Port2_W2_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Pin_Port2_W2_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Pin_Port2_W2_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port2_W2_RestoreConfig(void) 
{   
    #if (!Timer_Pin_Port2_W2_UsingFixedFunction)

        Timer_Pin_Port2_W2_WriteCounter(Timer_Pin_Port2_W2_backup.TimerUdb);
        Timer_Pin_Port2_W2_STATUS_MASK =Timer_Pin_Port2_W2_backup.InterruptMaskValue;
        #if (Timer_Pin_Port2_W2_UsingHWCaptureCounter)
            Timer_Pin_Port2_W2_SetCaptureCount(Timer_Pin_Port2_W2_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Pin_Port2_W2_UDB_CONTROL_REG_REMOVED)
            Timer_Pin_Port2_W2_WriteControlRegister(Timer_Pin_Port2_W2_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Pin_Port2_W2_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Pin_Port2_W2_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port2_W2_Sleep(void) 
{
    #if(!Timer_Pin_Port2_W2_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Pin_Port2_W2_CTRL_ENABLE == (Timer_Pin_Port2_W2_CONTROL & Timer_Pin_Port2_W2_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Pin_Port2_W2_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Pin_Port2_W2_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Pin_Port2_W2_Stop();
    Timer_Pin_Port2_W2_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Pin_Port2_W2_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Pin_Port2_W2_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port2_W2_Wakeup(void) 
{
    Timer_Pin_Port2_W2_RestoreConfig();
    #if(!Timer_Pin_Port2_W2_UDB_CONTROL_REG_REMOVED)
        if(Timer_Pin_Port2_W2_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Pin_Port2_W2_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
