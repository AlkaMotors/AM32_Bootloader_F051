################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/bootloader.c \
../Core/Src/main.c \
../Core/Src/stm32f0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f0xx.c 

OBJS += \
./Core/Src/bootloader.o \
./Core/Src/main.o \
./Core/Src/stm32f0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f0xx.o 

C_DEPS += \
./Core/Src/bootloader.d \
./Core/Src/main.d \
./Core/Src/stm32f0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/bootloader.o: ../Core/Src/bootloader.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/bootloader.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/main.o: ../Core/Src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/stm32f0xx_it.o: ../Core/Src/stm32f0xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/stm32f0xx_it.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/syscalls.o: ../Core/Src/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/syscalls.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/sysmem.o: ../Core/Src/sysmem.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/sysmem.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/system_stm32f0xx.o: ../Core/Src/system_stm32f0xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 '-DHSE_VALUE=8000000' -DSTM32F051x8 '-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000' '-DLSE_VALUE=32768' '-DDATA_CACHE_ENABLE=0' '-DINSTRUCTION_CACHE_ENABLE=0' '-DVDD_VALUE=3300' '-DLSI_VALUE=40000' '-DHSI_VALUE=8000000' -DUSE_FULL_LL_DRIVER '-DPREFETCH_ENABLE=1' -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/system_stm32f0xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

