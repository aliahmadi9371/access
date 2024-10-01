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

//Inputs:
//                  In1 -> p2.4
//                  In2 -> p2.7
// ###################################################### Includes #########################################################
#include "OS_Main.h"
// #########################################################################################################################

// ###################################################### Variables ########################################################
// ###################################################### Variables ########################################################
OS_TIMER Timer_Read_Linux;
OS_TIMER Timer_RGB_LED;
OS_TIMER Timer_Read_W_Port1;
OS_TIMER Timer_Read_W_Port2;
OS_TIMER Timer_AfterRead_In1;
OS_TIMER Timer_AfterRead_Port1;
OS_TIMER Timer_AfterRead_In2;
OS_TIMER Timer_AfterRead_Port2;
OS_TIMER Timer_WD;
OS_TIMER Timer_Inps;
OS_TIMER Timer_Update;


OS_TIMER Timer_intr1;
OS_TIMER Timer_intr2;

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

//uint8 RGB_Color = 0;
//uint8 RGB_Color_Old = 0;

char Inputs[13] = "{\"inps\":\"FF\"}";
//char Inputs_Old[20] = "";
uint8 inps_old = 0b11111111;

uint8 intr1[256];
uint16 intr1_counter = 0;

uint8 intr2[256];
uint16 intr2_counter = 0;

uint8 inp_1_old = 1;
uint8 inp_2_old = 1;
// #########################################################################################################################

// ###################################################### Functions ########################################################
// #########################################################################################################################
//Job of Ports
void Port1_Job(uint8 intr)
{
    uint8 Port1_W0 = 0;
    uint8 Port1_W1 = 0;
    
    uint8 Port1 = 0;
    Port1 = intr;
    check_port_1(Port1, &Port1_W0, &Port1_W1);
    check_port_wiegand_1(Port1_W0, Port1_W1);  
    
//    char s1[100];
//    sprintf(s1, "OS_TIMER_GetStatus(&Timer_AfterRead_Port1) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port1));
//    UART_Debug_PutString(s1);  
    
    return;
}

void check_port_1(uint8 Port1, uint8* p1_0, uint8* p1_1)
{
    switch(Port1){
        case 1:{
            //W1_0
            *p1_0 = 1;
            break;
        }
        case 2:{ 
            //W1_1
            *p1_1 = 1;
            break;
        }
        //case 4:{
            ////In1
            //*in1 = 1;
            //break;
        //}
        //case 5:{
            ////W1_0, In1
            //*p1_0 = 1;
            //*in1 = 1;
             //break;
        //}
        //case 6:{
            ////W1_1, In1
            //*p1_1 = 1;
            //*in1 = 1;
             //break;
        //}
        default:{
            break;
        }
    }
    
    return;
}

void check_port_input_1(uint8 Port1_In)
{
    if(Port1_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In1)){   
        In1[0] = 'I';
        In1[1] = '1';
        In1[2] = '\0';
        
        
        //read_input_port1++;
        //read_input_total = read_input_port1 + read_input_port2;
        //char data[256];
        //sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
        
        //UART_Debug_PutString("\r\n----------------------------\r\n");
        //UART_Debug_PutString("Read Input from Port 1: ");
        //CyWdtClear(); 
        //UART_Debug_PutString(In1);
        //UART_Debug_PutString("\r\n");
        //UART_Debug_PutString(data);
        //CyWdtClear(); 
        
        OS_TIMER_Restart(&Timer_AfterRead_In1);
    }

    return;
}

void check_port_wiegand_1(uint8 Port1_W0, uint8 Port1_W1)
{
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
                    
                    //read_counter_port1++;
                    //read_counter_total = read_counter_port1 + read_counter_port2;
                    //char data[256];
                    //sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
                    
                    //UART_Debug_PutString("\r\n----------------------------\r\n");
                    //UART_Debug_PutString("Read W Port 1: ");
                    //CyWdtClear(); 
                    //UART_Debug_PutString(Port1_W);
                    //CyWdtClear(); 
                    //UART_Debug_PutString("\r\n");
                    
                    //UART_Debug_PutString(data);
                    //CyWdtClear(); 
                    
                    //Port1_W_Counter = 0;
                    //Port1_w_Start = 0;
                    
                    Port1_w_have = 1;
                    OS_TIMER_Stop(&Timer_Read_W_Port1);
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
    
    uint8 Port2 = 0;
    Port2 = intr;
    
    check_port_2(Port2, &Port2_W0, &Port2_W1);
    check_port_wiegand_2(Port2_W0, Port2_W1);
    
    //char s2[100];
    //sprintf(s2, "OS_TIMER_GetStatus(&Timer_AfterRead_Port2) = %d\r\n", OS_TIMER_GetStatus(&Timer_AfterRead_Port2));
    //UART_Debug_PutString(s2);
    
    return;
}

