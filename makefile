CC = $(ARM_SDK_PREFIX)gcc
CP = $(ARM_SDK_PREFIX)objcopy
MCU := -mcpu=cortex-m0 -mthumb
LDSCRIPT := STM32F051K6TX_FLASH.ld
LIBS := -lc -lm -lnosys 
LDFLAGS := -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,--gc-sections -Wl,--print-memory-usage
MAIN_SRC_DIR := Src
SRC_DIR := Core/Startup \
	Core/Src \
	Drivers/STM32F0xx_HAL_Driver/Src
SRC := $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.[cs]))
OBJ := $(SRC:%.[cs]=%.o)
INCLUDES :=  \
	-ICore/Inc \
	-IDrivers/STM32F0xx_HAL_Driver/Inc \
	-IDrivers/CMSIS/Include \
	-IDrivers/CMSIS/Device/ST/STM32F0xx/Include 
VALUES :=  \
	-DUSE_MAKE \
	-DHSE_VALUE=8000000 \
	-DSTM32F051x8 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DDATA_CACHE_ENABLE=0 \
	-DINSTRUCTION_CACHE_ENABLE=0 \
	-DVDD_VALUE=3300 \
	-DLSI_VALUE=40000 \
	-DHSI_VALUE=8000000 \
	-DUSE_FULL_LL_DRIVER \
	-DPREFETCH_ENABLE=1
CFLAGS = $(MCU) $(VALUES) $(INCLUDES) -O2 -Wall -fdata-sections -ffunction-sections
CFLAGS += -DUSE_$(TARGET)
CFLAGS += -MMD -MP -MF $(@:%.bin=%.d)

ARM_SDK_PREFIX ?= arm-none-eabi-

# Targets by signal input pin:
# PB4: iFlight
# PA2: all others
TARGETS := PA2 PB4
TARGET_PREFIX := BOOTLOADER_

VERSION := $(shell grep "#define BOOTLOADER_VERSION" Core/Src/main.c | awk '{print $$3}' )

.PHONY : clean all version
all : $(TARGETS)
clean :
	rm -f Src/*.o
version :
	@echo $(VERSION)

$(TARGETS) :
	$(MAKE) TARGET=$@ $(TARGET_PREFIX)$@.bin

$(TARGETS:%=$(TARGET_PREFIX)%.bin) : clean $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_PREFIX)$(TARGET)_$(VERSION).elf $(OBJ)
	$(CP) -O binary $(TARGET_PREFIX)$(TARGET)_$(VERSION).elf $(TARGET_PREFIX)$(TARGET)_$(VERSION).bin
	$(CP) $(TARGET_PREFIX)$(TARGET)_$(VERSION).elf -O ihex  $(TARGET_PREFIX)$(TARGET)_$(VERSION).hex
