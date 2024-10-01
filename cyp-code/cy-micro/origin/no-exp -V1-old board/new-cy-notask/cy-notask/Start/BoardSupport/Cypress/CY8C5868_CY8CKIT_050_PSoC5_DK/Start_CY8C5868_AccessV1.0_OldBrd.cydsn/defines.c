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
#include "defines.h"
// #########################################################################################################################

// ###################################################### Functions ########################################################
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
// #########################################################################################################################
/* [] END OF FILE */