void check_port_2(uint8 Port2, uint8* p2_0, uint8* p2_1)
{
    switch(Port2){
        case 1:{
            //W2_0
            *p2_0 = 1;
            break;
        }
        case 2:{ 
            //W2_1
            *p2_1 = 1;
            break;
        }
        //case 4:{
            ////In2
            //*in2 = 1;
            //break;
        //}
        //case 5:{
            ////W2_0, In2
            //*p2_0 = 1;
            //*in2 = 1;
            //break;
        //}
        //case 6:{
            //W2_1, In2
            //*p2_1 = 1;
            //*in2 = 1;
            //break;
        //}
        default:{
            break;
        }
    }
    
    return;
}

void check_port_input_2(uint8 Port2_In)
{
    if(Port2_In && !OS_TIMER_GetStatus(&Timer_AfterRead_In2)){
        In2[0] = 'I';
        In2[1] = '2';
        In2[2] = '\0';
        
        //read_input_port2++;
        //read_input_total = read_input_port1 + read_input_port2;
        //char data[256];
        //sprintf(data, "read_input_port1 = %lu, read_input_port2 = %lu, read_input_total = %lu\r\n", read_input_port1, read_input_port2, read_input_total);
        
        //UART_Debug_PutString("\r\n----------------------------\r\n");
        //UART_Debug_PutString("Read Input from Port 2: ");
        //CyWdtClear(); 
        //UART_Debug_PutString(In2);
        //UART_Debug_PutString("\r\n");
        //UART_Debug_PutString(data);
        //CyWdtClear(); 
        
        OS_TIMER_Restart(&Timer_AfterRead_In2);
    }
    
    return;
}

