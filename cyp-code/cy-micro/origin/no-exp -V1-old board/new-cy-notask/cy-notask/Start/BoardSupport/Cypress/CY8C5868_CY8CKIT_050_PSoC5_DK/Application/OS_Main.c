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
File    : OS_StartLEDBlink.c
Purpose : embOS sample program running two simple tasks, each toggling
          an LED of the target hardware (as configured in BSP.c).
*/
        
//Port 1 Weigand:
//                  W0 -> In1 -> P2.1
//                  W1 -> In2 -> P2.2
//                  In -> In3 -> P2.3


//Port 2 Weignad:
//                  W0 -> In6 -> P6.4
//                  W1 -> In7 -> P6.5
//                  In -> In8 -> P6.6

// ###################################################### Includes #########################################################
#include "OS_Main.h"
// #########################################################################################################################

// ###################################################### Variables ########################################################
uint32 read_counter_port1 = 0;
uint32 read_counter_port2 = 0;
uint32 read_counter_total = 0;

uint32 read_failure_port1 = 0;
uint32 read_failure_port2 = 0;
uint32 read_failure_total = 0;

uint32 linux_read_counter_port1 = 0;
uint32 linux_read_counter_port2 = 0;
uint32 linux_read_counter_total = 0;

uint32 linux_read_failure_port1 = 0;
uint32 linux_read_failure_port2 = 0;
uint32 linux_read_failure_total = 0;

uint32 read_input_port1 = 0;
uint32 read_input_port2 = 0;
uint32 read_input_total = 0;

uint32 linux_read_input_failure_port1 = 0;
uint32 linux_read_input_failure_port2 = 0;
uint32 linux_read_input_failure_total = 0;

uint32 linux_uart_failure = 0;

uint8 Port1_Relay = 0;
uint8 Port1_Relay_Old = 0;
uint8 Port1_Locked = 0;
uint8 Port1_Locked_Old = 0;
uint8 Port1_w_Start = 0;
uint8 Port1_w_have = 0;
char Port1_W[129];
char In1[3];
uint8 Port1_W_Counter = 0;
uint8 Port1_W_bits = 26;
uint8 Port1_W_bits_Old = 26;

uint8 Port2_Relay = 0;
uint8 Port2_Relay_Old = 0;
uint8 Port2_Locked = 0;
uint8 Port2_Locked_Old = 0;
uint8 Port2_w_Start = 0;
uint8 Port2_w_have = 0;
char Port2_W[129];
char In2[3];
uint8 Port2_W_Counter = 0;
uint8 Port2_W_bits = 26;
uint8 Port2_W_bits_Old = 26;

uint8 UART_Linux_StartBit = 0;
char UART_Linux_Rx_Buffer[1024];
int UART_Linux_Rx_Counter = 0;

uint8 RGB_Color = 0;
uint8 RGB_Color_Old = 0;
// #########################################################################################################################

// ###################################################### Functions ########################################################
// #########################################################################################################################
//Job of Ports
void Port1_Job(uint8 intr)
{
    uint8 Port1_W0 = 0;
    uint8 Port1_W1 = 0;
    uint8 Port1_In = 0;
    
    switch(intr){
        case 1:{
            //W1_0
            Port1_W0 = 1;
            break;
        }
        case 2:{ 
            //W1_1
            Port1_W1 = 1;
            break;
        }
        case 4:{
            //In1
            Port1_In = 1;
            break;
        }
        case 5:{
            //W1_0, In1
            Port1_W0 = 1;
            Port1_In = 1;
            break;
        }
        case 6:{
            //W1_1, In1
            Port1_W1 = 1;
            Port1_In = 1;
            break;
        }
        default:{
            break;
        }
    }
    
    if(Port1_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In1)){   
        In1[0] = 'I';
        In1[1] = '1';
        In1[2] = '\0';
        
        
        read_input_port1++;
        read_input_total = read_input_port1 + read_input_port2;
        char data[256];
        sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
        
        UART_Debug_PutString("\r\n----------------------------\r\n");
        UART_Debug_PutString("Read Input from Port 1: ");
        CyWdtClear(); 
        UART_Debug_PutString(In1);
        UART_Debug_PutString("\r\n");
        UART_Debug_PutString(data);
        CyWdtClear(); 
        
        OS_TIMER_Restart(&Timer_AfterRead_In1);
    }
    
//    char sssss[100];
//    sprintf(sssss, "OS_TIMER_GetStatus(&Timer_AfterRead_Port1) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port1));
//    UART_Debug_PutString(sssss);
    
    if(!Port1_w_have){
        if((Port1_W0 || Port1_W1) && !Port1_Locked && !OS_TIMER_GetStatus(&Timer_AfterRead_Port1)){
            if(Port1_w_Start){
                OS_TIMER_Restart(&Timer_Read_W_Port1);
                if(Port1_W0){
                    Port1_W[Port1_W_Counter] = '0';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }
                
                if(Port1_W1){
                    Port1_W[Port1_W_Counter] = '1';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }
                
                if(Port1_W_Counter >= (Port1_W_bits + 3)){
                    Port1_W[Port1_W_Counter] = '\0';
                    Port1_W_Counter = Port1_W_Counter + 1;
                    
                    read_counter_port1++;
                    read_counter_total = read_counter_port1 + read_counter_port2;
                    char data[256];
                    sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
                    
                    UART_Debug_PutString("\r\n----------------------------\r\n");
                    UART_Debug_PutString("Read W Port 1: ");
                    CyWdtClear(); 
                    UART_Debug_PutString(Port1_W);
                    CyWdtClear(); 
                    UART_Debug_PutString("\r\n");
                    
                    UART_Debug_PutString(data);
                    CyWdtClear(); 
                    
                    OS_TIMER_Stop(&Timer_Read_W_Port1);
                    Port1_W_Counter = 0;
                    Port1_w_Start = 0;
                    Port1_w_have = 1;
                    
                    OS_TIMER_Restart(&Timer_AfterRead_Port1); 
                }
                    
            }
            else{
                Port1_w_Start = 1;
                OS_TIMER_Restart(&Timer_Read_W_Port1);
                
                Port1_W_Counter = 0;
                Port1_W[0] = 'P';
                Port1_W[1] = '1';
                Port1_W[2] = 'W';
                Port1_W_Counter = 3;
                
                if(Port1_W0){
                    Port1_W[Port1_W_Counter] = '0';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }
                
                if(Port1_W1){
                    Port1_W[Port1_W_Counter] = '1';
                    Port1_W_Counter = Port1_W_Counter + 1;
                }
            }
        }
    }
    
    return;
}

