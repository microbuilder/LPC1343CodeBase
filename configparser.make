DEFS += -DCFG_CPU_CCLK='(${CFG_CPU_CCLK})'
DEFS += -DCFG_SYSTICK_DELAY_IN_MS='(${CFG_SYSTICK_DELAY_IN_MS})'
DEFS += -DCFG_FIRMWARE_VERSION_MAJOR='(${CFG_FIRMWARE_VERSION_MAJOR})' -DCFG_FIRMWARE_VERSION_MINOR='(${CFG_FIRMWARE_VERSION_MINOR})' -DCFG_FIRMWARE_VERSION_REVISION='(${CFG_FIRMWARE_VERSION_REVISION})'

ifeq (${GPIO_ENABLE_IRQ0},1)
	DEFS += -DGPIO_ENABLE_IRQ0
endif
ifeq (${GPIO_ENABLE_IRQ1},1)
	DEFS += -DGPIO_ENABLE_IRQ1
endif
ifeq (${GPIO_ENABLE_IRQ2},1)
	DEFS += -DGPIO_ENABLE_IRQ2
endif
ifeq (${GPIO_ENABLE_IRQ3},1)
	DEFS += -DGPIO_ENABLE_IRQ3
endif

ifeq (${CFG_ALTRESET},1)
	DEFS += -DCFG_ALTRESET -DCFG_ALTRESET_PORT='(${CFG_ALTRESET_PORT})' -DCFG_ALTRESET_PIN='(${CFG_ALTRESET_PIN})'
endif

DEFS += -DCFG_UART_BAUDRATE='(${CFG_UART_BAUDRATE})' -DCFG_UART_BUFSIZE='(${CFG_UART_BUFSIZE})'

ifneq (${CFG_SSP0_SCKPIN},)
	DEFS += -DCFG_SSP0_SCKPIN_${CFG_SSP0_SCKPIN}
endif

ifeq (${ADC_AVERAGING_ENABLE},1)
	DEFS += -DADC_AVERAGING_ENABLE='(1)' -DADC_AVERAGING_SAMPLES='(${ADC_AVERAGING_SAMPLES})'
else
	DEFS += -DADC_AVERAGING_ENABLE='(0)'
endif

DEFS += -DCFG_LED_PORT='(${CFG_LED_PORT})' -DCFG_LED_PIN='(${CFG_LED_PIN})' -DCFG_LED_ON='(${CFG_LED_ON})' -DCFG_LED_OFF='(${CFG_LED_OFF})'

ifeq (${CFG_SDCARD},1)
	ifeq (${CFG_STEPPER},1)
$(error CFG_SDCARD and CFG_STEPPER can not be defined at the same time since they both use pin 3.0.)
	endif
	ifeq (${CFG_SSP0_SCKPIN},)
$(error CFG_SDCARD requires CFG_SSP0_SCKPIN to use SSP)
	endif
	
	DEFS += -DCFG_SDCARD -DCFG_SDCARD_READONLY='(${CFG_SDCARD_READONLY})' -DCFG_SDCARD_CDPORT='(${CFG_SDCARD_CDPORT})' -DCFG_SDCARD_CDPIN='(${CFG_SDCARD_CDPIN})'
	VPATH += drivers/fatfs
	OBJS += ff.o mmc.o
endif

SRAM_USB = 0
ifneq (${CFG_USBHID}${CFG_USBCDC},)
	DEFS += -DCFG_USB_VID='(0x${CFG_USB_VID})' -DCFG_USB_PID='(0x${CFG_USB_PID})'
	DEFS += -DCFG_USB_SOFTCONNECT='(${CFG_USB_SOFTCONNECT})'
	OBJS += usbcore.o usbdesc.o usbhw.o usbuser.o
	
	ifeq (${CFG_USBHID},1)
		DEFS += -DCFG_USBHID
		VPATH += core/usbhid-rom
		OBJS += usbconfig.o usbhid.o
		SRAM_USB = 384
		ifeq (${CFG_USBCDC},1)
$(error Only one USB class can be defined at a time (CFG_USBCDC or CFG_USBHID))
		endif
	endif
	ifeq (${CFG_USBCDC},1)
		DEFS += -DCFG_USBCDC -DCFG_USBCDC_BAUDRATE='(${CFG_USBCDC_BAUDRATE})' -DCFG_USBCDC_INITTIMEOUT='(${CFG_USBCDC_INITTIMEOUT})' -DCFG_USBCDC_BUFFERSIZE='(${CFG_USBCDC_BUFFERSIZE})'
		VPATH += core/usbcdc
		OBJS += cdcuser.o cdc_buf.o
	endif
