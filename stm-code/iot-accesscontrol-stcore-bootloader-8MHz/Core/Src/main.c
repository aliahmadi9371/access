/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <core_cm3.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FOTA_DONE                          	2
#define FOTA_START                          1
#define FOTA_STOP                           0
#define FOTA_ROW_SIZE                       42 // ??????????????????????????
// external e2prom
#define EE2_1_CHIP_ADDR                     0x56 << 1
#define EE2_2_CHIP_ADDR                     0x50


#define	EE2_METADATA_START_ADDRESS			0
#define	IE2_Loc_FOTA_ROW_START				0x08040400

#define EE2_Loc_Reset_Status             	(EE2_METADATA_START_ADDRESS + 38)
#define EE2_Loc_Reset_Code_Address         	(EE2_METADATA_START_ADDRESS + 39)

// FOTA METADATA
#define EE2_Loc_FOTA_STATUS                 (EE2_METADATA_START_ADDRESS + 20)
#define EE2_Loc_FOTA_CRC_BYTE0              (EE2_METADATA_START_ADDRESS + 21)
#define EE2_Loc_FOTA_CRC_BYTE1              (EE2_METADATA_START_ADDRESS + 22)
#define EE2_Loc_FOTA_CRC_BYTE2              (EE2_METADATA_START_ADDRESS + 23)
#define EE2_Loc_FOTA_CRC_BYTE3              (EE2_METADATA_START_ADDRESS + 24)
#define EE2_Loc_FOTA_App_SIZE_BYTE0         (EE2_METADATA_START_ADDRESS + 25)
#define EE2_Loc_FOTA_App_SIZE_BYTE1         (EE2_METADATA_START_ADDRESS + 26)
#define EE2_Loc_FOTA_App_SIZE_BYTE2         (EE2_METADATA_START_ADDRESS + 27)
#define EE2_Loc_FOTA_App_SIZE_BYTE3         (EE2_METADATA_START_ADDRESS + 28)
#define EE2_Loc_FOTA_ROW_START              (EE2_METADATA_START_ADDRESS + 29)



#define MAX_BLOCK_SIZE          			( 1024 )                  //1KB
#define ETX_APP_START_ADDRESS   			0x08008800	// ORIGINAL
//#define ETX_APP_START_ADDRESS   			0x08040400	// TEST


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t BL_Version[2] = { MAJOR, MINOR };
uint32_t App_Size = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void update_firmware();
static void jump_to_app(uint8_t flag);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void usb_debug_print(char* msg, uint8_t state)
{
	HAL_UART_Transmit(&huart3,(const uint8_t*)(msg), strlen((const char*)(msg)), 500);

	return;
}

HAL_StatusTypeDef ee2_read_byte(uint8_t chipAddr, uint8_t* byte, uint16_t addr)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t i = 0;

	while(status != HAL_OK && i < 1000){
		status = HAL_I2C_Mem_Read(&hi2c1, chipAddr, addr, 2, byte, 1, 500);
		if(status != HAL_OK){
			usb_debug_print("ee2_readByte: HAL_I2C_Mem_Read: status != HAL_OK\r\n", 0);
		}
		i++;
//		HAL_IWDG_Refresh(&hiwdg);
	}

    return status;
}

HAL_StatusTypeDef ee2_write_byte(uint8_t chipAddr, uint8_t byte, uint16_t addr)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t i = 0;

	while(status != HAL_OK && i < 200){
		status = HAL_I2C_IsDeviceReady(&hi2c1, chipAddr, 0x00000001, 500);

		if(status == HAL_OK){
			status = HAL_ERROR;
			status = HAL_I2C_Mem_Write(&hi2c1, chipAddr, addr, 2, &byte, 1, 500);  // write the data to the EEPROM
			if(status != HAL_OK){
				usb_debug_print("ee2_writeByte:  HAL_I2C_Mem_Write: status != HAL_OK\r\n", 0);
			}
			HAL_Delay(5);   // delay for writing data
		}

		i++;
//		HAL_IWDG_Refresh(&hiwdg);
	}

    return status;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! use INTERNAL FLASH as Internal EEPROM
HAL_StatusTypeDef internal_flash_write_WORD(uint32_t Flash_Address, uint32_t data){
	HAL_StatusTypeDef internal_flash_ret = HAL_ERROR;
	internal_flash_ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_Address, (uint64_t)data);

    return internal_flash_ret;
}