void Port2_Job(uint8 intr)
{
    uint8 Port2_W0 = 0;
    uint8 Port2_W1 = 0;
    uint8 Port2_In = 0;
    
    switch(intr){
        case 1:{
            //W2_0
            Port2_W0 = 1;
            break;
        }
        case 2:{ 
            //W2_1
            Port2_W1 = 1;
            break;
        }
        case 4:{
            //In2
            Port2_In = 1;
            break;
        }
        case 5:{
            //W2_0, In2
            Port2_W0 = 1;
            Port2_In = 1;
            break;
        }
        case 6:{
            //W2_1, In2
            Port2_W1 = 1;
            Port2_In = 1;
            break;
        }
        default:{
            break;
        }
    }
    
    if(Port2_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In2)){
        In2[0] = 'I';
        In2[1] = '2';
        In2[2] = '\0';
        
        read_input_port2++;
        read_input_total = read_input_port1 + read_input_port2;
        char data[256];
        sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
        
        UART_Debug_PutString("\r\n----------------------------\r\n");
        UART_Debug_PutString("Read Input from Port 2: ");
        CyWdtClear(); 
        UART_Debug_PutString(In2);
        UART_Debug_PutString("\r\n");
        UART_Debug_PutString(data);
        CyWdtClear(); 
        
        OS_TIMER_Restart(&Timer_AfterRead_In2);
    }
    
    //char sssss[100];
    //sprintf(sssss, "OS_TIMER_GetStatus(&Timer_AfterRead_Port2) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port2));
    //UART_Debug_PutString(sssss);
    
    if(!Port2_w_have){
        if((Port2_W0 || Port2_W1) && !Port2_Locked && !OS_TIMER_GetStatus(&Timer_AfterRead_Port2)){
            if(Port2_w_Start){
                OS_TIMER_Restart(&Timer_Read_W_Port2);
                if(Port2_W0){
                    Port2_W[Port2_W_Counter] = '0';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }
                
                if(Port2_W1){
                    Port2_W[Port2_W_Counter] = '1';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }
                
                if(Port2_W_Counter >= (Port2_W_bits + 3)){
                    Port2_W[Port2_W_Counter] = '\0';
                    Port2_W_Counter = Port2_W_Counter + 1;
                    
                    read_counter_port2++;
                    read_counter_total = read_counter_port1 + read_counter_port2;
                    char data[256];
                    sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
                    
                    UART_Debug_PutString("\r\n----------------------------\r\n");
                    UART_Debug_PutString("Read W Port 2: ");
                    CyWdtClear(); 
                    UART_Debug_PutString(Port2_W);
                    CyWdtClear(); 
                    UART_Debug_PutString("\r\n");
                    
                    CyWdtClear(); 
                    UART_Debug_PutString(data);
                    
                    OS_TIMER_Stop(&Timer_Read_W_Port2);
                    Port2_W_Counter = 0;
                    Port2_w_Start = 0;
                    Port2_w_have = 1;
                    
                    OS_TIMER_Restart(&Timer_AfterRead_Port2);
                }
                    
            }
            else{
                Port2_w_Start = 1;
                OS_TIMER_Restart(&Timer_Read_W_Port2);
                
                Port2_W_Counter = 0;
                Port2_W[0] = 'P';
                Port2_W[1] = '2';
                Port2_W[2] = 'W';
                Port2_W_Counter = 3;
                
                if(Port2_W0){
                    Port2_W[Port2_W_Counter] = '0';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }
                
                if(Port2_W1){
                    Port2_W[Port2_W_Counter] = '1';
                    Port2_W_Counter = Port2_W_Counter + 1;
                }
            }
        }
    }
    
    return;
}


