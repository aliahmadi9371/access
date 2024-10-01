/*******************************************************************************
* File Name: Timer_Read_Input2_PM.c
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

#include "Timer_Read_Input2.h"

static Timer_Read_Input2_backupStruct Timer_Read_Input2_backup;


/*******************************************************************************
* Function Name: Timer_Read_Input2_SaveConfig
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
*  Timer_Read_Input2_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Read_Input2_SaveConfig(void) 
{
    #if (!Timer_Read_Input2_UsingFixedFunction)
        Timer_Read_Input2_backup.TimerUdb = Timer_Read_Input2_ReadCounter();
        Timer_Read_Input2_backup.InterruptMaskValue = Timer_Read_Input2_STATUS_MASK;
        #if (Timer_Read_Input2_UsingHWCaptureCounter)
            Timer_Read_Input2_backup.TimerCaptureCounter = Timer_Read_Input2_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Read_Input2_UDB_CONTROL_REG_REMOVED)
            Timer_Read_Input2_backup.TimerControlRegister = Timer_Read_Input2_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Read_Input2_RestoreConfig
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
*  Timer_Read_Input2_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Read_Input2_RestoreConfig(void) 
{   
    #if (!Timer_Read_Input2_UsingFixedFunction)

        Timer_Read_Input2_WriteCounter(Timer_Read_Input2_backup.TimerUdb);
        Timer_Read_Input2_STATUS_MASK =Timer_Read_Input2_backup.InterruptMaskValue;
        #if (Timer_Read_Input2_UsingHWCaptureCounter)
            Timer_Read_Input2_SetCaptureCount(Timer_Read_Input2_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Read_Input2_UDB_CONTROL_REG_REMOVED)
            Timer_Read_Input2_WriteControlRegister(Timer_Read_Input2_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Read_Input2_Sleep
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
*  Timer_Read_Input2_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Read_Input2_Sleep(void) 
{
    #if(!Timer_Read_Input2_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Read_Input2_CTRL_ENABLE == (Timer_Read_Input2_CONTROL & Timer_Read_Input2_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Read_Input2_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Read_Input2_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Read_Input2_Stop();
    Timer_Read_Input2_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Read_Input2_Wakeup
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
*  Timer_Read_Input2_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Read_Input2_Wakeup(void) 
{
    Timer_Read_Input2_RestoreConfig();
    #if(!Timer_Read_Input2_UDB_CONTROL_REG_REMOVED)
        if(Timer_Read_Input2_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Read_Input2_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
