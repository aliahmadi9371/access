/*******************************************************************************
* File Name: RS485_Sensor.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_RS485_Sensor_H)
#define CY_UART_RS485_Sensor_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define RS485_Sensor_RX_ENABLED                     (1u)
#define RS485_Sensor_TX_ENABLED                     (1u)
#define RS485_Sensor_HD_ENABLED                     (0u)
#define RS485_Sensor_RX_INTERRUPT_ENABLED           (1u)
#define RS485_Sensor_TX_INTERRUPT_ENABLED           (1u)
#define RS485_Sensor_INTERNAL_CLOCK_USED            (1u)
#define RS485_Sensor_RXHW_ADDRESS_ENABLED           (0u)
#define RS485_Sensor_OVER_SAMPLE_COUNT              (8u)
#define RS485_Sensor_PARITY_TYPE                    (0u)
#define RS485_Sensor_PARITY_TYPE_SW                 (0u)
#define RS485_Sensor_BREAK_DETECT                   (0u)
#define RS485_Sensor_BREAK_BITS_TX                  (13u)
#define RS485_Sensor_BREAK_BITS_RX                  (13u)
#define RS485_Sensor_TXCLKGEN_DP                    (1u)
#define RS485_Sensor_USE23POLLING                   (1u)
#define RS485_Sensor_FLOW_CONTROL                   (0u)
#define RS485_Sensor_CLK_FREQ                       (0u)
#define RS485_Sensor_TX_BUFFER_SIZE                 (2048u)
#define RS485_Sensor_RX_BUFFER_SIZE                 (256u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define RS485_Sensor_CONTROL_REG_REMOVED            (0u)
#else
    #define RS485_Sensor_CONTROL_REG_REMOVED            (1u)
#endif /* End RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct RS485_Sensor_backupStruct_
{
    uint8 enableState;

    #if(RS485_Sensor_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End RS485_Sensor_CONTROL_REG_REMOVED */

} RS485_Sensor_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void RS485_Sensor_Start(void) ;
void RS485_Sensor_Stop(void) ;
uint8 RS485_Sensor_ReadControlRegister(void) ;
void RS485_Sensor_WriteControlRegister(uint8 control) ;

void RS485_Sensor_Init(void) ;
void RS485_Sensor_Enable(void) ;
void RS485_Sensor_SaveConfig(void) ;
void RS485_Sensor_RestoreConfig(void) ;
void RS485_Sensor_Sleep(void) ;
void RS485_Sensor_Wakeup(void) ;

/* Only if RX is enabled */
#if( (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) )

    #if (RS485_Sensor_RX_INTERRUPT_ENABLED)
        #define RS485_Sensor_EnableRxInt()  CyIntEnable (RS485_Sensor_RX_VECT_NUM)
        #define RS485_Sensor_DisableRxInt() CyIntDisable(RS485_Sensor_RX_VECT_NUM)
        CY_ISR_PROTO(RS485_Sensor_RXISR);
    #endif /* RS485_Sensor_RX_INTERRUPT_ENABLED */

    void RS485_Sensor_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void RS485_Sensor_SetRxAddress1(uint8 address) ;
    void RS485_Sensor_SetRxAddress2(uint8 address) ;

    void  RS485_Sensor_SetRxInterruptMode(uint8 intSrc) ;
    uint8 RS485_Sensor_ReadRxData(void) ;
    uint8 RS485_Sensor_ReadRxStatus(void) ;
    uint8 RS485_Sensor_GetChar(void) ;
    uint16 RS485_Sensor_GetByte(void) ;
    uint16 RS485_Sensor_GetRxBufferSize(void)
                                                            ;
    void RS485_Sensor_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define RS485_Sensor_GetRxInterruptSource   RS485_Sensor_ReadRxStatus

#endif /* End (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) */

/* Only if TX is enabled */
#if(RS485_Sensor_TX_ENABLED || RS485_Sensor_HD_ENABLED)

    #if(RS485_Sensor_TX_INTERRUPT_ENABLED)
        #define RS485_Sensor_EnableTxInt()  CyIntEnable (RS485_Sensor_TX_VECT_NUM)
        #define RS485_Sensor_DisableTxInt() CyIntDisable(RS485_Sensor_TX_VECT_NUM)
        #define RS485_Sensor_SetPendingTxInt() CyIntSetPending(RS485_Sensor_TX_VECT_NUM)
        #define RS485_Sensor_ClearPendingTxInt() CyIntClearPending(RS485_Sensor_TX_VECT_NUM)
        CY_ISR_PROTO(RS485_Sensor_TXISR);
    #endif /* RS485_Sensor_TX_INTERRUPT_ENABLED */

    void RS485_Sensor_SetTxInterruptMode(uint8 intSrc) ;
    void RS485_Sensor_WriteTxData(uint8 txDataByte) ;
    uint8 RS485_Sensor_ReadTxStatus(void) ;
    void RS485_Sensor_PutChar(uint8 txDataByte) ;
    void RS485_Sensor_PutString(const char8 string[]) ;
    void RS485_Sensor_PutArray(const uint8 string[], uint16 byteCount)
                                                            ;
    void RS485_Sensor_PutCRLF(uint8 txDataByte) ;
    void RS485_Sensor_ClearTxBuffer(void) ;
    void RS485_Sensor_SetTxAddressMode(uint8 addressMode) ;
    void RS485_Sensor_SendBreak(uint8 retMode) ;
    uint16 RS485_Sensor_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define RS485_Sensor_PutStringConst         RS485_Sensor_PutString
    #define RS485_Sensor_PutArrayConst          RS485_Sensor_PutArray
    #define RS485_Sensor_GetTxInterruptSource   RS485_Sensor_ReadTxStatus

#endif /* End RS485_Sensor_TX_ENABLED || RS485_Sensor_HD_ENABLED */

#if(RS485_Sensor_HD_ENABLED)
    void RS485_Sensor_LoadRxConfig(void) ;
    void RS485_Sensor_LoadTxConfig(void) ;
#endif /* End RS485_Sensor_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RS485_Sensor) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    RS485_Sensor_CyBtldrCommStart(void) CYSMALL ;
    void    RS485_Sensor_CyBtldrCommStop(void) CYSMALL ;
    void    RS485_Sensor_CyBtldrCommReset(void) CYSMALL ;
    cystatus RS485_Sensor_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus RS485_Sensor_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RS485_Sensor)
        #define CyBtldrCommStart    RS485_Sensor_CyBtldrCommStart
        #define CyBtldrCommStop     RS485_Sensor_CyBtldrCommStop
        #define CyBtldrCommReset    RS485_Sensor_CyBtldrCommReset
        #define CyBtldrCommWrite    RS485_Sensor_CyBtldrCommWrite
        #define CyBtldrCommRead     RS485_Sensor_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_RS485_Sensor) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define RS485_Sensor_BYTE2BYTE_TIME_OUT (25u)
    #define RS485_Sensor_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define RS485_Sensor_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define RS485_Sensor_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define RS485_Sensor_SET_SPACE      (0x00u)
#define RS485_Sensor_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (RS485_Sensor_TX_ENABLED) || (RS485_Sensor_HD_ENABLED) )
    #if(RS485_Sensor_TX_INTERRUPT_ENABLED)
        #define RS485_Sensor_TX_VECT_NUM            (uint8)RS485_Sensor_TXInternalInterrupt__INTC_NUMBER
        #define RS485_Sensor_TX_PRIOR_NUM           (uint8)RS485_Sensor_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* RS485_Sensor_TX_INTERRUPT_ENABLED */

    #define RS485_Sensor_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define RS485_Sensor_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define RS485_Sensor_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(RS485_Sensor_TX_ENABLED)
        #define RS485_Sensor_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (RS485_Sensor_HD_ENABLED) */
        #define RS485_Sensor_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (RS485_Sensor_TX_ENABLED) */

    #define RS485_Sensor_TX_STS_COMPLETE            (uint8)(0x01u << RS485_Sensor_TX_STS_COMPLETE_SHIFT)
    #define RS485_Sensor_TX_STS_FIFO_EMPTY          (uint8)(0x01u << RS485_Sensor_TX_STS_FIFO_EMPTY_SHIFT)
    #define RS485_Sensor_TX_STS_FIFO_FULL           (uint8)(0x01u << RS485_Sensor_TX_STS_FIFO_FULL_SHIFT)
    #define RS485_Sensor_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << RS485_Sensor_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (RS485_Sensor_TX_ENABLED) || (RS485_Sensor_HD_ENABLED)*/

#if( (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) )
    #if(RS485_Sensor_RX_INTERRUPT_ENABLED)
        #define RS485_Sensor_RX_VECT_NUM            (uint8)RS485_Sensor_RXInternalInterrupt__INTC_NUMBER
        #define RS485_Sensor_RX_PRIOR_NUM           (uint8)RS485_Sensor_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* RS485_Sensor_RX_INTERRUPT_ENABLED */
    #define RS485_Sensor_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define RS485_Sensor_RX_STS_BREAK_SHIFT             (0x01u)
    #define RS485_Sensor_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define RS485_Sensor_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define RS485_Sensor_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define RS485_Sensor_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define RS485_Sensor_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define RS485_Sensor_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define RS485_Sensor_RX_STS_MRKSPC           (uint8)(0x01u << RS485_Sensor_RX_STS_MRKSPC_SHIFT)
    #define RS485_Sensor_RX_STS_BREAK            (uint8)(0x01u << RS485_Sensor_RX_STS_BREAK_SHIFT)
    #define RS485_Sensor_RX_STS_PAR_ERROR        (uint8)(0x01u << RS485_Sensor_RX_STS_PAR_ERROR_SHIFT)
    #define RS485_Sensor_RX_STS_STOP_ERROR       (uint8)(0x01u << RS485_Sensor_RX_STS_STOP_ERROR_SHIFT)
    #define RS485_Sensor_RX_STS_OVERRUN          (uint8)(0x01u << RS485_Sensor_RX_STS_OVERRUN_SHIFT)
    #define RS485_Sensor_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << RS485_Sensor_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define RS485_Sensor_RX_STS_ADDR_MATCH       (uint8)(0x01u << RS485_Sensor_RX_STS_ADDR_MATCH_SHIFT)
    #define RS485_Sensor_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << RS485_Sensor_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define RS485_Sensor_RX_HW_MASK                     (0x7Fu)
#endif /* End (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) */

/* Control Register definitions */
#define RS485_Sensor_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define RS485_Sensor_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define RS485_Sensor_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define RS485_Sensor_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define RS485_Sensor_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define RS485_Sensor_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define RS485_Sensor_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define RS485_Sensor_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define RS485_Sensor_CTRL_HD_SEND               (uint8)(0x01u << RS485_Sensor_CTRL_HD_SEND_SHIFT)
#define RS485_Sensor_CTRL_HD_SEND_BREAK         (uint8)(0x01u << RS485_Sensor_CTRL_HD_SEND_BREAK_SHIFT)
#define RS485_Sensor_CTRL_MARK                  (uint8)(0x01u << RS485_Sensor_CTRL_MARK_SHIFT)
#define RS485_Sensor_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << RS485_Sensor_CTRL_PARITY_TYPE0_SHIFT)
#define RS485_Sensor_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << RS485_Sensor_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define RS485_Sensor_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define RS485_Sensor_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define RS485_Sensor_SEND_BREAK                         (0x00u)
#define RS485_Sensor_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define RS485_Sensor_REINIT                             (0x02u)
#define RS485_Sensor_SEND_WAIT_REINIT                   (0x03u)

#define RS485_Sensor_OVER_SAMPLE_8                      (8u)
#define RS485_Sensor_OVER_SAMPLE_16                     (16u)

#define RS485_Sensor_BIT_CENTER                         (RS485_Sensor_OVER_SAMPLE_COUNT - 2u)

#define RS485_Sensor_FIFO_LENGTH                        (4u)
#define RS485_Sensor_NUMBER_OF_START_BIT                (1u)
#define RS485_Sensor_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define RS485_Sensor_TXBITCTR_BREAKBITS8X   ((RS485_Sensor_BREAK_BITS_TX * RS485_Sensor_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define RS485_Sensor_TXBITCTR_BREAKBITS ((RS485_Sensor_BREAK_BITS_TX * RS485_Sensor_OVER_SAMPLE_COUNT) - 1u)

#define RS485_Sensor_HALF_BIT_COUNT   \
                            (((RS485_Sensor_OVER_SAMPLE_COUNT / 2u) + (RS485_Sensor_USE23POLLING * 1u)) - 2u)
#if (RS485_Sensor_OVER_SAMPLE_COUNT == RS485_Sensor_OVER_SAMPLE_8)
    #define RS485_Sensor_HD_TXBITCTR_INIT   (((RS485_Sensor_BREAK_BITS_TX + \
                            RS485_Sensor_NUMBER_OF_START_BIT) * RS485_Sensor_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define RS485_Sensor_RXBITCTR_INIT  ((((RS485_Sensor_BREAK_BITS_RX + RS485_Sensor_NUMBER_OF_START_BIT) \
                            * RS485_Sensor_OVER_SAMPLE_COUNT) + RS485_Sensor_HALF_BIT_COUNT) - 1u)

#else /* RS485_Sensor_OVER_SAMPLE_COUNT == RS485_Sensor_OVER_SAMPLE_16 */
    #define RS485_Sensor_HD_TXBITCTR_INIT   ((8u * RS485_Sensor_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define RS485_Sensor_RXBITCTR_INIT      (((7u * RS485_Sensor_OVER_SAMPLE_COUNT) - 1u) + \
                                                      RS485_Sensor_HALF_BIT_COUNT)
#endif /* End RS485_Sensor_OVER_SAMPLE_COUNT */

#define RS485_Sensor_HD_RXBITCTR_INIT                   RS485_Sensor_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 RS485_Sensor_initVar;
#if (RS485_Sensor_TX_INTERRUPT_ENABLED && RS485_Sensor_TX_ENABLED)
    extern volatile uint8 RS485_Sensor_txBuffer[RS485_Sensor_TX_BUFFER_SIZE];
    extern volatile uint16 RS485_Sensor_txBufferRead;
    extern uint16 RS485_Sensor_txBufferWrite;
#endif /* (RS485_Sensor_TX_INTERRUPT_ENABLED && RS485_Sensor_TX_ENABLED) */
#if (RS485_Sensor_RX_INTERRUPT_ENABLED && (RS485_Sensor_RX_ENABLED || RS485_Sensor_HD_ENABLED))
    extern uint8 RS485_Sensor_errorStatus;
    extern volatile uint8 RS485_Sensor_rxBuffer[RS485_Sensor_RX_BUFFER_SIZE];
    extern volatile uint16 RS485_Sensor_rxBufferRead;
    extern volatile uint16 RS485_Sensor_rxBufferWrite;
    extern volatile uint8 RS485_Sensor_rxBufferLoopDetect;
    extern volatile uint8 RS485_Sensor_rxBufferOverflow;
    #if (RS485_Sensor_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 RS485_Sensor_rxAddressMode;
        extern volatile uint8 RS485_Sensor_rxAddressDetected;
    #endif /* (RS485_Sensor_RXHW_ADDRESS_ENABLED) */
#endif /* (RS485_Sensor_RX_INTERRUPT_ENABLED && (RS485_Sensor_RX_ENABLED || RS485_Sensor_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define RS485_Sensor__B_UART__AM_SW_BYTE_BYTE 1
#define RS485_Sensor__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define RS485_Sensor__B_UART__AM_HW_BYTE_BY_BYTE 3
#define RS485_Sensor__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define RS485_Sensor__B_UART__AM_NONE 0

#define RS485_Sensor__B_UART__NONE_REVB 0
#define RS485_Sensor__B_UART__EVEN_REVB 1
#define RS485_Sensor__B_UART__ODD_REVB 2
#define RS485_Sensor__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define RS485_Sensor_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define RS485_Sensor_NUMBER_OF_STOP_BITS    (1u)

#if (RS485_Sensor_RXHW_ADDRESS_ENABLED)
    #define RS485_Sensor_RX_ADDRESS_MODE    (0u)
    #define RS485_Sensor_RX_HW_ADDRESS1     (0u)
    #define RS485_Sensor_RX_HW_ADDRESS2     (0u)
#endif /* (RS485_Sensor_RXHW_ADDRESS_ENABLED) */

#define RS485_Sensor_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << RS485_Sensor_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_BREAK_SHIFT) \
                                        | (0 << RS485_Sensor_RX_STS_OVERRUN_SHIFT))

#define RS485_Sensor_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << RS485_Sensor_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << RS485_Sensor_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << RS485_Sensor_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << RS485_Sensor_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define RS485_Sensor_CONTROL_REG \
                            (* (reg8 *) RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define RS485_Sensor_CONTROL_PTR \
                            (  (reg8 *) RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(RS485_Sensor_TX_ENABLED)
    #define RS485_Sensor_TXDATA_REG          (* (reg8 *) RS485_Sensor_BUART_sTX_TxShifter_u0__F0_REG)
    #define RS485_Sensor_TXDATA_PTR          (  (reg8 *) RS485_Sensor_BUART_sTX_TxShifter_u0__F0_REG)
    #define RS485_Sensor_TXDATA_AUX_CTL_REG  (* (reg8 *) RS485_Sensor_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define RS485_Sensor_TXDATA_AUX_CTL_PTR  (  (reg8 *) RS485_Sensor_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define RS485_Sensor_TXSTATUS_REG        (* (reg8 *) RS485_Sensor_BUART_sTX_TxSts__STATUS_REG)
    #define RS485_Sensor_TXSTATUS_PTR        (  (reg8 *) RS485_Sensor_BUART_sTX_TxSts__STATUS_REG)
    #define RS485_Sensor_TXSTATUS_MASK_REG   (* (reg8 *) RS485_Sensor_BUART_sTX_TxSts__MASK_REG)
    #define RS485_Sensor_TXSTATUS_MASK_PTR   (  (reg8 *) RS485_Sensor_BUART_sTX_TxSts__MASK_REG)
    #define RS485_Sensor_TXSTATUS_ACTL_REG   (* (reg8 *) RS485_Sensor_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define RS485_Sensor_TXSTATUS_ACTL_PTR   (  (reg8 *) RS485_Sensor_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(RS485_Sensor_TXCLKGEN_DP)
        #define RS485_Sensor_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define RS485_Sensor_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define RS485_Sensor_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define RS485_Sensor_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define RS485_Sensor_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define RS485_Sensor_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define RS485_Sensor_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define RS485_Sensor_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define RS485_Sensor_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define RS485_Sensor_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) RS485_Sensor_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* RS485_Sensor_TXCLKGEN_DP */

#endif /* End RS485_Sensor_TX_ENABLED */

#if(RS485_Sensor_HD_ENABLED)

    #define RS485_Sensor_TXDATA_REG             (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__F1_REG )
    #define RS485_Sensor_TXDATA_PTR             (  (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__F1_REG )
    #define RS485_Sensor_TXDATA_AUX_CTL_REG     (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define RS485_Sensor_TXDATA_AUX_CTL_PTR     (  (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define RS485_Sensor_TXSTATUS_REG           (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_REG )
    #define RS485_Sensor_TXSTATUS_PTR           (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_REG )
    #define RS485_Sensor_TXSTATUS_MASK_REG      (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__MASK_REG )
    #define RS485_Sensor_TXSTATUS_MASK_PTR      (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__MASK_REG )
    #define RS485_Sensor_TXSTATUS_ACTL_REG      (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define RS485_Sensor_TXSTATUS_ACTL_PTR      (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End RS485_Sensor_HD_ENABLED */

#if( (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) )
    #define RS485_Sensor_RXDATA_REG             (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__F0_REG )
    #define RS485_Sensor_RXDATA_PTR             (  (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__F0_REG )
    #define RS485_Sensor_RXADDRESS1_REG         (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__D0_REG )
    #define RS485_Sensor_RXADDRESS1_PTR         (  (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__D0_REG )
    #define RS485_Sensor_RXADDRESS2_REG         (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__D1_REG )
    #define RS485_Sensor_RXADDRESS2_PTR         (  (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__D1_REG )
    #define RS485_Sensor_RXDATA_AUX_CTL_REG     (* (reg8 *) RS485_Sensor_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define RS485_Sensor_RXBITCTR_PERIOD_REG    (* (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define RS485_Sensor_RXBITCTR_PERIOD_PTR    (  (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define RS485_Sensor_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define RS485_Sensor_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define RS485_Sensor_RXBITCTR_COUNTER_REG   (* (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__COUNT_REG )
    #define RS485_Sensor_RXBITCTR_COUNTER_PTR   (  (reg8 *) RS485_Sensor_BUART_sRX_RxBitCounter__COUNT_REG )

    #define RS485_Sensor_RXSTATUS_REG           (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_REG )
    #define RS485_Sensor_RXSTATUS_PTR           (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_REG )
    #define RS485_Sensor_RXSTATUS_MASK_REG      (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__MASK_REG )
    #define RS485_Sensor_RXSTATUS_MASK_PTR      (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__MASK_REG )
    #define RS485_Sensor_RXSTATUS_ACTL_REG      (* (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define RS485_Sensor_RXSTATUS_ACTL_PTR      (  (reg8 *) RS485_Sensor_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) */

#if(RS485_Sensor_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define RS485_Sensor_INTCLOCK_CLKEN_REG     (* (reg8 *) RS485_Sensor_IntClock__PM_ACT_CFG)
    #define RS485_Sensor_INTCLOCK_CLKEN_PTR     (  (reg8 *) RS485_Sensor_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define RS485_Sensor_INTCLOCK_CLKEN_MASK    RS485_Sensor_IntClock__PM_ACT_MSK
#endif /* End RS485_Sensor_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(RS485_Sensor_TX_ENABLED)
    #define RS485_Sensor_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End RS485_Sensor_TX_ENABLED */

#if(RS485_Sensor_HD_ENABLED)
    #define RS485_Sensor_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End RS485_Sensor_HD_ENABLED */

#if( (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) )
    #define RS485_Sensor_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define RS485_Sensor_WAIT_1_MS      RS485_Sensor_BL_CHK_DELAY_MS   

#define RS485_Sensor_TXBUFFERSIZE   RS485_Sensor_TX_BUFFER_SIZE
#define RS485_Sensor_RXBUFFERSIZE   RS485_Sensor_RX_BUFFER_SIZE

#if (RS485_Sensor_RXHW_ADDRESS_ENABLED)
    #define RS485_Sensor_RXADDRESSMODE  RS485_Sensor_RX_ADDRESS_MODE
    #define RS485_Sensor_RXHWADDRESS1   RS485_Sensor_RX_HW_ADDRESS1
    #define RS485_Sensor_RXHWADDRESS2   RS485_Sensor_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define RS485_Sensor_RXAddressMode  RS485_Sensor_RXADDRESSMODE
#endif /* (RS485_Sensor_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define RS485_Sensor_initvar                    RS485_Sensor_initVar

#define RS485_Sensor_RX_Enabled                 RS485_Sensor_RX_ENABLED
#define RS485_Sensor_TX_Enabled                 RS485_Sensor_TX_ENABLED
#define RS485_Sensor_HD_Enabled                 RS485_Sensor_HD_ENABLED
#define RS485_Sensor_RX_IntInterruptEnabled     RS485_Sensor_RX_INTERRUPT_ENABLED
#define RS485_Sensor_TX_IntInterruptEnabled     RS485_Sensor_TX_INTERRUPT_ENABLED
#define RS485_Sensor_InternalClockUsed          RS485_Sensor_INTERNAL_CLOCK_USED
#define RS485_Sensor_RXHW_Address_Enabled       RS485_Sensor_RXHW_ADDRESS_ENABLED
#define RS485_Sensor_OverSampleCount            RS485_Sensor_OVER_SAMPLE_COUNT
#define RS485_Sensor_ParityType                 RS485_Sensor_PARITY_TYPE

#if( RS485_Sensor_TX_ENABLED && (RS485_Sensor_TXBUFFERSIZE > RS485_Sensor_FIFO_LENGTH))
    #define RS485_Sensor_TXBUFFER               RS485_Sensor_txBuffer
    #define RS485_Sensor_TXBUFFERREAD           RS485_Sensor_txBufferRead
    #define RS485_Sensor_TXBUFFERWRITE          RS485_Sensor_txBufferWrite
#endif /* End RS485_Sensor_TX_ENABLED */
#if( ( RS485_Sensor_RX_ENABLED || RS485_Sensor_HD_ENABLED ) && \
     (RS485_Sensor_RXBUFFERSIZE > RS485_Sensor_FIFO_LENGTH) )
    #define RS485_Sensor_RXBUFFER               RS485_Sensor_rxBuffer
    #define RS485_Sensor_RXBUFFERREAD           RS485_Sensor_rxBufferRead
    #define RS485_Sensor_RXBUFFERWRITE          RS485_Sensor_rxBufferWrite
    #define RS485_Sensor_RXBUFFERLOOPDETECT     RS485_Sensor_rxBufferLoopDetect
    #define RS485_Sensor_RXBUFFER_OVERFLOW      RS485_Sensor_rxBufferOverflow
#endif /* End RS485_Sensor_RX_ENABLED */

#ifdef RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define RS485_Sensor_CONTROL                RS485_Sensor_CONTROL_REG
#endif /* End RS485_Sensor_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(RS485_Sensor_TX_ENABLED)
    #define RS485_Sensor_TXDATA                 RS485_Sensor_TXDATA_REG
    #define RS485_Sensor_TXSTATUS               RS485_Sensor_TXSTATUS_REG
    #define RS485_Sensor_TXSTATUS_MASK          RS485_Sensor_TXSTATUS_MASK_REG
    #define RS485_Sensor_TXSTATUS_ACTL          RS485_Sensor_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(RS485_Sensor_TXCLKGEN_DP)
        #define RS485_Sensor_TXBITCLKGEN_CTR        RS485_Sensor_TXBITCLKGEN_CTR_REG
        #define RS485_Sensor_TXBITCLKTX_COMPLETE    RS485_Sensor_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define RS485_Sensor_TXBITCTR_PERIOD        RS485_Sensor_TXBITCTR_PERIOD_REG
        #define RS485_Sensor_TXBITCTR_CONTROL       RS485_Sensor_TXBITCTR_CONTROL_REG
        #define RS485_Sensor_TXBITCTR_COUNTER       RS485_Sensor_TXBITCTR_COUNTER_REG
    #endif /* RS485_Sensor_TXCLKGEN_DP */
#endif /* End RS485_Sensor_TX_ENABLED */

#if(RS485_Sensor_HD_ENABLED)
    #define RS485_Sensor_TXDATA                 RS485_Sensor_TXDATA_REG
    #define RS485_Sensor_TXSTATUS               RS485_Sensor_TXSTATUS_REG
    #define RS485_Sensor_TXSTATUS_MASK          RS485_Sensor_TXSTATUS_MASK_REG
    #define RS485_Sensor_TXSTATUS_ACTL          RS485_Sensor_TXSTATUS_ACTL_REG
#endif /* End RS485_Sensor_HD_ENABLED */

#if( (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) )
    #define RS485_Sensor_RXDATA                 RS485_Sensor_RXDATA_REG
    #define RS485_Sensor_RXADDRESS1             RS485_Sensor_RXADDRESS1_REG
    #define RS485_Sensor_RXADDRESS2             RS485_Sensor_RXADDRESS2_REG
    #define RS485_Sensor_RXBITCTR_PERIOD        RS485_Sensor_RXBITCTR_PERIOD_REG
    #define RS485_Sensor_RXBITCTR_CONTROL       RS485_Sensor_RXBITCTR_CONTROL_REG
    #define RS485_Sensor_RXBITCTR_COUNTER       RS485_Sensor_RXBITCTR_COUNTER_REG
    #define RS485_Sensor_RXSTATUS               RS485_Sensor_RXSTATUS_REG
    #define RS485_Sensor_RXSTATUS_MASK          RS485_Sensor_RXSTATUS_MASK_REG
    #define RS485_Sensor_RXSTATUS_ACTL          RS485_Sensor_RXSTATUS_ACTL_REG
#endif /* End  (RS485_Sensor_RX_ENABLED) || (RS485_Sensor_HD_ENABLED) */

#if(RS485_Sensor_INTERNAL_CLOCK_USED)
    #define RS485_Sensor_INTCLOCK_CLKEN         RS485_Sensor_INTCLOCK_CLKEN_REG
#endif /* End RS485_Sensor_INTERNAL_CLOCK_USED */

#define RS485_Sensor_WAIT_FOR_COMLETE_REINIT    RS485_Sensor_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_RS485_Sensor_H */


/* [] END OF FILE */
