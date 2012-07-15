PROJECTCFG=projectconfig

include boards/defaults
include $(PROJECTCFG)
include configparser.make

##########################################################################
# Debug settings
##########################################################################

# Set DEBUGBUILD to 'TRUE' for full debugging (larger, slower binaries), 
# or to 'FALSE' for release builds (smallest, fastest binaries)
DEBUGBUILD = FALSE

##########################################################################
# IDE Flags (Keeps various IDEs happy)
##########################################################################

OPTDEFINES = -D __NEWLIB__

##########################################################################
# Project-specific files 
##########################################################################

VPATH += project

##########################################################################
# Optional driver files 
##########################################################################

# ISL12022M RTC
VPATH += drivers/rtc/isl12022m
OBJS += isl12022m.o

# DAC
VPATH += drivers/dac/mcp4725 drivers/dac/mcp4901
OBJS += mcp4725.o mcp4901.o

# RFID/NFC
VPATH += drivers/rf/pn532 drivers/rf/pn532/helpers
OBJS += pn532.o pn532_bus_i2c.o pn532_bus_uart.o
OBJS += pn532_mifare_classic.o pn532_mifare_ultralight.o

# TAOS Light Sensors
VPATH += drivers/sensors/tcs3414 drivers/sensors/tsl2561
OBJS += tcs3414.o tsl2561.o

# SPI Flash
VPATH += drivers/storage/spiflash/w25q16bv
OBJS += w25q16bv.o

# FM Radio
VPATH += drivers/audio/tea5767
OBJS += tea5767.o

# IN219 Current Sensor
VPATH += drivers/sensors/ina219
OBJS += ina219.o

# MPL115A2 Barometric Pressure Sensor
VPATH += drivers/sensors/mpl115a2
OBJS += mpl115a2.o

##########################################################################
# Library files 
##########################################################################

VPATH += core core/adc core/cmd core/cpu core/gpio core/i2c core/pmu
VPATH += core/ssp core/systick core/timer16 core/timer32 core/uart
VPATH += core/wdt core/iap
VPATH += core/libc
OBJS += stdio.o string.o
OBJS += adc.o cpu.o gpio.o i2c.o pmu.o ssp.o systick.o timer16.o
OBJS += timer32.o uart.o uart_buf.o
OBJS += wdt.o
OBJS += sysinit.o iap.o

##########################################################################
# GNU GCC compiler prefix and location
##########################################################################

CROSS_COMPILE = arm-none-eabi-
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
LPCRC = ./lpcrc

##########################################################################
# GNU GCC compiler flags
##########################################################################
ROOT_PATH = .
INCLUDE_PATHS = -I$(ROOT_PATH) -I$(ROOT_PATH)/project

##########################################################################
# Startup files
##########################################################################

LD_PATH = lpc1xxx
LD_SCRIPT = $(LD_PATH)/linkscript.ld
LD_TEMP = $(LD_PATH)/memory.ld

ifeq (LPC11xx,$(TARGET))
  CORTEX_TYPE=m0
else
  CORTEX_TYPE=m3
endif

CPU_TYPE = cortex-$(CORTEX_TYPE)
VPATH += lpc1xxx
OBJS += $(TARGET)_handlers.o LPC1xxx_startup.o

##########################################################################
# Compiler settings, parameters and flags
##########################################################################
ifeq (TRUE,$(DEBUGBUILD))
  CFLAGS  = -g -O0
  ASFLAGS = -g -O0
else
  CFLAGS  = -g -Os
  ASFLAGS = -g -Os
endif


xCFLAGS  = -c $(CFLAGS) $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin $(OPTDEFINES) $(DEFS)
xASFLAGS = -c $(ASFLAGS) $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
LDFLAGS = -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections
LDLIBS  = -lm
OCFLAGS = --strip-unneeded

all: $(OUTFILE)

%.o : %.c
	@echo CC $<
	@$(CC) $(xCFLAGS) -o $@ $<

%.o : %.s
	@echo AS $<
	@$(AS) $(xASFLAGS) -o $@ $<

$(OUTFILE): $(OBJS) $(SYS_OBJS)
	-@echo "MEMORY" > $(LD_TEMP)
	-@echo "{" >> $(LD_TEMP)
	-@echo "  flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH)" >> $(LD_TEMP)
	-@echo "  sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB)" >> $(LD_TEMP)
	-@echo "}" >> $(LD_TEMP)
	-@echo "INCLUDE $(LD_SCRIPT)" >> $(LD_TEMP)
	$(LD) $(LDFLAGS) -T $(LD_TEMP) -o $(OUTFILE).elf $(OBJS) $(LDLIBS)
	-@echo ""
	$(SIZE) $(OUTFILE).elf
	-@echo ""
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUTFILE).elf $(OUTFILE).hex
	-@echo ""
	$(LPCRC) $(OUTFILE).bin

clean:
	rm -f $(OBJS) $(LD_TEMP) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex
