##########################################################################
# User configuration and firmware specific object files	
##########################################################################

# The target, flash and ram of the LPC1xxx microprocessor.
# Use for the target the value: LPC11xx, LPC13xx or LPC17xx
TARGET = LPC13xx
FLASH = 32K
SRAM = 8K

# For USB HID support the LPC134x reserves 384 bytes from the sram,
# if you don't want to use the USB features, just use 0 here.
SRAM_USB = 384

VPATH = 
OBJS = main.o

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
OBJS += commands.o

VPATH += project/commands
OBJS += cmd_chibi_addr.o cmd_chibi_tx.o
OBJS += cmd_i2ceeprom_read.o cmd_i2ceeprom_write.o cmd_lm75b_gettemp.o
OBJS += cmd_reset.o cmd_sd_dir.o cmd_sysinfo.o cmd_uart.o 
OBJS += cmd_roundedcorner.o cmd_pwm.o
OBJS += cmd_adc.o cmd_ssp.o

VPATH += project/commands/drawing
OBJS += cmd_backlight.o cmd_bmp.o cmd_button.o cmd_calibrate.o
OBJS += cmd_circle.o cmd_clear.o cmd_line.o cmd_orientation.o
OBJS += cmd_pixel.o cmd_progress.o cmd_rectangle.o cmd_text.o
OBJS += cmd_textw.o cmd_tsthreshhold.o cmd_tswait.o cmd_triangle.o

##########################################################################
# Optional driver files 
##########################################################################

# Chibi Light-Weight Wireless Stack (AT86RF212)
VPATH += drivers/rf/chibi
OBJS += chb.o chb_buf.o chb_drvr.o chb_eeprom.o chb_spi.o

# 4K EEPROM
VPATH += drivers/storage/eeprom drivers/storage/eeprom/mcp24aa
OBJS += eeprom.o mcp24aa.o

# LM75B temperature sensor
VPATH += drivers/sensors/lm75b
OBJS += lm75b.o

VPATH += drivers/sensors/ds18b20
OBJS += ds18b20.o

# ISL12022M RTC
VPATH += drivers/rtc/isl12022m
OBJS += isl12022m.o

# TFT LCD support
VPATH += drivers/displays/tft drivers/displays/tft/hw 
OBJS += drawing.o touchscreen.o colors.o theme.o bmp.o

# GUI Controls
VPATH += drivers/displays/tft/controls
OBJS += button.o hsbchart.o huechart.o label.o
OBJS += labelcentered.o progressbar.o

# Bitmap (non-AA) fonts
VPATH += drivers/displays/tft/fonts
OBJS += fonts.o 
OBJS += dejavusans9.o dejavusansbold9.o dejavusanscondensed9.o
OBJS += dejavusansmono8.o dejavusansmonobold8.o
OBJS += verdana9.o verdana14.o verdanabold14.o 

# Anti-aliased fonts
VPATH += drivers/displays/tft/aafonts/aa2 drivers/displays/tft/aafonts/aa4
OBJS += aafonts.o 
OBJS += DejaVuSansCondensed14_AA2.o DejaVuSansCondensedBold14_AA2.o
OBJS += DejaVuSansMono10_AA2.o DejaVuSansMono13_AA2.o DejaVuSansMono14_AA2.o

# LCD Driver (Only one can be included at a time!)
# OBJS += hx8340b.o
# OBJS += hx8347d.o
OBJS += ILI9328.o
# OBJS += ILI9325.o
# OBJS += ssd1331.o
# OBJS += ssd1351.o
# OBJS += st7735.o
# OBJS += st7783.o

# Bitmap/Monochrome LCD support (ST7565, SSD1306, etc.)
VPATH += drivers/displays
VPATH += drivers/displays/bitmap/sharpmem
VPATH += drivers/displays/bitmap/st7565
VPATH += drivers/displays/bitmap/ssd1306
OBJS += smallfonts.o sharpmem.o st7565.o ssd1306.o

#Character Displays (VFD text displays, etc.)
VPATH += drivers/displays/character/samsung_20T202DA2JA
OBJS += samsung_20T202DA2JA.o

# ChaN FatFS and SD card support
VPATH += drivers/fatfs
OBJS += ff.o mmc.o

# Motors
VPATH += drivers/motor/stepper
OBJS += stepper.o

# RSA Encryption/Descryption
VPATH += drivers/rsa
OBJS += rsa.o

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

# ADS1015 12-bit ADC
VPATH += drivers/adc/ads1015
OBJS += ads1015.o

##########################################################################
# Library files 
##########################################################################

VPATH += core core/adc core/cmd core/cpu core/gpio core/i2c core/pmu
VPATH += core/ssp core/systick core/timer16 core/timer32 core/uart
VPATH += core/usbhid-rom core/wdt core/usbcdc core/pwm core/iap
VPATH += core/libc
OBJS += stdio.o string.o
OBJS += adc.o cpu.o cmd.o gpio.o i2c.o pmu.o ssp.o systick.o timer16.o
OBJS += timer32.o uart.o uart_buf.o usbconfig.o usbhid.o
OBJS += wdt.o cdcuser.o cdc_buf.o usbcore.o usbdesc.o usbhw.o usbuser.o 
OBJS += sysinit.o pwm.o iap.o

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
OUTFILE = firmware
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
  CFLAGS  = -c -g -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin $(OPTDEFINES)
  ASFLAGS = -c -g -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
else
  CFLAGS  = -c -g -Os $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin $(OPTDEFINES)
  ASFLAGS = -c -g -Os $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
endif

LDFLAGS = -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections
LDLIBS  = -lm
OCFLAGS = --strip-unneeded

all: firmware

%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $<

firmware: $(OBJS) $(SYS_OBJS)
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
	$(LPCRC) firmware.bin

clean:
	rm -f $(OBJS) $(LD_TEMP) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex
