/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

// ###################################################### Includes #########################################################
#include "ChipMonitor.h"
// #########################################################################################################################
    
// ###################################################### Prototypes #######################################################
void cymonitoring_get_lastWtd()
{
    uint8 stat = CyResetStatus;
    switch(stat){
        case 0:
            // Hardware Reset
            UART_Debug_PutString("cymonitoring_get_lastWtd: last reset cause: Hardware Reset\r\n");
            break;
        case 8:
            // Watchdog Reset
            UART_Debug_PutString("cymonitoring_get_lastWtd: last reset cause: Watchdog Reset\r\n");
            break;
        case 32:
            // Software Reset
            UART_Debug_PutString("cymonitoring_get_lastWtd: last reset cause: Software Reset\r\n");
            break;
        default:
            break;
    }
    
    return;
}
// #########################################################################################################################

/* [] END OF FILE */