uint16_t internal_flash_read_HALFWORD(uint32_t Flash_Address){
	__IO uint32_t read_data = *(__IO uint32_t *)Flash_Address;

	return (uint16_t)read_data;
}

// SOURCE: https://stackoverflow.com/questions/34196663/stm32-how-to-get-last-reset-status
typedef enum reset_cause_e
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET
    //RESET_CAUSE_BROWNOUT_RESET,
} reset_cause_t;

/// @brief      Obtain the STM32 system reset cause
/// @param      None
/// @return     The system reset cause
reset_cause_t reset_cause_get(void)
{
    reset_cause_t reset_cause;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reset_cause = RESET_CAUSE_LOW_POWER_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        reset_cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        reset_cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        // This reset is induced by calling the ARM CMSIS
        // `NVIC_SystemReset()` function!
        reset_cause = RESET_CAUSE_SOFTWARE_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        reset_cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
    // Needs to come *after* checking the `RCC_FLAG_PORRST` flag in order to
    // ensure first that the reset cause is NOT a POR/PDR reset. See note
    // below.
//    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
//    {
//        reset_cause = RESET_CAUSE_BROWNOUT_RESET;
//    }
    else
    {
        reset_cause = RESET_CAUSE_UNKNOWN;
    }

    // Clear all the reset flags or else they will remain set during future
    // resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return reset_cause;
}

// Note: any of the STM32 Hardware Abstraction Layer (HAL) Reset and Clock
// Controller (RCC) header files, such as
// "STM32Cube_FW_F7_V1.12.0/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_rcc.h",
// "STM32Cube_FW_F2_V1.7.0/Drivers/STM32F2xx_HAL_Driver/Inc/stm32f2xx_hal_rcc.h",
// etc., indicate that the brownout flag, `RCC_FLAG_BORRST`, will be set in
// the event of a "POR/PDR or BOR reset". This means that a Power-On Reset
// (POR), Power-Down Reset (PDR), OR Brownout Reset (BOR) will trip this flag.
// See the doxygen just above their definition for the
// `__HAL_RCC_GET_FLAG()` macro to see this:
//      "@arg RCC_FLAG_BORRST: POR/PDR or BOR reset." <== indicates the Brownout
//      Reset flag will *also* be set in the event of a POR/PDR.
// Therefore, you must check the Brownout Reset flag, `RCC_FLAG_BORRST`, *after*
// first checking the `RCC_FLAG_PORRST` flag in order to ensure first that the
// reset cause is NOT a POR/PDR reset.


/// @brief      Obtain the system reset cause as an ASCII-printable name string
///             from a reset cause type
/// @param[in]  reset_cause     The previously-obtained system reset cause
/// @return     A null-terminated ASCII name string describing the system
///             reset cause
uint8_t cm_reset_flag = 0;
const char * reset_cause_get_name(reset_cause_t reset_cause)
{
    const char * reset_cause_name = "TBD";

    switch (reset_cause)
    {
        case RESET_CAUSE_UNKNOWN:
            reset_cause_name = "UNKNOWN";
            cm_reset_flag = 0;
            break;
        case RESET_CAUSE_LOW_POWER_RESET:
            reset_cause_name = "LOW_POWER_RESET";
            cm_reset_flag = 1;
            break;
        case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
            reset_cause_name = "WINDOW_WATCHDOG_RESET";
            cm_reset_flag = 2;
            break;
        case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
            reset_cause_name = "INDEPENDENT_WATCHDOG_RESET";
            cm_reset_flag = 3;
            break;
        case RESET_CAUSE_SOFTWARE_RESET:
            reset_cause_name = "SOFTWARE_RESET";
            cm_reset_flag = 4;
            break;
        case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
            reset_cause_name = "POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
            cm_reset_flag = 5;
            break;
        case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
            reset_cause_name = "EXTERNAL_RESET_PIN_RESET";
            cm_reset_flag = 6;
            break;
//        case RESET_CAUSE_BROWNOUT_RESET:
//            reset_cause_name = "BROWNOUT_RESET (BOR)";
//            cm_reset_flag = 7;
//            break;
    }

    return reset_cause_name;
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! use INTERNAL FLASH as Internal EEPROM


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
//  __HAL_AFIO_REMAP_SWJ_NOJTAG(); // FYI : fix inp_2 problem
  __enable_irq();

//  HAL_Delay(5);
  usb_debug_print("\r\n##################################### START BOOTLOADER 57600 #####################################\r\n", 0);

//  reset_cause_t reset_cause = reset_cause_get();
//  char buf[64];
//  sprintf(buf, "CHIP RESET CAUSE >> ");
//  strcat(buf, "\"");
//  strcat(buf, reset_cause_get_name(reset_cause));
//  strcat(buf, "\"\r\n");
//  usb_debug_print(buf, 0);
////
//  ee2_write_byte(EE2_1_CHIP_ADDR, cm_reset_flag, EE2_Loc_Reset_Status);
////
  update_firmware();
//  jump_to_app(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  usb_debug_print(">> BootLoader failed >>\r\n", 0);
	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void update_firmware()
{
//	ee2_writeByte(EE2_1_CHIP_ADDR, FOTA_START, EE2_Loc_FOTA_STATUS);
	/* ***************************** CHECK METADATA FROM External EEPROM ***************************** */
	uint8_t byte = 0;
	HAL_StatusTypeDef status = ee2_read_byte(EE2_1_CHIP_ADDR, &byte, EE2_Loc_FOTA_STATUS);


	uint8_t byte1 = 0;
	uint8_t byte2 = 0;
	uint8_t byte3 = 0;
	uint8_t byte4 = 0;

	HAL_StatusTypeDef s1 = ee2_read_byte(EE2_1_CHIP_ADDR, &byte1, EE2_Loc_FOTA_App_SIZE_BYTE3);
	HAL_StatusTypeDef s2 = ee2_read_byte(EE2_1_CHIP_ADDR, &byte2, EE2_Loc_FOTA_App_SIZE_BYTE2);
	HAL_StatusTypeDef s3 = ee2_read_byte(EE2_1_CHIP_ADDR, &byte3, EE2_Loc_FOTA_App_SIZE_BYTE1);
	HAL_StatusTypeDef s4 = ee2_read_byte(EE2_1_CHIP_ADDR, &byte4, EE2_Loc_FOTA_App_SIZE_BYTE0);

	unsigned int appSize = (uint32_t)byte1 << 24 | (uint32_t)byte2 << 16 | (uint32_t)byte3 << 8 | (uint32_t)byte4 << 0;
	/* ************************************************************************************************ */

	uint8_t fail = 0;

	if(status == HAL_OK && byte == FOTA_START && s1 == HAL_OK && s2 == HAL_OK && s3 == HAL_OK && s4 == HAL_OK){
		HAL_StatusTypeDef ret = HAL_FLASH_Unlock();
		if(ret != HAL_OK){
			usb_debug_print("############################ HAL_FLASH_Unlock >> FAILED! Please try again later...\r\n", 0);
			jump_to_app(0);
		}
		else{
			usb_debug_print("	*****	HAL_FLASH_Unlock >> OK	*****	\r\n", 0);

			usb_debug_print("	*****	Erasing Flash Memory...	*****	\r\n", 0);
			//Erase the Flash
			FLASH_EraseInitTypeDef EraseInitStruct;
			uint32_t SectorError;

			EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.PageAddress   = ETX_APP_START_ADDRESS;
			EraseInitStruct.NbPages       = 110;                     // from 0x08008800 to 0x08040000 (2k Pages)

			ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
			if( ret != HAL_OK ){
				usb_debug_print("	*****	Erasing Flash FAILED!	*****	\r\n", 0);
				jump_to_app(0);
			}
			else {
				usb_debug_print("	*****	Erasing Flash Done!	*****	\r\n", 0);
			}

			char buf[100];
			sprintf(buf, ">> Application size: %d\r\n", appSize);
			usb_debug_print(buf, 0);

			int flash_write_progress = 0;
			int percent = 0;
			int former_percent = 0;

			for (int var = 0; var < appSize; var += 4) {

				uint32_t four_bytes = internal_flash_read_HALFWORD(IE2_Loc_FOTA_ROW_START + ( var + 3 )) << 24
									| internal_flash_read_HALFWORD(IE2_Loc_FOTA_ROW_START + ( var + 2 )) << 16
									| internal_flash_read_HALFWORD(IE2_Loc_FOTA_ROW_START + ( var + 1 )) << 8
									| internal_flash_read_HALFWORD(IE2_Loc_FOTA_ROW_START + ( var + 0 )) << 0;

				ret = internal_flash_write_WORD(ETX_APP_START_ADDRESS + var, four_bytes);

				flash_write_progress++;

				percent = (((float)flash_write_progress) / appSize) * 100;
				if( percent % 5 == 0 && percent != former_percent){
					former_percent = percent;
					sprintf(buf, ">> Flash Writing Progress >> %d%% \r\n", (percent * 4));
					usb_debug_print(buf, 0);
				}

				if(ret != HAL_OK){
					usb_debug_print(">> FLASH_WRITE_NOT_OK!\r\n", 0);
					fail = 1;
					break;
				}
			}

			ret = HAL_FLASH_Lock();

			if(ret != HAL_OK){
				fail = 2;
			}
			else{
				usb_debug_print("	*****	HAL_FLASH_Lock => OK	*****	\r\n", 0);
			}

			if(fail == 0){
				ee2_write_byte(EE2_1_CHIP_ADDR, FOTA_STOP, EE2_Loc_FOTA_STATUS);
				ee2_write_byte(EE2_1_CHIP_ADDR, FOTA_DONE, EE2_Loc_Reset_Code_Address);
				usb_debug_print("############################ Programming FLASH finished successfully...\r\n", 0);
				jump_to_app(1);
			}
			else if(fail == 1){
//				ee2_write_byte(EE2_1_CHIP_ADDR, FOTA_STOP, EE2_Loc_FOTA_STATUS);
				usb_debug_print("############################ Programming FLASH FAILED! Error occurred in writing into the FLASH Memory\r\n", 0);
//				jump_to_app(0);
				HAL_Delay(1000);
				NVIC_SystemReset();
			}
			else if(fail == 2){
//				ee2_write_byte(EE2_1_CHIP_ADDR, FOTA_STOP, EE2_Loc_FOTA_STATUS);
				usb_debug_print("############################ Programming FLASH FAILED! Error occurred in Locking the FLASH Memory\r\n", 0);
//				jump_to_app(0);
				HAL_Delay(1000);
				NVIC_SystemReset();
			}
		}

	}
	else{
		usb_debug_print("############################ BOOTLOADER >> No New Program to write!\r\n", 0);
		jump_to_app(0);
	}

	return;
}

static void jump_to_app(uint8_t flag)
{
	if(flag){
		usb_debug_print("############################ Jumping to NEW Application\r\n", 0);
	}
	else{
		usb_debug_print("############################ Jumping to Application\r\n", 0);
	}
	char jump_address[100];
	sprintf(jump_address, "############################ JUMP ADDRESS >> %08lX\r\n", (long unsigned int)ETX_APP_START_ADDRESS);
	usb_debug_print(jump_address, 0);
	usb_debug_print("##################################### ENDOF BOOTLOADER #####################################\r\n", 0);

//	void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*)(ETX_APP_START_ADDRESS)));
//
//	__set_MSP(*(volatile uint32_t*) ETX_APP_START_ADDRESS);
//
//	// Turn OFF the Led to tell the user that Bootloader is not running
////	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET );
//
//	app_reset_handler();    //call the app reset handler

//////////////////////////	CORRECT	////////////////////////////////////
	HAL_RCC_DeInit();

	__disable_irq();

	for (int i = 0; i < (sizeof(NVIC->ICPR) / sizeof(NVIC->ICPR[0])); i++) {
		NVIC_ClearPendingIRQ(i);
	}

	__set_CONTROL(0x00);

	typedef void (*jump_app)(void);

	volatile uint32_t *_vectable = (volatile uint32_t *)(__IO uint32_t*)(ETX_APP_START_ADDRESS);  // point _vectable to the start of the application at 0x08005000

	jump_app app_jump = (jump_app) *(_vectable + 1);   // get the address of the application's reset handler by loading the 2nd entry in the table

	SCB->VTOR = *_vectable;   // point VTOR to the start of the application's vector table

	__set_MSP(*_vectable);   // setup the initial stack pointer using the RAM address contained at the start of the vector table

	app_jump();   // call the application's reset handler


}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
