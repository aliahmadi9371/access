/*******************************************************************************
* File Name: RS232_Debug_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "RS232_Debug.h"


/***************************************
* Local data allocation
***************************************/

static RS232_Debug_BACKUP_STRUCT  RS232_Debug_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: RS232_Debug_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the RS232_Debug_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RS232_Debug_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RS232_Debug_SaveConfig(void)
{
    #if(RS232_Debug_CONTROL_REG_REMOVED == 0u)
        RS232_Debug_backup.cr = RS232_Debug_CONTROL_REG;
    #endif /* End RS232_Debug_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RS232_Debug_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RS232_Debug_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling RS232_Debug_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void RS232_Debug_RestoreConfig(void)
{
    #if(RS232_Debug_CONTROL_REG_REMOVED == 0u)
        RS232_Debug_CONTROL_REG = RS232_Debug_backup.cr;
    #endif /* End RS232_Debug_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RS232_Debug_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The RS232_Debug_Sleep() API saves the current component state. Then it
*  calls the RS232_Debug_Stop() function and calls 
*  RS232_Debug_SaveConfig() to save the hardware configuration.
*  Call the RS232_Debug_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RS232_Debug_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RS232_Debug_Sleep(void)
{
    #if(RS232_Debug_RX_ENABLED || RS232_Debug_HD_ENABLED)
        if((RS232_Debug_RXSTATUS_ACTL_REG  & RS232_Debug_INT_ENABLE) != 0u)
        {
            RS232_Debug_backup.enableState = 1u;
        }
        else
        {
            RS232_Debug_backup.enableState = 0u;
        }
    #else
        if((RS232_Debug_TXSTATUS_ACTL_REG  & RS232_Debug_INT_ENABLE) !=0u)
        {
            RS232_Debug_backup.enableState = 1u;
        }
        else
        {
            RS232_Debug_backup.enableState = 0u;
        }
    #endif /* End RS232_Debug_RX_ENABLED || RS232_Debug_HD_ENABLED*/

    RS232_Debug_Stop();
    RS232_Debug_SaveConfig();
}


/*******************************************************************************
* Function Name: RS232_Debug_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  RS232_Debug_Sleep() was called. The RS232_Debug_Wakeup() function
*  calls the RS232_Debug_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  RS232_Debug_Sleep() function was called, the RS232_Debug_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RS232_Debug_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RS232_Debug_Wakeup(void)
{
    RS232_Debug_RestoreConfig();
    #if( (RS232_Debug_RX_ENABLED) || (RS232_Debug_HD_ENABLED) )
        RS232_Debug_ClearRxBuffer();
    #endif /* End (RS232_Debug_RX_ENABLED) || (RS232_Debug_HD_ENABLED) */
    #if(RS232_Debug_TX_ENABLED || RS232_Debug_HD_ENABLED)
        RS232_Debug_ClearTxBuffer();
    #endif /* End RS232_Debug_TX_ENABLED || RS232_Debug_HD_ENABLED */

    if(RS232_Debug_backup.enableState != 0u)
    {
        RS232_Debug_Enable();
    }
}


/* [] END OF FILE */