void check_port_wiegand_2(uint8 Port2_W0, uint8 Port2_W1)
{
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
                    
                    //read_counter_port2++;
                    //read_counter_total = read_counter_port1 + read_counter_port2;
                    //char data[256];
                    //sprintf(data, "read_counter_port1 = %lu, read_counter_port2 = %lu, read_counter_total = %lu\r\n", read_counter_port1, read_counter_port2, read_counter_total);
                    
                    //UART_Debug_PutString("\r\n----------------------------\r\n");
                    //UART_Debug_PutString("Read W Port 2: ");
                    //CyWdtClear(); 
                    //UART_Debug_PutString(Port2_W);
                    //CyWdtClear(); 
                    //UART_Debug_PutString("\r\n");
                    
                    //CyWdtClear(); 
                    //UART_Debug_PutString(data);
                    
                    
                    //Port2_W_Counter = 0;
                    //Port2_w_Start = 0;
                    
                    Port2_w_have = 1;
                    OS_TIMER_Stop(&Timer_Read_W_Port2);
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

void Inputs_Job()
{
    uint8 inp4 = In4_Read() & 0b00000001;
    uint8 inp5 = In5_Read() & 0b00000001;
    CyWdtClear();

    uint8 inps = ((inp4 << 3) + (inp5 << 4)) | 0b11100111;
    inps = inps | 0b11100111;
    CyWdtClear();
    
    if(inps_old != inps){
    	inps_old = inps;
    	sprintf(Inputs, "{\"inps\":\"%02X\"}", inps);
		if(!q_put_message(&_Queue_Send, Inputs, 14)){
			UART_Debug_PutString("Inputs_Job >> failed to put to _Queue_Send >> ");
			UART_Debug_PutString(Inputs);
			UART_Debug_PutString("\r\n");
		}
    	CyWdtClear();
    }
    return;
}

void Inps()
{
    uint8 inp_1 = Inp_1_Read();
    uint8 inp_2 = Inp_2_Read();
    
    if(inp_1_old != inp_1){
        inp_1_old = inp_1;
        if(!inp_1)
            check_port_input_1(1);
    }
        
    if(inp_2_old != inp_2){
        inp_2_old = inp_2;
        if(!inp_2)
            check_port_input_2(1);
    }
    
    return;
}


//Main Job
void CmdTask(char* msg)
{
    //read ports
    if(fota_flag_start == 1){
		if((memcmp((char*)&msg[0], "{FOT_FOT}:", 10) == 0)){
//			OS_TIMER_Restart(&Timer_Update);
			update_procedure(&((char*)msg)[10]);
		}
		else{
			UART_Debug_PutString("UPDATE IN PROGRESS >> NOT AN UPDATE PACKET!!!\r\n");
		}
	}
	else{
        /*if(memcmp((char*)msg, "Port 1 Read", 11) == 0){
            if(Port1_W[0] != '\0' && Port1_w_have){
                UART_Linux_PutString(Port1_W);
                UART_Linux_PutChar('\0');
                
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

            if(Port2_W[0] != '\0' && Port2_w_have){
                UART_Linux_PutString(Port2_W); 
                UART_Linux_PutChar('\0');
                
                
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
                UART_Linux_PutString(In1); 
                UART_Linux_PutChar('\0');
                
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
                UART_Linux_PutString(In2); 
                UART_Linux_PutChar('\0');
                
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
        else*/ if(memcmp((char*)msg, "Port 1 Open", 11) == 0){
            Port1_Relay = 1;   
            if(Port1_Relay != Port1_Relay_Old){
                Port1_Relay_Old = Port1_Relay;
                Relay_Port1_Write(Port1_Relay);
                UART_Debug_PutString("Port 1 Open\r\n");
                CyWdtClear(); 
            }
        }
        else if(memcmp((char*)msg, "Port 1 Close", 12) == 0){
            Port1_Relay = 0;  
            if(Port1_Relay != Port1_Relay_Old){
                Port1_Relay_Old = Port1_Relay;
                Relay_Port1_Write(Port1_Relay);
                UART_Debug_PutString("Port 1 Close\r\n");
                CyWdtClear(); 
            }
        }
        else if(memcmp((char*)msg, "Port 2 Open", 11) == 0){
            Port2_Relay = 1; 
            if(Port2_Relay != Port2_Relay_Old){
                Port2_Relay_Old = Port2_Relay;
                Relay_Port2_Write(Port2_Relay);
                UART_Debug_PutString("Port 2 Open\r\n");
                CyWdtClear(); 
            }
        }
        else if(memcmp((char*)msg, "Port 2 Close", 12) == 0){
            Port2_Relay = 0; 
            if(Port2_Relay != Port2_Relay_Old){
                Port2_Relay_Old = Port2_Relay;
                Relay_Port2_Write(Port2_Relay);
                UART_Debug_PutString("Port 2 Close\r\n");
                CyWdtClear(); 
            }
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
                RGB(RGB_Color);
                char ss[100];
                sprintf(ss, "RGB_Color = %d\r\n", RGB_Color);
                UART_Debug_PutString(ss);
                CyWdtClear(); 
            }
            
        }
        //Cy reset
        else if(memcmp((char*)msg, "Cy Reset", 8) == 0){
            CyWdtClear(); 
            CySoftwareReset();
        }
        else if(memcmp((char*)msg, "Cy Connect", 10) == 0){
            UART_Linux_PutArrayConst((uint8*)("Cy Connected"), 13);
            //UART_Linux_PutString("Cy Connected"); 
            //UART_Linux_PutChar('\0');
            CyWdtClear(); 
        }
        /*else if(memcmp(msg, "Inputs", 6) == 0){
            UART_Linux_PutString(Inputs); 
            UART_Linux_PutChar('\0');
            CyWdtClear(); 
        }*/
        else if(memcmp(msg, "{SET_FOT}:", 10) == 0){
    	    	UART_Debug_PutString("\r\nUPDATE COMMAND RECEIVED!!!\r\n");
    //			OS_TIMER_Restart(&Timer_Update);
    	    	RGB_Color = 5;
    	    	RGB_Color_Old = 5;
    	    	RGB(RGB_Color);
    			update_check_packet(msg);
    			CyWdtClear(); 

        }
        else{
            UART_Linux_ClearRxBuffer();
            UART_Linux_ClearTxBuffer();
            
            UART_Debug_PutString("---------- Wrong Msg: ");
            UART_Debug_PutString(msg);
            UART_Debug_PutString("\r\n");
            CyWdtClear(); 
        }
    }
    return;
}