//Main Job
void CmdTask(char* msg)
{
//	UART_Debug_PutString(msg);
//	UART_Debug_PutString("\r\n");
//	return;
    //read ports
    if(memcmp((char*)msg, "Port 1 Read", 11) == 0){
        //UART_Debug_PutString("Port 1 Read\r\n");
        if(Port1_W[0] != '\0' && Port1_w_have){
            UART_Linux_PutArray((uint8*)Port1_W, strlen(Port1_W) + 1);
//            UART_Linux_PutChar('\0');
            
            linux_read_counter_port1++;
            linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
            char data[256];
            sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);
            
            UART_Debug_PutString("Read New W from Port 1 with linux: ");
            UART_Debug_PutString(Port1_W);
            CyWdtClear(); 
            UART_Debug_PutString("\r\n");
            
            UART_Debug_PutString(data);
            CyWdtClear(); 
            
            OS_TIMER_Stop(&Timer_AfterRead_Port1);
            Port1_W_Counter = 0;
            Port1_w_Start = 0;
            Port1_W[0] = '\0';
            Port1_w_have = 0;
            UART_Debug_PutString("----------------------------\r\n");
            CyWdtClear(); 
        }
        
    }
    else if(memcmp((char*)msg, "Port 2 Read", 11) == 0){
        //UART_Debug_PutString("Port 2 Read\r\n");
        if(Port2_W[0] != '\0' && Port2_w_have){
            UART_Linux_PutArray((uint8*)Port2_W, strlen(Port2_W) + 1); 
            //UART_Linux_PutChar('\0');
            
            
            linux_read_counter_port2++;
            linux_read_counter_total = linux_read_counter_port1 + linux_read_counter_port2;
            char data[256];
            sprintf(data, "linux_read_counter_port1 = %lu, linux_read_counter_port2 = %lu, linux_read_counter_total = %lu\r\n", linux_read_counter_port1, linux_read_counter_port2, linux_read_counter_total);
            
            UART_Debug_PutString("Read New W from Port 2 with linux: ");
            UART_Debug_PutString(Port2_W);
            CyWdtClear(); 
            UART_Debug_PutString("\r\n");
            
            UART_Debug_PutString(data);
            CyWdtClear(); 
            
            OS_TIMER_Stop(&Timer_AfterRead_Port2);
            Port2_W_Counter = 0;
            Port2_w_Start = 0;
            Port2_W[0] = '\0';
            Port2_w_have = 0;
            UART_Debug_PutString("----------------------------\r\n");
            CyWdtClear(); 
        }    
    }
    //read iputs
    else if(memcmp((char*)msg, "Port 1 Input", 12) == 0){
        if(In1[0] != '\0'){
            UART_Linux_PutArray((uint8*)In1, strlen(In1) + 1); 
//            UART_Linux_PutChar('\0');
            
            OS_TIMER_Stop(&Timer_AfterRead_In1);
            
            UART_Debug_PutString("Read New Input from Port 1 with linux: ");
            UART_Debug_PutString(In1);
            CyWdtClear(); 
            UART_Debug_PutString("\r\n");
            
            OS_TIMER_Stop(&Timer_AfterRead_In1);
            In1[0] = '\0';
            In1[1] = '\0';
            In1[2] = '\0';
            UART_Debug_PutString("----------------------------\r\n");
        }    
    }
    else if(memcmp((char*)msg, "Port 2 Input", 12) == 0){
        if(In2[0] != '\0'){
            UART_Linux_PutArray((uint8*)In2, strlen(In2) + 1); 
//            UART_Linux_PutChar('\0');
            
            UART_Debug_PutString("Read New Input from Port 2 with linux: ");
            UART_Debug_PutString(In2);
            CyWdtClear(); 
            UART_Debug_PutString("\r\n");
            
            OS_TIMER_Stop(&Timer_AfterRead_In2);
            In2[0] = '\0';
            In2[1] = '\0';
            In2[2] = '\0';
            UART_Debug_PutString("----------------------------\r\n");
            CyWdtClear(); 
        }        
    }
    //relay open close
    else if(memcmp((char*)msg, "Port 1 Open", 11) == 0){
        Port1_Relay = 1;   
        if(Port1_Relay != Port1_Relay_Old){
            Port1_Relay_Old = Port1_Relay;
            UART_Debug_PutString("Port 1 Open\r\n");
            CyWdtClear(); 
        }
        Relay_Port1_Write(Port1_Relay); 
        CyWdtClear();
    }
    else if(memcmp((char*)msg, "Port 1 Close", 12) == 0){
        Port1_Relay = 0;  
        if(Port1_Relay != Port1_Relay_Old){
            Port1_Relay_Old = Port1_Relay;
            UART_Debug_PutString("Port 1 Close\r\n");
            CyWdtClear();
        }
        Relay_Port1_Write(Port1_Relay);
        CyWdtClear(); 
    }
    else if(memcmp((char*)msg, "Port 2 Open", 11) == 0){
        Port2_Relay = 1; 
        if(Port2_Relay != Port2_Relay_Old){
            Port2_Relay_Old = Port2_Relay;
            UART_Debug_PutString("Port 2 Open\r\n");
            CyWdtClear(); 
        }
        Relay_Port2_Write(Port2_Relay);
        CyWdtClear(); 
    }
    else if(memcmp((char*)msg, "Port 2 Close", 12) == 0){
        Port2_Relay = 0; 
        if(Port2_Relay != Port2_Relay_Old){
            Port2_Relay_Old = Port2_Relay;
            UART_Debug_PutString("Port 2 Close\r\n");
            CyWdtClear(); 
        }
        Relay_Port2_Write(Port2_Relay);
        CyWdtClear(); 
    }
    //port locks
    else if(memcmp((char*)msg, "Port 1 Lock", 11) == 0){
        Port1_Locked = 1; 
        if(Port1_Locked != Port1_Locked_Old){
            Port1_Locked_Old = Port1_Locked;
            UART_Debug_PutString("Port 1 Lock\r\n");
            CyWdtClear(); 
        }
    }
    else if(memcmp((char*)msg, "Port 1 UnLock", 13) == 0){
        Port1_Locked = 0;
        if(Port1_Locked != Port1_Locked_Old){
            Port1_Locked_Old = Port1_Locked;
            UART_Debug_PutString("Port 1 UnLock\r\n");
            CyWdtClear(); 
        }
    }
    else if(memcmp((char*)msg, "Port 2 Lock", 11) == 0){
        Port2_Locked = 1;
        if(Port2_Locked != Port2_Locked_Old){
            Port2_Locked_Old = Port2_Locked;
            UART_Debug_PutString("Port 2 Lock\r\n");
            CyWdtClear(); 
        }
    }
    else if(memcmp((char*)msg, "Port 2 UnLock", 13) == 0){
        Port2_Locked = 0;   
        if(Port2_Locked != Port2_Locked_Old){
            Port2_Locked_Old = Port2_Locked;
            UART_Debug_PutString("Port 2 UnLock\r\n");
            CyWdtClear(); 
        }
    }
    //port w config bits
    else if(memcmp((char*)msg, "Port 1 Wbits:", 13) == 0){
        //Port1_W_bits
        char* ret = NULL;
        ret = strremove((char*)msg, "Port 1 Wbits:");
        CyWdtClear(); 
        
        Port1_W_bits = atoi(ret);
        CyWdtClear(); 
        if(Port1_W_bits != Port1_W_bits_Old){
            Port1_W_bits_Old = Port1_W_bits;
            char ss[100];
            sprintf(ss, "Port 1 Wbits = %d\r\n", Port1_W_bits);
            UART_Debug_PutString(ss);
            CyWdtClear(); 
        }
    }
    else if(memcmp((char*)msg, "Port 2 Wbits:", 13) == 0){
        //Port2_W_bits
        char* ret = NULL;
        ret = strremove((char*)msg, "Port 2 Wbits:");
        CyWdtClear(); 
        Port2_W_bits = atoi(ret);
        CyWdtClear(); 
        if(Port2_W_bits != Port2_W_bits_Old){
            Port2_W_bits_Old = Port2_W_bits;
            char ss[100];
            sprintf(ss, "Port 2 Wbits = %d\r\n", Port2_W_bits);
            UART_Debug_PutString(ss);
            CyWdtClear(); 
        }
    }
    //Cy RGB mode
    else if(memcmp((char*)msg, "Cy RGB:", 7) == 0){
        //RGB
        OS_TIMER_Restart(&Timer_RGB_LED);
        char* ret = NULL;
        ret = strremove((char*)msg, "Cy RGB:");
        CyWdtClear(); 
        uint32 color = strtol(ret, NULL, 10);
        CyWdtClear(); 
        RGB_Color = (uint8)color;
        if(RGB_Color != RGB_Color_Old){
            RGB_Color_Old = RGB_Color;
            if(RGB_Color > 6 || RGB_Color < 0){
                RGB_Color = 1;
                RGB_Color_Old = 1;
            }
            char ss[100];
            sprintf(ss, "RGB_Color = %d\r\n", RGB_Color);
            UART_Debug_PutString(ss);
            CyWdtClear(); 
        }
        RGB(RGB_Color);
        
    }
    //Cy reset
    else if(memcmp((char*)msg, "Cy Reset", 8) == 0){
        CyWdtClear(); 
        CySoftwareReset();
    }
    else if(memcmp((char*)msg, "Cy Connect", 10) == 0){
        UART_Linux_PutArray((uint8*)"Cy Connected", strlen("Cy Connected") + 1); 
//        UART_Linux_PutChar('\0');
        CyWdtClear(); 
    }
    else{
        UART_Debug_PutString("Wrong Msg!!!!! \r\n");
        CyWdtClear(); 
    }
    return;
}

