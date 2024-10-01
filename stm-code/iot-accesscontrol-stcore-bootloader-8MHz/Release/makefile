################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := iot-accesscontrol-stcore-bootloader-8MHz
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
iot-accesscontrol-stcore-bootloader-8MHz.elf \

MAP_FILES += \
iot-accesscontrol-stcore-bootloader-8MHz.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
iot-accesscontrol-stcore-bootloader-8MHz.list \

OBJCOPY_HEX += \
iot-accesscontrol-stcore-bootloader-8MHz.hex \


# All Target
all: main-build

# Main-build Target
main-build: iot-accesscontrol-stcore-bootloader-8MHz.elf secondary-outputs

# Tool invocations
iot-accesscontrol-stcore-bootloader-8MHz.elf iot-accesscontrol-stcore-bootloader-8MHz.map: $(OBJS) $(USER_OBJS) C:\Users\ali\Desktop\my\ files\my\ work\prg\iroteam-tolue\stm\iot-accesscontrol-stcore-bootloader-8MHz\STM32F103RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "iot-accesscontrol-stcore-bootloader-8MHz.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\ali\Desktop\my files\my work\prg\iroteam-tolue\stm\iot-accesscontrol-stcore-bootloader-8MHz\STM32F103RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="iot-accesscontrol-stcore-bootloader-8MHz.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

iot-accesscontrol-stcore-bootloader-8MHz.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "iot-accesscontrol-stcore-bootloader-8MHz.list"
	@echo 'Finished building: $@'
	@echo ' '

iot-accesscontrol-stcore-bootloader-8MHz.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "iot-accesscontrol-stcore-bootloader-8MHz.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout iot-accesscontrol-stcore-bootloader-8MHz.elf iot-accesscontrol-stcore-bootloader-8MHz.hex iot-accesscontrol-stcore-bootloader-8MHz.list iot-accesscontrol-stcore-bootloader-8MHz.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