//Interrupt Service Routine
CY_ISR(UART_Linux_Rx_ISR) 
{   
    OS_INT_Enter();

	OS_TIMER_Restart(&Timer_Read_Linux);
	UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter] = UART_Linux_GetChar();
	UART_Linux_Rx_Counter++;

    OS_INT_Leave();
    return;
}

CY_ISR(Port1_ISR)
{
    OS_INT_Enter();
    
    Pins_Port1_ClearInterrupt();
    //Port1_Job(intr);
    
    OS_TIMER_Restart(&Timer_intr1);
	intr1[intr1_counter] = ~Pins_Port1_Read() & 0b00000011;
	intr1_counter++;
    
    OS_INT_Leave();
    return;
}

CY_ISR(Port2_ISR)
{
    OS_INT_Enter();
    
    Pins_Port2_ClearInterrupt();
    //Port2_Job(intr);
    
    OS_TIMER_Restart(&Timer_intr2);
	intr2[intr2_counter] = ~Pins_Port2_Read() & 0b00000011;
	intr2_counter++;
    
    OS_INT_Leave();
    return;
}


//Callbacks
void Timer_Read_Linux_Callback()
{
    //OS_TIMER_Stop(&Timer_Read_Linux);
    
    //linux_uart_failure++;
    //char data[128];
    //sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
    
    //UART_Debug_PutString("****************************\r\n");
    //UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
    //CyWdtClear(); 
    //UART_Debug_PutString(data);
    //UART_Debug_PutString("****************************\r\n");
    //CyWdtClear(); 
    
    //memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
    //CyWdtClear(); 
    //UART_Linux_Rx_Counter = 0;
    
    
    if(UART_Linux_Rx_Buffer[UART_Linux_Rx_Counter - 1] == '\0'){
		if(!q_put_message(&_Queue_Main, UART_Linux_Rx_Buffer, UART_Linux_Rx_Counter)){
			UART_Debug_PutString("HAL_UART_RxCpltCallback >> failed to put to _Queue_Main >> ");
			UART_Debug_PutString(UART_Linux_Rx_Buffer);
			UART_Debug_PutString("\r\n");
		}
	}
	else{
		linux_uart_failure++;
		char data[128];
		sprintf(data, "linux_uart_failure = %lu\r\n", linux_uart_failure);
		UART_Debug_PutString("****************************\r\n");
		UART_Debug_PutString("Timer_Read_Linux_Callback\r\n");
		UART_Debug_PutString(data);
		UART_Debug_PutString(UART_Linux_Rx_Buffer);
		UART_Debug_PutString("\r\n");
		UART_Debug_PutString("****************************\r\n");
	}

	memset(UART_Linux_Rx_Buffer, '\0', UART_Linux_Rx_Counter);
	UART_Linux_Rx_Counter = 0;
    
    return;
}

