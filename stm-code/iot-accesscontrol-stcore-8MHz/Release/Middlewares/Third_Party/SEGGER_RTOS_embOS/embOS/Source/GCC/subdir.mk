################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.c 

OBJS += \
./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.o 

C_DEPS += \
./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/%.o Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/%.su Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/%.cyclo: ../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/%.c Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../embOS -I../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Include -I../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Include/GCC -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-SEGGER_RTOS_embOS-2f-embOS-2f-Source-2f-GCC

clean-Middlewares-2f-Third_Party-2f-SEGGER_RTOS_embOS-2f-embOS-2f-Source-2f-GCC:
	-$(RM) ./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.cyclo ./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.d ./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.o ./Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-SEGGER_RTOS_embOS-2f-embOS-2f-Source-2f-GCC