endif

ifneq (${CFG_PRINTF_UART}${CFG_PRINTF_USBCDC},)
	DEFS += -DCFG_PRINTF_MAXSTRINGSIZE='(${CFG_PRINTF_MAXSTRINGSIZE})' -DCFG_PRINTF_NEWLINE='(${CFG_PRINTF_NEWLINE})'
	
	ifeq (${CFG_PRINTF_UART},1)
		DEFS += -DCFG_PRINTF_UART
		ifeq (${CFG_PRINTF_USBCDC},1)
$(error CFG_PRINTF_UART or CFG_PRINTF_USBCDC cannot both be defined at once)
		endif
		ifneq (${CFG_USBCDC},1)
$(error CFG_PRINTF_CDC requires CFG_USBCDC to be defined as well)
		endif
	endif
	ifeq (${CFG_PRINTF_USBCDC},1)
		DEFS += -DCFG_PRINTF_USBCDC
	endif
endif

ifeq (${CFG_INTERFACE},1)
	ifeq (${CFG_PRINTF_USBCDC},1)
		ifeq (${CFG_INTERFACE_SILENTMODE},1)
			$(warning CFG_INTERFACE_SILENTMODE typically isn't enabled with CFG_PRINTF_USBCDC)
		endif
	else
		ifeq (${CFG_PRINTF_UART},)
$(error CFG_PRINTF_UART or CFG_PRINTF_USBCDC must be defined for for CFG_INTERFACE Input/Output)
		endif
	endif
	
	DEFS += -DCFG_INTERFACE -DCFG_INTERFACE_MAXMSGSIZE='(${CFG_INTERFACE_MAXMSGSIZE})' -DCFG_INTERFACE_PROMPT='${CFG_INTERFACE_PROMPT}' -DCFG_INTERFACE_SILENTMODE='(${CFG_INTERFACE_SILENTMODE})' -DCFG_INTERFACE_DROPCR='(${CFG_INTERFACE_DROPCR})' -DCFG_INTERFACE_ENABLEIRQ='(${CFG_INTERFACE_ENABLEIRQ})' -DCFG_INTERFACE_IRQPORT='(${CFG_INTERFACE_IRQPORT})' -DCFG_INTERFACE_IRQPIN='(${CFG_INTERFACE_IRQPIN})' -DCFG_INTERFACE_LONGSYSINFO='(${CFG_INTERFACE_LONGSYSINFO})'
	ifeq (${CFG_SHORTERRORS},1)
		DEFS += -DCFG_INTERFACE_SHORTERRORS='(1)' -DCFG_INTERFACE_SHORTERRORS_UNKNOWNCOMMAND='${CFG_INTERFACE_SHORTERRORS_UNKNOWNCOMMAND}' -DCFG_INTERFACE_SHORTERRORS_TOOMANYARGS='${CFG_INTERFACE_SHORTERRORS_TOOMANYARGS}' -DCFG_INTERFACE_SHORTERRORS_TOOFEWARGS='${CFG_INTERFACE_SHORTERRORS_TOOFEWARGS}'
	else
		DEFS += -DCFG_INTERFACE_SHORTERRORS='(0)'
	endif
	ifeq (${CFG_INTERFACE_CONFIRMREADY},1)
		DEFS += -DCFG_INTERFACE_CONFIRMREADY='(1)' -DCFG_INTERFACE_CONFIRMREADY_TEXT='${CFG_INTERFACE_CONFIRMREADY_TEXT}'
	else
		DEFS += -DCFG_INTERFACE_CONFIRMREADY='(0)'
	endif
	
	OBJS += cmd.o
	OBJS += commands.o
	
	VPATH += project/commands
	OBJS += cmd_reset.o cmd_sysinfo.o cmd_uart.o cmd_roundedcorner.o
	
	ifeq (${CFG_CHIBI},1)
		OBJS += cmd_chibi_addr.o cmd_chibi_tx.o
	endif
	ifeq (${CFG_I2CEEPROM},1)
		OBJS += cmd_i2ceeprom_read.o cmd_i2ceeprom_write.o cmd_lm75b_gettemp.o
	endif
	ifeq (${CFG_SDCARD},1)
		OBJS += cmd_sd_dir.o
	endif
	ifeq (${CFG_PWM},1)
		OBJS += cmd_pwm.o
	endif
	
	VPATH += project/commands/drawing
	OBJS += cmd_backlight.o cmd_bmp.o cmd_button.o cmd_calibrate.o
	OBJS += cmd_circle.o cmd_clear.o cmd_line.o cmd_orientation.o
	OBJS += cmd_pixel.o cmd_progress.o cmd_rectangle.o cmd_text.o
	OBJS += cmd_textw.o cmd_tsthreshhold.o cmd_tswait.o cmd_triangle.o
endif

ifeq (${CFG_PWM},1)
	DEFS += -DCFG_PWM -DCFG_PWM_DEFAULT_PULSEWIDTH='(${CFG_PWM_DEFAULT_PULSEWIDTH})' -DCFG_PWM_DEFAULT_DUTYCYCLE='(${CFG_PWM_DEFAULT_DUTYCYCLE})'
	VPATH += core/pwm
	OBJS += pwm.o
endif

ifeq (${CFG_STEPPER},1)
	DEFS += -DCFG_STEPPER
	VPATH += drivers/motor/stepper
	OBJS += stepper.o
endif

ifeq (${CFG_I2CEEPROM},1)
	ifeq (${CFG_SSP0_SCKPIN},)
$(error CFG_I2CEEPROM requires CFG_SSP0_SCKPIN to use SSP)
	endif
	
	DEFS += -DCFG_I2CEEPROM -DCFG_I2CEEPROM_SIZE='(${CFG_I2CEEPROM_SIZE})'
	VPATH += drivers/storage/eeprom drivers/storage/eeprom/mcp24aa
	OBJS += eeprom.o mcp24aa.o
	
	DEFS += -DCFG_EEPROM_RESERVED='(0x${CFG_EEPROM_RESERVED})'
	DEFS += -DCFG_EEPROM_CHIBI_IEEEADDR='(uint16_t)(0x${CFG_EEPROM_CHIBI_IEEEADDR})'
	DEFS += -DCFG_EEPROM_CHIBI_SHORTADDR='(uint16_t)(0x${CFG_EEPROM_CHIBI_SHORTADDR})'
	DEFS += -DCFG_EEPROM_UART_SPEED='(uint16_t)(0x${CFG_EEPROM_UART_SPEED})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CALIBRATED='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CALIBRATED})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_AN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_AN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_BN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_BN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_CN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_CN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_DN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_DN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_EN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_EN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_FN='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_FN})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER})'
	DEFS += -DCFG_EEPROM_TOUCHSCREEN_THRESHHOLD='(uint16_t)(0x${CFG_EEPROM_TOUCHSCREEN_THRESHHOLD})'
endif

ifeq (${CFG_LM75B},1)
	DEFS += -DCFG_LM75B
	VPATH += drivers/sensors/lm75b
	OBJS += lm75b.o
endif

ifeq (${CFG_CHIBI},1)
	ifeq (${CFG_SSP0_SCKPIN},)
$(error CFG_CHIBI requires CFG_SSP0_SCKPIN to use SSP)
	endif
	ifneq (${CFG_I2CEEPROM},1)
$(error CFG_CHIBI requires CFG_I2CEEPROM to store and retrieve addresses)
	endif
	ifeq (${CFG_SDCARD},1)
$(error CFG_CHIBI and CFG_SDCARD can not be defined at the same time. Only one SPI block is available on the LPC1343.)
	endif
	ifeq (${CFG_TFTLCD},1)
$(error CFG_CHIBI and CFG_TFTLCD can not be defined at the same time since they both use pins 1.8, 1.9 and 1.10.)
	endif
	ifeq (${CFG_PWM},1)
$(error CFG_CHIBI and CFG_PWM can not be defined at the same time since they both use pin 1.9.)
	endif
	ifneq (${CFG_CHIBI_PROMISCUOUS},1)
		ifneq (${CFG_CHIBI_PROMISCUOUS},0)
$(error CFG_CHIBI_PROMISCUOUS must be equal to either 1 or 0)
		endif
	endif
	ifneq (${GPIO_ENABLE_IRQ1},1)