void Timer_RGB_LED_Callback()
{
    OS_TIMER_Stop(&Timer_RGB_LED);
    
    if(!fota_flag_start){
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
    //OS_TIMER_Stop(&Timer_AfterRead_In1);
    
    //linux_read_input_failure_port1++;
    //linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
    //char data[256];
    //sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
    
    //UART_Debug_PutString("****************************\r\n");
    //UART_Debug_PutString("Timer_AfterRead_Input1_Callback\r\n");
    //CyWdtClear(); 
    //UART_Debug_PutString(data);
    //UART_Debug_PutString("****************************\r\n");
    //CyWdtClear(); 
    
    
    if(!q_put_message(&_Queue_Send, In1, 3)){
		UART_Debug_PutString("Timer_AfterRead_Input1_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(In1);
		UART_Debug_PutString("\r\n");
	}
    
    In1[0] = '\0';
    In1[1] = '\0';
    In1[2] = '\0';
    
    return;
}

void Timer_AfterRead_Port1_Callback()
{
    //OS_TIMER_Stop(&Timer_AfterRead_Port1);
    
    //linux_read_failure_port1++;
    //linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
    //char data[256];
    //sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
    
    //UART_Debug_PutString("****************************\r\n");
    //UART_Debug_PutString("Timer_AfterRead_Port1_Callback\r\n");
    //CyWdtClear(); 
    //UART_Debug_PutString(data);
    //UART_Debug_PutString("****************************\r\n");
    //CyWdtClear();
    
    if(!q_put_message(&_Queue_Send, Port1_W, Port1_W_Counter)){
		UART_Debug_PutString("Timer_AfterRead_Port1_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(Port1_W);
		UART_Debug_PutString("\r\n");
	}
    
    Port1_W_Counter = 0;
    Port1_w_Start = 0;
    Port1_W[0] = '\0';
    Port1_w_have = 0;

    return;
}

void Timer_AfterRead_Input2_Callback()
{
    //OS_TIMER_Stop(&Timer_AfterRead_In2);
    
    //linux_read_input_failure_port2++;
    //linux_read_input_failure_total = linux_read_input_failure_port1 + linux_read_input_failure_port2;
    //char data[256];
    //sprintf(data, "linux_read_input_failure_port1 = %lu, linux_read_input_failure_port2 = %lu, linux_read_input_failure_total = %lu\r\n", linux_read_input_failure_port1, linux_read_input_failure_port2, linux_read_input_failure_total);
    
    //UART_Debug_PutString("****************************\r\n");
    //UART_Debug_PutString("Timer_AfterRead_Input2_Callback\r\n");
    //CyWdtClear(); 
    //UART_Debug_PutString(data);
    //UART_Debug_PutString("****************************\r\n");
    //CyWdtClear(); 
    
    if(!q_put_message(&_Queue_Send, In2, 3)){
		UART_Debug_PutString("Timer_AfterRead_Input2_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(In2);
		UART_Debug_PutString("\r\n");
	}
    
    In2[0] = '\0';
    In2[1] = '\0';
    In2[2] = '\0';
    
    return;
}

void Timer_AfterRead_Port2_Callback()
{
    //OS_TIMER_Stop(&Timer_AfterRead_Port2);
    
    //linux_read_failure_port2++;
    //linux_read_failure_total = linux_read_failure_port1 + linux_read_failure_port2;
    //char data[256];
    //sprintf(data, "linux_read_failure_port1 = %lu, linux_read_failure_port2 = %lu, linux_read_failure_total = %lu\r\n", linux_read_failure_port1, linux_read_failure_port2, linux_read_failure_total);
    
    //UART_Debug_PutString("****************************\r\n");
    //UART_Debug_PutString("Timer_AfterRead_Port2_Callback\r\n");
    //CyWdtClear(); 
    //UART_Debug_PutString(data);
    //UART_Debug_PutString("****************************\r\n");
    //CyWdtClear(); 
   
    if(!q_put_message(&_Queue_Send, Port2_W, Port2_W_Counter)){
		UART_Debug_PutString("Timer_AfterRead_Port2_Callback >> failed to put to _Queue_Send >> ");
		UART_Debug_PutString(Port2_W);
		UART_Debug_PutString("\r\n");
	}
    
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

void Timer_Inps_Callback()
{
    OS_TIMER_Stop(&Timer_Inps);
    Inputs_Job();
    Inps();
    OS_TIMER_Restart(&Timer_Inps);
    return;
}

void Timer_Intr1_Callback()
{
    //UART_Debug_PutString("Timer_Intr1_Callback\r\n");
	for(uint8 i = 0; i < intr1_counter; i++)
		Port1_Job(intr1[i]);
	intr1_counter = 0;
	return;
}

void Timer_Intr2_Callback()
{
    //UART_Debug_PutString("Timer_Intr2_Callback\r\n");
	for(uint8 i = 0; i < intr2_counter; i++)
		Port2_Job(intr2[i]);
	intr2_counter = 0;
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
    
    memset(intr1, '\0', sizeof(intr1));
    CyWdtClear();
    
    memset(intr2, '\0', sizeof(intr2));
    CyWdtClear();
    
    memset(_QBuffer_Queue_Main, '\0', Queue_Size_Main);
    CyWdtClear();

    memset(_QBuffer_Queue_Send, '\0', Queue_Size_Send);
	CyWdtClear();
    
    return;
}

void timers_create()
{
    // Create without start.
    // Create without start.
    OS_TIMER_Create(&Timer_Read_Linux, Timer_Read_Linux_Callback, 10);
    CyWdtClear();

    OS_TIMER_Create(&Timer_RGB_LED, Timer_RGB_LED_Callback, 1000);
    CyWdtClear();

    OS_TIMER_Create(&Timer_Read_W_Port1, Timer_Read_W_Port1_Callback, 150);
    CyWdtClear();
    OS_TIMER_Create(&Timer_Read_W_Port2, Timer_Read_W_Port2_Callback, 150);
    CyWdtClear();

    OS_TIMER_Create(&Timer_AfterRead_In1, Timer_AfterRead_Input1_Callback, 10);
    CyWdtClear();
    OS_TIMER_Create(&Timer_AfterRead_Port1, Timer_AfterRead_Port1_Callback, 10);
    CyWdtClear();

    OS_TIMER_Create(&Timer_AfterRead_In2, Timer_AfterRead_Input2_Callback, 10);
    CyWdtClear();
    OS_TIMER_Create(&Timer_AfterRead_Port2, Timer_AfterRead_Port2_Callback, 10);
    CyWdtClear();

    OS_TIMER_Create(&Timer_WD, Timer_WD_Callback, 100);
    CyWdtClear();

    OS_TIMER_Create(&Timer_Inps, Timer_Inps_Callback, 50);
    CyWdtClear();
    
    OS_TIMER_Create(&Timer_intr1, Timer_Intr1_Callback, 200);
    CyWdtClear();
    
    OS_TIMER_Create(&Timer_intr2, Timer_Intr2_Callback, 200);
    CyWdtClear();
    
    return;
}

void Task_Main()
{
	while(1){
		Proc_Task_main();
		CyWdtClear();
		OS_TASK_Delay(TASK_DELAY_Main_MS);
	}

	return;
}

void Proc_Task_main()
{

	void* ret_send = NULL;
	ret_send = q_get_message(&_Queue_Send);
	while(ret_send){
		CyWdtClear();
		UART_Linux_PutArrayConst((uint8*)ret_send, strlen((char*)ret_send) + 1);
		if(memcmp((char*)ret_send, "{FOT_REQ}:", 10) != 0){
			UART_Debug_PutString("Proc_Task_main -> send: ");
			UART_Debug_PutString((char*)ret_send);
			UART_Debug_PutString("\r\n");
		}
		OS_QUEUE_Purge(&_Queue_Send);

		OS_TASK_Delay(TASK_DELAY_Main_MS);
		ret_send = NULL;
		ret_send = q_get_message(&_Queue_Send);
	}

	void* ret = NULL;
	ret = q_get_message(&_Queue_Main);
	while(ret){
		CyWdtClear();
		CmdTask((char*)ret);
		OS_QUEUE_Purge(&_Queue_Main);
		OS_TASK_Delay(TASK_DELAY_Main_MS);
		ret = NULL;
		ret = q_get_message(&_Queue_Main);
	}
	return;
}

/*********************************************************************
*
*       main()
*/
int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    
    
    OS_Init();    // Initialize embOS
    OS_InitHW();  // Initialize required hardware
    BSP_Init();   // Initialize LED ports
    
    RGB_Color = 0;
    RGB_Color_Old = 0;
    RGB(RGB_Color); //Cypress run
    
    //CyDelay(1000);
    
    intrrupt_init(); 
    timers_create();
    init_vars();
    
    UART_Debug_PutString("********************************* Start Cy IO *********************************\r\n"); 
    cymonitoring_get_lastWtd();
    UART_Debug_PutString("********************************* Brd V1.0 App V2.0 (and-inps-fota) *********************************\r\n");
    
    
    OS_TASK_Create(&TCB_Task_Main, "Task_Main", 400, Task_Main, Stack_Task_Main, sizeof(Stack_Task_Main), 2);
    
    OS_Q_Create(&_Queue_Main, &_QBuffer_Queue_Main, sizeof(_QBuffer_Queue_Main));
	OS_Q_Create(&_Queue_Send, &_QBuffer_Queue_Send, sizeof(_QBuffer_Queue_Send));
    
    RGB_Color = 1;
    RGB_Color_Old = 1;
    RGB(RGB_Color); //Cypress run
    
    CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE); 
    
    CyWdtClear();
    Timer_WD_Callback();
    
    CyWdtClear();
    Timer_Inps_Callback();
    
    
    UART_Linux_ClearRxBuffer();
    UART_Linux_ClearTxBuffer();
    
    OS_Start();   // Start embOS
    
    return 0;
}
/*************************** End of file ****************************/