//Interrupt Service Routine
CY_ISR(UART_Linux_Rx_ISR) 
{   
    OS_INT_Enter();
    
    OS_TIMER_Restart(&Timer_Read_Linux);
    char c = UART_Linux_GetChar();
    
    if(c == '\0'){  
        UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter] = c;
        UART_Linux_Rx_Counter++;

        CmdTask(UART_Linux_Rx_Buffer);
        OS_TIMER_Stop(&Timer_Read_Linux);
        
        memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
        CyWdtClear(); 
        UART_Linux_Rx_Counter = 0;
    }
    else{
        UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter] = c;
        UART_Linux_Rx_Counter++;
    }
    
    OS_INT_Leave();
    return;
    
}

CY_ISR(Port1_ISR)
{
//    CyGlobalIntDisable;
    OS_INT_Enter();

    uint8 intr = Pins_Port1_ClearInterrupt();
    
    Port1_Job(intr);
    
    OS_INT_Leave();
//     CyGlobalIntEnable;
    return;
}

CY_ISR(Port2_ISR)
{
//    CyGlobalIntDisable;
    OS_INT_Enter();
    
    uint8 intr = Pins_Port2_ClearInterrupt();
    
    Port2_Job(intr);
    
    OS_INT_Leave();
//    CyGlobalIntEnable;
    return;
}


