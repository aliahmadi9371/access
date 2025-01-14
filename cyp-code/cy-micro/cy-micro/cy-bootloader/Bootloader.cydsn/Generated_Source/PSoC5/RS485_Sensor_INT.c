/*******************************************************************************
* File Name: RS485_SensorINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "RS485_Sensor.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (RS485_Sensor_RX_INTERRUPT_ENABLED && (RS485_Sensor_RX_ENABLED || RS485_Sensor_HD_ENABLED))
    /*******************************************************************************
    * Function Name: RS485_Sensor_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RS485_Sensor_rxBuffer - RAM buffer pointer for save received data.
    *  RS485_Sensor_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  RS485_Sensor_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  RS485_Sensor_rxBufferOverflow - software overflow flag. Set to one
    *     when RS485_Sensor_rxBufferWrite index overtakes
    *     RS485_Sensor_rxBufferRead index.
    *  RS485_Sensor_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when RS485_Sensor_rxBufferWrite is equal to
    *    RS485_Sensor_rxBufferRead
    *  RS485_Sensor_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  RS485_Sensor_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(RS485_Sensor_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef RS485_Sensor_RXISR_ENTRY_CALLBACK
        RS485_Sensor_RXISR_EntryCallback();
    #endif /* RS485_Sensor_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START RS485_Sensor_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = RS485_Sensor_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in RS485_Sensor_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (RS485_Sensor_RX_STS_BREAK | 
                            RS485_Sensor_RX_STS_PAR_ERROR |
                            RS485_Sensor_RX_STS_STOP_ERROR | 
                            RS485_Sensor_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                RS485_Sensor_errorStatus |= readStatus & ( RS485_Sensor_RX_STS_BREAK | 
                                                            RS485_Sensor_RX_STS_PAR_ERROR | 
                                                            RS485_Sensor_RX_STS_STOP_ERROR | 
                                                            RS485_Sensor_RX_STS_OVERRUN);
                /* `#START RS485_Sensor_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef RS485_Sensor_RXISR_ERROR_CALLBACK
                RS485_Sensor_RXISR_ERROR_Callback();
            #endif /* RS485_Sensor_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & RS485_Sensor_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = RS485_Sensor_RXDATA_REG;
            #if (RS485_Sensor_RXHW_ADDRESS_ENABLED)
                if(RS485_Sensor_rxAddressMode == (uint8)RS485_Sensor__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & RS485_Sensor_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & RS485_Sensor_RX_STS_ADDR_MATCH) != 0u)
                        {
                            RS485_Sensor_rxAddressDetected = 1u;
                        }
                        else
                        {
                            RS485_Sensor_rxAddressDetected = 0u;
                        }
                    }
                    if(RS485_Sensor_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        RS485_Sensor_rxBuffer[RS485_Sensor_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    RS485_Sensor_rxBuffer[RS485_Sensor_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                RS485_Sensor_rxBuffer[RS485_Sensor_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (RS485_Sensor_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(RS485_Sensor_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        RS485_Sensor_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    RS485_Sensor_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(RS485_Sensor_rxBufferWrite >= RS485_Sensor_RX_BUFFER_SIZE)
                    {
                        RS485_Sensor_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(RS485_Sensor_rxBufferWrite == RS485_Sensor_rxBufferRead)
                    {
                        RS485_Sensor_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (RS485_Sensor_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            RS485_Sensor_RXSTATUS_MASK_REG  &= (uint8)~RS485_Sensor_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(RS485_Sensor_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (RS485_Sensor_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & RS485_Sensor_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START RS485_Sensor_RXISR_END` */

        /* `#END` */

    #ifdef RS485_Sensor_RXISR_EXIT_CALLBACK
        RS485_Sensor_RXISR_ExitCallback();
    #endif /* RS485_Sensor_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (RS485_Sensor_RX_INTERRUPT_ENABLED && (RS485_Sensor_RX_ENABLED || RS485_Sensor_HD_ENABLED)) */


#if (RS485_Sensor_TX_INTERRUPT_ENABLED && RS485_Sensor_TX_ENABLED)
    /*******************************************************************************
    * Function Name: RS485_Sensor_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RS485_Sensor_txBuffer - RAM buffer pointer for transmit data from.
    *  RS485_Sensor_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  RS485_Sensor_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(RS485_Sensor_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef RS485_Sensor_TXISR_ENTRY_CALLBACK
        RS485_Sensor_TXISR_EntryCallback();
    #endif /* RS485_Sensor_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START RS485_Sensor_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((RS485_Sensor_txBufferRead != RS485_Sensor_txBufferWrite) &&
             ((RS485_Sensor_TXSTATUS_REG & RS485_Sensor_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(RS485_Sensor_txBufferRead >= RS485_Sensor_TX_BUFFER_SIZE)
            {
                RS485_Sensor_txBufferRead = 0u;
            }

            RS485_Sensor_TXDATA_REG = RS485_Sensor_txBuffer[RS485_Sensor_txBufferRead];

            /* Set next pointer */
            RS485_Sensor_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START RS485_Sensor_TXISR_END` */

        /* `#END` */

    #ifdef RS485_Sensor_TXISR_EXIT_CALLBACK
        RS485_Sensor_TXISR_ExitCallback();
    #endif /* RS485_Sensor_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (RS485_Sensor_TX_INTERRUPT_ENABLED && RS485_Sensor_TX_ENABLED) */


/* [] END OF FILE */
