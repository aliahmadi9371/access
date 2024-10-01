/*
 * defines.c
 *
 *  Created on: Jan 20, 2024
 *      Author: dasha
 */
// ###################################################### Includes #########################################################
#include "defines.h"
//#include "iwdg.h"
// #########################################################################################################################
// ---------------------------- FOTA Variables ----------------------------

uint8_t fota_flag_start = 0;

uint16_t fota_file_numbers = 1;
uint16_t fota_row_counter = 0;
uint16_t fota_row_sequence = 0;
uint16_t fota_recv_byte_count = 0;

uint32_t server_fota_App_size = 0;
uint32_t server_fota_App_CRC = 0;
char server_fota_folder_name[15];
uint16_t server_fota_file_numbers = 0;
uint16_t server_fota_App_lines = 0;
char server_fota_data[1500];

uint8_t int_byte = 0;
uint8_t int_byte_ctr = 0;

uint8_t RGB_Color = 0;
uint8_t RGB_Color_Old = 0;

OS_Q _Queue_Main;
char _QBuffer_Queue_Main[Queue_Size_Main];

OS_Q _Queue_Send;
char _QBuffer_Queue_Send[Queue_Size_Send];

uint8_t q_put_message(OS_QUEUE* queue, char* message, int lng)
{
    HAL_IWDG_Refresh(&hiwdg);

    int res = OS_QUEUE_Put(queue, (void*)message, lng);
    if(res == 0){
        return 1;
    }
    else{
//      char msg[] = "q_put_message >> failed to put message";
//      file_fill_log_buff(msg);

        return 0;
    }
}

void* q_get_message(OS_QUEUE* queue)
{
    void* q_output = NULL;// must be static to return pointer!!!
    if(OS_QUEUE_GetPtr(queue, (void**)(&q_output))){
//        OS_QUEUE_Purge(queue);
    }
    else{
        q_output = NULL;
    }

    return q_output;
}
// ###################################################### Functions ########################################################
uint8_t df_hexstring_hex(char character)
{
    uint8_t data = 255;

    if(
        (character == '0') ||
        (character == '1') ||
        (character == '2') ||
        (character == '3') ||
        (character == '4') ||
        (character == '5') ||
        (character == '6') ||
        (character == '7') ||
        (character == '8') ||
        (character == '9') ||
        (character == 'a') ||
        (character == 'A') ||
        (character == 'b') ||
        (character == 'B') ||
        (character == 'c') ||
        (character == 'C') ||
        (character == 'd') ||
        (character == 'D') ||
        (character == 'e') ||
        (character == 'E') ||
        (character == 'f') ||
        (character == 'F')
    ){
        switch(character){
            case '0':
                data = 0;
                break;
            case '1':
                data = 1;
                break;
            case '2':
                data = 2;
                break;
            case '3':
                data = 3;
                break;
            case '4':
                data = 4;
                break;
            case '5':
                data = 5;
                break;
            case '6':
                data = 6;
                break;
            case '7':
                data = 7;
                break;
            case '8':
                data = 8;
                break;
            case '9':
                data = 9;
                break;
            case 'a':
                data = 10;
                break;
            case 'A':
                data = 10;
                break;
            case 'b':
                data = 11;
                break;
            case 'B':
                data = 11;
                break;
            case 'c':
                data = 12;
                break;
            case 'C':
                data = 12;
                break;
            case 'd':
                data = 13;
                break;
            case 'D':
                data = 13;
                break;
            case 'e':
                data = 14;
                break;
            case 'E':
                data = 14;
                break;
            case 'f':
                data = 15;
                break;
            case 'F':
                data = 15;
                break;
            default:
                break;
        }
    }
    return data;
}

char df_hex_hexstring(uint8_t data)
{
    char character = 0;


    if((data >= 0) && (data <= 15)){
        switch(data){
            case 0:
                character = '0';
                break;
            case 1:
                character = '1';
                break;
            case 2:
                character = '2';
                break;
            case 3:
                character = '3';
                break;
            case 4:
                character = '4';
                break;
            case 5:
                character = '5';
                break;
            case 6:
                character = '6';
                break;
            case 7:
                data = '7';
                break;
            case 8:
                character = '8';
                break;
            case 9:
                character = '9';
                break;
            case 10:
                character = 'A';
                break;
            case 11:
                character = 'B';
                break;
            case 12:
                character = 'C';
                break;
            case 13:
                character = 'D';
                break;
            case 14:
                character = 'E';
                break;
            case 15:
                character = 'F';
                break;
            default:
                break;
        }
    }

    return character;
}

char* strremove(char *str, const char *sub)
{
    size_t len = strlen(sub);
    if(len > 0){
        char *p = str;
        while((p = strstr(p, sub)) != NULL){
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }

    return str;
}

void byte_to_binary(uint8_t byte, char* str_binary)
{
	uint8_t i;
    str_binary[8] = '\0';

    uint8_t j = 0;
    for(i = 0x80; i!= 0; i >>= 1){
        str_binary[j] = (byte & i) ? '1' : '0';
        j++;
    }

    return;
}

// SORTING ALGORITHM
void swap(uint8_t* xp, uint8_t* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
void selectionSort(uint8_t arr[], uint8_t arr_len)
{
    int i, j, min_idx;

    // One by one move boundary of
    // unsorted subarray
    for (i = 0; i < arr_len - 1; i++){
//    	HAL_IWDG_Refresh(&hiwdg);

        // Find the minimum element in
        // unsorted array
        min_idx = i;
        for (j = i + 1; j < arr_len; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}

// #########################################################################################################################
/* [] END OF FILE */