//Callbacks
void Timer_Read_Linux_Callback()
{
    OS_TIMER_Stop(&Timer_Read_Linux);
    
    linux_uart_failure++;
    char data[128];
    sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
    
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
    CyWdtClear(); 
    UART_Linux_Rx_Counter = 0;
    
    return;
}

void Timer_RGB_LED_Callback()
{
    OS_TIMER_Stop(&Timer_RGB_LED);
    
    RGB_Color = 1;
    if(RGB_Color != RGB_Color_Old){
        RGB_Color_Old = RGB_Color;
        RGB(RGB_Color);
        
        char ss[256];
        sprintf(ss, "Timer_RGB_LED_Callback --> RGB_Color = %d\r\n", RGB_Color);
        CyWdtClear(); 
        UART_Debug_PutString(ss);
        CyWdtClear(); 
    }
    
    OS_TIMER_Restart(&Timer_RGB_LED);
    
    return;
}

void Timer_Read_W_Port1_Callback()
{
    OS_TIMER_Stop(&Timer_Read_W_Port1);
    
    read_failure_port1++;
    read_failure_total = read_failure_port1 + read_failure_port2;
    char data[256];
    sprintf(data, "read_failure_port1 = %lu, read_failure_port2 = %lu, read_failure_total = %lu\r\n", read_failure_port1, read_failure_port2, read_failure_total);
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_Read_W_Port1_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    Port1_W_Counter = 0;
    Port1_w_Start = 0;
    Port1_W[0] = '\0';
    Port1_w_have = 0;
    
    return;
}

void Timer_Read_W_Port2_Callback()
{
    OS_TIMER_Stop(&Timer_Read_W_Port2);
    
    read_failure_port2++;
    read_failure_total = read_failure_port1 + read_failure_port2;
    char data[256];
    sprintf(data, "read_failure_port1 = %lu, read_failure_port2 = %lu, read_failure_total = %lu\r\n", read_failure_port1, read_failure_port2, read_failure_total);
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_Read_W_Port2_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    Port2_W_Counter = 0;
    Port2_w_Start = 0;
    Port2_W[0] = '\0';
    Port2_w_have = 0;
    
    return;
}

void Timer_AfterRead_Input1_Callback()
{
    OS_TIMER_Stop(&Timer_AfterRead_In1);
    
    linux_read_input_failure_port1++;
    linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
    char data[256];
    sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
    
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_AfterRead_Input1_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    In1[0] = '\0';
    In1[1] = '\0';
    In1[2] = '\0';
    
    return;
}

void Timer_AfterRead_Port1_Callback()
{
    OS_TIMER_Stop(&Timer_AfterRead_Port1);
    
    linux_read_failure_port1++;
    linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
    char data[256];
    sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
    
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_AfterRead_Port1_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    Port1_W_Counter = 0;
    Port1_w_Start = 0;
    Port1_W[0] = '\0';
    Port1_w_have = 0;

    return;
}