$(error GPIO_ENABLE_IRQ1 must be enabled when using Chibi (Chibi IRQ is on GPIO1.8))
	endif
	
	DEFS += -DCFG_CHIBI
	DEFS += -DCFG_CHIBI_MODE='(${CFG_CHIBI_MODE})'
	DEFS += -DCFG_CHIBI_POWER='(${CFG_CHIBI_POWER})'
	DEFS += -DCFG_CHIBI_CHANNEL='(${CFG_CHIBI_CHANNEL})'
	DEFS += -DCFG_CHIBI_PANID='(0x${CFG_CHIBI_PANID})'
	DEFS += -DCFG_CHIBI_PROMISCUOUS='(${CFG_CHIBI_PROMISCUOUS})'
	DEFS += -DCFG_CHIBI_BUFFERSIZE='(${CFG_CHIBI_BUFFERSIZE})'
	VPATH += drivers/rf/chibi
	OBJS += chb.o chb_buf.o chb_drvr.o chb_eeprom.o chb_spi.o
endif

ifeq (${CFG_TFTLCD},1)
	ifeq (${CFG_ST7565},1)
$(error CFG_TFTLCD and CFG_ST7565 can not be defined at the same time.)
	endif
	ifeq (${CFG_SSD1306},1)
$(error CFG_TFTLCD and CFG_SSD1306 can not be defined at the same time.)
	endif
	ifeq (${CFG_SHARPMEM},1)
$(error CFG_TFTLCD and CFG_SHARPMEM can not be defined at the same time.)
	endif
	ifeq (${CFG_PWM},1)
$(error CFG_TFTLCD and CFG_PWM can not be defined at the same time since they both use pin 1.9.)
	endif
	ifneq (${CFG_I2CEEPROM},1)
$(error CFG_TFTLCD requires CFG_I2CEEPROM to store and retrieve configuration settings)
	endif
	
	ifeq (${CFG_TFTLCD_DRIVER},hx8347d)
		ifeq (${CFG_SSP0_SCKPIN},)
$(error CFG_TFTLCD_DRIVER=hx8347d requires CFG_SSP0_SCKPIN to use SSP)
		endif
	endif
	
	DEFS += -DCFG_TFTLCD
	DEFS += -DCFG_TFTLCD_INCLUDESMALLFONTS='(${CFG_TFTLCD_INCLUDESMALLFONTS})'
	DEFS += -DCFG_TFTLCD_USEAAFONTS='(${CFG_TFTLCD_USEAAFONTS})'
	DEFS += -DCFG_TFTLCD_TS_DEFAULTTHRESHOLD='(${CFG_TFTLCD_TS_DEFAULTTHRESHOLD})'
	DEFS += -DCFG_TFTLCD_TS_KEYPADDELAY='(${CFG_TFTLCD_TS_KEYPADDELAY})'
	
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
	OBJS += ${CFG_TFTLCD_DRIVER}.o
	
	#Character Displays (VFD text displays, etc.)
	VPATH += drivers/displays/character/samsung_20T202DA2JA
	OBJS += samsung_20T202DA2JA.o
endif

ifneq (${CFG_ST7565}${CFG_SSD1306}${CFG_SHARPMEM},)
	# Bitmap/Monochrome LCD support (ST7565, SSD1306, etc.)
	VPATH += drivers/displays
	OBJS += smallfonts.o
	
	ifeq (${CFG_ST7565},1)
		DEFS += -DCFG_ST7565
		VPATH += drivers/displays/bitmap/st7565
		OBJS += st7565.o
		ifeq (${CFG_SSD1306},1)
$(error CFG_ST7565 and CFG_SSD1306 can not be defined at the same time)
		endif
	endif
	ifeq (${CFG_SSD1306},1)
		DEFS += -DCFG_SSD1306
		VPATH += drivers/displays/bitmap/ssd1306
		OBJS += ssd1306.o
	endif
	ifeq (${CFG_SHARPMEM},1)
		DEFS += -DCFG_SHARPMEM
		VPATH += drivers/displays/bitmap/sharpmem
		OBJS += sharpmem.o
	endif
endif

ifeq (${CFG_RSA},1)
	ifneq (${CFG_RSA_BITS},32)
		ifneq (${CFG_RSA_BITS},64)
$(error CFG_RSA_BITS must be equal to either 32 or 64.)
		endif
	endif
	DEFS += -DCFG_RSA -DCFG_RSA_BITS='(${CFG_RSA_BITS})'
	VPATH += drivers/rsa
	OBJS += rsa.o
endif
