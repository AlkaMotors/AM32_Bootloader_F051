################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Serial_software.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Serial_software.list \

OBJCOPY_HEX += \
Serial_software.hex \

OBJCOPY_BIN += \
Serial_software.bin \


# All Target
all: Serial_software.elf secondary-outputs

# Tool invocations
Serial_software.elf: $(OBJS) $(USER_OBJS) C:\Users\Alka\STM32CubeIDE\workspace_1.0.2\Serial_software\STM32F051K6TX_FLASH.ld
	arm-none-eabi-gcc -o "Serial_software.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\Alka\STM32CubeIDE\workspace_1.0.2\Serial_software\STM32F051K6TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Serial_software.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Serial_software.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Serial_software.list"
	@echo 'Finished building: $@'
	@echo ' '

Serial_software.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Serial_software.hex"
	@echo 'Finished building: $@'
	@echo ' '

Serial_software.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Serial_software.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