void Timer_AfterRead_Input2_Callback()
{
    OS_TIMER_Stop(&Timer_AfterRead_In2);
    
    linux_read_input_failure_port2++;
    linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
    char data[256];
    sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
    
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_AfterRead_Input2_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
    
    In2[0] = '\0';
    In2[1] = '\0';
    In2[2] = '\0';
    
    return;
}

void Timer_AfterRead_Port2_Callback()
{
    OS_TIMER_Stop(&Timer_AfterRead_Port2);
    
    linux_read_failure_port2++;
    linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
    char data[256];
    sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
    
    UART_Debug_PutString("****************************\r\n");
    UART_Debug_PutString("Timer_AfterRead_Port2_Callback\r\n");
    CyWdtClear(); 
    UART_Debug_PutString(data);
    UART_Debug_PutString("****************************\r\n");
    CyWdtClear(); 
   
    Port2_W_Counter = 0;
    Port2_w_Start = 0;
    Port2_W[0] = '\0';
    Port2_w_have = 0;
    
    return;
}

void Timer_WD_Callback()
{
    OS_TIMER_Stop(&Timer_WD);
    CyWdtClear();
    OS_TIMER_Restart(&Timer_WD);
    
    return;
}


//Init Functions
void intrrupt_init()
{
    intr_UART_Linux_Rx_StartEx(UART_Linux_Rx_ISR);
    UART_Linux_EnableRxInt();
    
    intr_Port1_StartEx(Port1_ISR);
    intr_Port2_StartEx(Port2_ISR);
    
    return;
}

void init_vars()
{
    Port1_W[0] = '\0';
    In1[0] = '\0';
    
    Port2_W[0] = '\0';
    In2[0] = '\0';
    
    return;
}

void timers_create()
{
    // Create without start.
    OS_TIMER_Create(&Timer_Read_Linux, Timer_Read_Linux_Callback, 100);
    
    OS_TIMER_Create(&Timer_RGB_LED, Timer_RGB_LED_Callback, 1000);
    
    OS_TIMER_Create(&Timer_Read_W_Port1, Timer_Read_W_Port1_Callback, 100);
    OS_TIMER_Create(&Timer_Read_W_Port2, Timer_Read_W_Port2_Callback, 100);  
    
    OS_TIMER_Create(&Timer_AfterRead_In1, Timer_AfterRead_Input1_Callback, 500);
    OS_TIMER_Create(&Timer_AfterRead_Port1, Timer_AfterRead_Port1_Callback, 500);  
    
    OS_TIMER_Create(&Timer_AfterRead_In2, Timer_AfterRead_Input2_Callback, 500);
    OS_TIMER_Create(&Timer_AfterRead_Port2, Timer_AfterRead_Port2_Callback, 500);
    
    OS_TIMER_Create(&Timer_WD, Timer_WD_Callback, 100);
    
    return;
}

/*********************************************************************
*
*       main()
*/
int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE); 
    CyWdtClear(); 
    
    OS_Init();    // Initialize embOS
    OS_InitHW();  // Initialize required hardware
    BSP_Init();   // Initialize LED ports
    
    RGB_Color = 0;
    RGB_Color_Old = 0;
    RGB(RGB_Color); //Cypress run
    
    //CyDelay(1000);
    
    intrrupt_init();
    CyWdtClear(); 
    timers_create();
    CyWdtClear(); 
    init_vars();
    CyWdtClear(); 
    
    UART_Debug_PutString("********************************* Start Cy IO *********************************\r\n");
    CyWdtClear(); 
    cymonitoring_get_lastWtd();
    CyWdtClear(); 
    
    RGB_Color = 1;
    RGB_Color_Old = 1;
    RGB(RGB_Color); //Cypress run
    
    Timer_WD_Callback();
    
    OS_Start();   // Start embOS
    
    return 0;
}
/*************************** End of file ****************************/
