/*******************************************************************************
* File Name: Timer_Pin_Port1_W1_PM.c
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

#include "Timer_Pin_Port1_W1.h"

static Timer_Pin_Port1_W1_backupStruct Timer_Pin_Port1_W1_backup;


/*******************************************************************************
* Function Name: Timer_Pin_Port1_W1_SaveConfig
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
*  Timer_Pin_Port1_W1_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Pin_Port1_W1_SaveConfig(void) 
{
    #if (!Timer_Pin_Port1_W1_UsingFixedFunction)
        Timer_Pin_Port1_W1_backup.TimerUdb = Timer_Pin_Port1_W1_ReadCounter();
        Timer_Pin_Port1_W1_backup.InterruptMaskValue = Timer_Pin_Port1_W1_STATUS_MASK;
        #if (Timer_Pin_Port1_W1_UsingHWCaptureCounter)
            Timer_Pin_Port1_W1_backup.TimerCaptureCounter = Timer_Pin_Port1_W1_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Pin_Port1_W1_UDB_CONTROL_REG_REMOVED)
            Timer_Pin_Port1_W1_backup.TimerControlRegister = Timer_Pin_Port1_W1_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Pin_Port1_W1_RestoreConfig
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
*  Timer_Pin_Port1_W1_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port1_W1_RestoreConfig(void) 
{   
    #if (!Timer_Pin_Port1_W1_UsingFixedFunction)

        Timer_Pin_Port1_W1_WriteCounter(Timer_Pin_Port1_W1_backup.TimerUdb);
        Timer_Pin_Port1_W1_STATUS_MASK =Timer_Pin_Port1_W1_backup.InterruptMaskValue;
        #if (Timer_Pin_Port1_W1_UsingHWCaptureCounter)
            Timer_Pin_Port1_W1_SetCaptureCount(Timer_Pin_Port1_W1_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Pin_Port1_W1_UDB_CONTROL_REG_REMOVED)
            Timer_Pin_Port1_W1_WriteControlRegister(Timer_Pin_Port1_W1_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Pin_Port1_W1_Sleep
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
*  Timer_Pin_Port1_W1_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port1_W1_Sleep(void) 
{
    #if(!Timer_Pin_Port1_W1_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Pin_Port1_W1_CTRL_ENABLE == (Timer_Pin_Port1_W1_CONTROL & Timer_Pin_Port1_W1_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Pin_Port1_W1_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Pin_Port1_W1_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Pin_Port1_W1_Stop();
    Timer_Pin_Port1_W1_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Pin_Port1_W1_Wakeup
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
*  Timer_Pin_Port1_W1_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Pin_Port1_W1_Wakeup(void) 
{
    Timer_Pin_Port1_W1_RestoreConfig();
    #if(!Timer_Pin_Port1_W1_UDB_CONTROL_REG_REMOVED)
        if(Timer_Pin_Port1_W1_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Pin_Port1_W1_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
