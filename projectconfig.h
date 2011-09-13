/**************************************************************************/
/*! 
    @file     projectconfig.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#include "lpc134x.h"
#include "sysdefs.h"

/*=========================================================================
    BOARD SELECTION

    Because several boards use this code library with sometimes slightly
    different pin configuration, you will need to specify which board you
    are using by enabling one of the following definitions. The code base
    will then try to configure itself accordingly for that board.
    -----------------------------------------------------------------------*/
    #define CFG_BRD_LPC1343_REFDESIGN
    // #define CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
    // #define CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
    // #define CFG_BRD_LPC1343_802154USBSTICK
/*=========================================================================*/


/**************************************************************************
    PIN USAGE
    -----------------------------------------------------------------------
    This table tries to give an indication of which GPIO pins and 
    peripherals are used by the available drivers and SW examples.  Only
    dedicated GPIO pins available on the LPC1343 Reference Board are shown
    below.  Any unused peripheral blocks like I2C, SSP, ADC, etc., can
    also be used as GPIO if they are available.

                PORT 1        PORT 2                PORT 3 
                =========     =================     =======
                8 9 10 11     1 2 3 4 5 6 7 8 9     0 1 2 3

    SDCARD      . .  .  .     . . . . . . . . .     X . . .
    PWM         . X  .  .     . . . . . . . . .     . . . .
    STEPPER     . .  .  .     . . . . . . . . .     X X X X
    CHIBI       X X  X  .     . . . . . . . . .     . . . .
    ILI9325/8   X X  X  X     X X X X X X X X X     . . . X
    ST7565      X X  X  X     X X X X X X X X X     . . . X
    ST7735      . .  .  .     X X X X X X . . .     . . . .
    SHARPMEM    . .  .  .     X X X X . . . . .     . . . .
    SSD1306     . .  .  .     X X X . X X . . .     . . . .
    MCP121      . .  .  .     . . . . . . . . .     . X . .

                TIMERS                    SSP     ADC         UART
                ======================    ===     =======     ====
                16B0  16B1  32B0  32B1    0       0 1 2 3     0

    SDCARD      .     .     .     .       X       . . . .     .
    PWM         .     X     .     .       .       . . . .     .
    PMU [1]     .     .     X     .       .       . . . .     .
    USB         .     .     .     X       .       . . . .     .
    STEPPER     .     .     X     .       .       . . . .     .
    CHIBI       x     .     .     .       X       . . . .     .
    ILI9325/8   .     .     .     .       .       X X X X     .
    ST7565      .     .     .     .       .       X X X X     .
    ST7535      .     .     .     .       .       . . . .     .
    SHARPMEM    .     .     .     .       .       . . . .     .
    SSD1306     .     .     .     .       .       . . . .     .
    INTERFACE   .     .     .     .       .       . . . .     X[2]

    [1]  PMU uses 32-bit Timer 0 for SW wakeup from deep-sleep.  This timer
         can safely be used by other peripherals, but may need to be
         reconfigured when you wakeup from deep-sleep.
    [2]  INTERFACE can be configured to use either USBCDC or UART

 **************************************************************************/


/**************************************************************************
    I2C Addresses
    -----------------------------------------------------------------------
    The following addresses are used by the different I2C sensors included
    in the code base [1]

                                HEX       BINARY  
                                ====      ========
    ISL12022M (RTC)             0xDE      1101111x
    ISL12022M (SRAM)            0xAE      1010111x
    LM75B                       0x90      1001000x
    MCP24AA                     0xA0      1010000x
    MCP4725                     0xC0      1100000x
    TSL2561                     0x72      0111001x
    TCS3414                     0x72      0111001x

    [1]  Alternative addresses may exists, but the addresses listed in this
         table are the values used in the code base

 **************************************************************************/


/*=========================================================================
    FIRMWARE VERSION SETTINGS
    -----------------------------------------------------------------------*/
    #define CFG_FIRMWARE_VERSION_MAJOR            (0)
    #define CFG_FIRMWARE_VERSION_MINOR            (9)
    #define CFG_FIRMWARE_VERSION_REVISION         (8)
/*=========================================================================*/


/*=========================================================================
    CORE CPU SETTINGS
    -----------------------------------------------------------------------

    CFG_CPU_CCLK    Value is for reference only.  'core/cpu/cpu.c' must
                    be modified to change the clock speed, but the value
                    should be indicated here since CFG_CPU_CCLK is used by
                    other peripherals to determine timing.

    -----------------------------------------------------------------------*/
    #define CFG_CPU_CCLK                (72000000)  // 1 tick = 13.88nS
/*=========================================================================*/


/*=========================================================================
    SYSTICK TIMER
    -----------------------------------------------------------------------

    CFG_SYSTICK_DELAY_IN_MS   The number of milliseconds between each tick
                              of the systick timer.

    -----------------------------------------------------------------------*/
    #define CFG_SYSTICK_DELAY_IN_MS     (1)
/*=========================================================================*/


/*=========================================================================
    ALTERNATE RESET PIN
    -----------------------------------------------------------------------

    CFG_ALTRESET        If defined, indicates that a GPIO pin should be
                        configured as an alternate reset pin in addition
                        to the dedicated reset pin.
    CFG_ALTRESET_PORT   The GPIO port where the alt reset pin is located
    CFG_ALTRESET_PIN    The GPIO pin where the alt reset pin is located

    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_ALTRESET
      #define CFG_ALTRESET_PORT         (1)
      #define CFG_ALTRESET_PIN          (5)   // P1.5 = RTS
    #endif
/*=========================================================================*/


/*=========================================================================
    UART
    -----------------------------------------------------------------------

    CFG_UART_BAUDRATE         The default UART speed.  This value is used 
                              when initialising UART, and should be a 
                              standard value like 57600, 9600, etc.  
                              NOTE: This value may be overridden if
                              another value is stored in EEPROM!
    CFG_UART_BUFSIZE          The length in bytes of the UART RX FIFO. This
                              will determine the maximum number of received
                              characters to store in memory.

    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      #define CFG_UART_BAUDRATE           (115200)
      #define CFG_UART_BUFSIZE            (512)
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
      #define CFG_UART_BAUDRATE           (115200)
      #define CFG_UART_BUFSIZE            (512)
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_UART_BAUDRATE           (57600)
      #define CFG_UART_BUFSIZE            (512)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      #define CFG_UART_BAUDRATE           (115200)
      #define CFG_UART_BUFSIZE            (512)
    #endif
/*=========================================================================*/


/*=========================================================================
    SSP
    -----------------------------------------------------------------------

    CFG_SSP0_SCKPIN_2_11      Indicates which pin should be used for SCK0
    CFG_SSP0_SCKPIN_0_6

    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      #define CFG_SSP0_SCKPIN_2_11
      // #define CFG_SSP0_SCKPIN_0_6
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_SSP0_SCKPIN_2_11
      // #define CFG_SSP0_SCKPIN_0_6
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_SSP0_SCKPIN_2_11
      #define CFG_SSP0_SCKPIN_0_6
    #endif
/*=========================================================================*/


/*=========================================================================
    ON-BOARD LED
    -----------------------------------------------------------------------

    CFG_LED_PORT              The port for the on board LED
    CFG_LED_PIN               The pin for the on board LED
    CFG_LED_ON                The pin state to turn the LED on (0 = low, 1 = high)
    CFG_LED_OFF               The pin state to turn the LED off (0 = low, 1 = high)

    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      #define CFG_LED_PORT                (2)
      #define CFG_LED_PIN                 (10)
      #define CFG_LED_ON                  (0)
      #define CFG_LED_OFF                 (1)
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_LED_PORT                (2)
      #define CFG_LED_PIN                 (10)
      #define CFG_LED_ON                  (0)
      #define CFG_LED_OFF                 (1)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      #define CFG_LED_PORT                (3)
      #define CFG_LED_PIN                 (2)
      #define CFG_LED_ON                  (0)
      #define CFG_LED_OFF                 (1)
    #endif
/*=========================================================================*/


/*=========================================================================
    MICRO-SD CARD
    -----------------------------------------------------------------------

    CFG_SDCARD                If this field is defined SD Card and FAT32
                              file system support will be included
    CFG_SDCARD_READONLY       If this is set to 1, all commands to
                              write to the SD card will be removed
                              saving some flash space.
    CFG_SDCARD_CDPORT         The card detect port number
    CFG_SDCARD_CDPIN          The card detect pin number

    NOTE:                     All config settings for FAT32 are defined
                              in ffconf.h

    BENCHMARK:                With SPI set to 6.0MHz, FATFS can read
                              ~300KB/s (w/512 byte read buffer)
							  
    PIN LAYOUT:               The pin layout that is used by this driver
                              can be seen in the following schematic:
                              /tools/schematics/Breakout_TFTLCD_ILI9325_v1.3

    DEPENDENCIES:             SDCARD requires the use of SSP0.
    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      // #define CFG_SDCARD
      #define CFG_SDCARD_READONLY         (1)   // Must be 0 or 1
      #define CFG_SDCARD_CDPORT           (3)
      #define CFG_SDCARD_CDPIN            (0)
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_SDCARD
      #define CFG_SDCARD_READONLY         (1)   // Must be 0 or 1
      #define CFG_SDCARD_CDPORT           (3)
      #define CFG_SDCARD_CDPIN            (0)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_SDCARD
      #define CFG_SDCARD_READONLY         (1)   // Must be 0 or 1
      #define CFG_SDCARD_CDPORT           (3)
      #define CFG_SDCARD_CDPIN            (0)
    #endif
/*=========================================================================*/


/*=========================================================================
    USB
    -----------------------------------------------------------------------

    CFG_USBHID                If this field is defined USB HID support will
                              be included.  Currently uses ROM-based USB HID
    CFG_USBCDC                If this field is defined USB CDC support will
                              be included, with the USB Serial Port speed
                              set to 115200 BPS by default
    CFG_USBCDC_BAUDRATE       The default TX/RX speed.  This value is used 
                              when initialising USBCDC, and should be a 
                              standard value like 57600, 9600, etc.
    CFG_USBCDC_INITTIMEOUT    The maximum delay in milliseconds to wait for
                              USB to connect.  Must be a multiple of 10!
    CFG_USBCDC_BUFFERSIZE     Size of the buffer (in bytes) that stores
                              printf data until it can be sent out in
                              64 byte frames.  The buffer is required since
                              only one frame per ms can be sent using USB
                              CDC (see 'puts' in systeminit.c).

    -----------------------------------------------------------------------*/
    #define CFG_USB_VID                   (0x239A)
    #define CFG_USB_PID                   (0x1002)

    #ifdef CFG_BRD_LPC1343_REFDESIGN
      // #define CFG_USBHID
      #define CFG_USBCDC
      #define CFG_USBCDC_BAUDRATE         (115200)
      #define CFG_USBCDC_INITTIMEOUT      (5000)
      #define CFG_USBCDC_BUFFERSIZE       (256)
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
      // #define CFG_USBHID
      #define CFG_USBCDC
      #define CFG_USBCDC_BAUDRATE         (115200)
      #define CFG_USBCDC_INITTIMEOUT      (5000)
      #define CFG_USBCDC_BUFFERSIZE       (256)
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      // #define CFG_USBHID
      // #define CFG_USBCDC
      #define CFG_USBCDC_BAUDRATE         (57600)
      #define CFG_USBCDC_INITTIMEOUT      (5000)
      #define CFG_USBCDC_BUFFERSIZE       (256)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_USBHID
      #define CFG_USBCDC
      #define CFG_USBCDC_BAUDRATE         (115200)
      #define CFG_USBCDC_INITTIMEOUT      (5000)
      #define CFG_USBCDC_BUFFERSIZE       (256)
    #endif
/*=========================================================================*/


/*=========================================================================
    PRINTF REDIRECTION
    -----------------------------------------------------------------------

    CFG_PRINTF_UART           Will cause all printf statements to be 
                              redirected to UART
    CFG_PRINTF_USBCDC         Will cause all printf statements to be
                              redirect to USB Serial
    CFG_PRINTF_NEWLINE        This is typically "\r\n" for Windows or
                              "\n" for *nix

    Note: If no printf redirection definitions are present, all printf
    output will be ignored.
    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      // #define CFG_PRINTF_UART
      #define CFG_PRINTF_USBCDC
      #define CFG_PRINTF_NEWLINE          "\r\n"
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
      // #define CFG_PRINTF_UART
      #define CFG_PRINTF_USBCDC
      #define CFG_PRINTF_NEWLINE          "\r\n"
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_PRINTF_UART
      // #define CFG_PRINTF_USBCDC
      #define CFG_PRINTF_NEWLINE          "\n"
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_PRINTF_UART
      #define CFG_PRINTF_USBCDC
      #define CFG_PRINTF_NEWLINE          "\r\n"
    #endif
/*=========================================================================*/


/*=========================================================================
    COMMAND LINE INTERFACE
    -----------------------------------------------------------------------

    CFG_INTERFACE             If this field is defined the UART or USBCDC
                              based command-line interface will be included
    CFG_INTERFACE_MAXMSGSIZE  The maximum number of bytes to accept for an
                              incoming command
    CFG_INTERFACE_PROMPT      The command prompt to display at the start
                              of every new data entry line
    CFG_INTERFACE_SILENTMODE  If this is set to 1 only text generated in
                              response to commands will be send to the
                              output buffer.  The command prompt will not
                              be displayed and incoming text will not be
                              echoed back to the output buffer (allowing
                              you to see the text you have input).  This
                              is normally only desirable in a situation
                              where another MCU is communicating with 
                              the LPC1343.
    CFG_INTERFACE_DROPCR      If this is set to 1 all incoming \r
                              characters will be dropped
    CFG_INTERFACE_ENABLEIRQ   If this is set to 1 the IRQ pin will be
                              set high when a command starts executing
                              and will go low when the command has
                              finished executing or the LCD is not busy.
                              This allows another device to know when a
                              new command can safely be sent.
    CFG_INTERFACE_IRQPORT     The gpio port for the IRQ/busy pin
    CFG_INTERFACE_IRQPIN      The gpio pin number for the IRQ/busy pin
    CFG_INTERFACE_SHORTERRORS If this is enabled only short 1 character
                              error messages will be returned (followed
                              by CFG_PRINTF_NEWLINE), rather than more
                              verbose error messages.  The specific
                              characters used are defined below.
    CFG_INTERFACE_CONFIRMREADY  If this is set to 1 a text confirmation
                              will be sent when the command prompt is
                              ready for a new command.  This is in
                              addition to CFG_INTERFACE_ENABLEIRQ if
                              this is also enabled.  The character used
                              is defined below.

    NOTE:                     The command-line interface will use either
                              USB-CDC or UART depending on whether
                              CFG_PRINTF_UART or CFG_PRINTF_USBCDC are 
                              selected.
    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      #define CFG_INTERFACE
      #define CFG_INTERFACE_MAXMSGSIZE    (256)
      #define CFG_INTERFACE_PROMPT        "LPC1343 >> "
      #define CFG_INTERFACE_SILENTMODE    (0)
      #define CFG_INTERFACE_DROPCR        (0)
      #define CFG_INTERFACE_ENABLEIRQ     (0)
      #define CFG_INTERFACE_IRQPORT       (0)
      #define CFG_INTERFACE_IRQPIN        (7)
      #define CFG_INTERFACE_SHORTERRORS   (0)
      #define CFG_INTERFACE_CONFIRMREADY  (0)
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
      #define CFG_INTERFACE
      #define CFG_INTERFACE_MAXMSGSIZE    (256)
      #define CFG_INTERFACE_PROMPT        "CMD >> "
      #define CFG_INTERFACE_SILENTMODE    (0)
      #define CFG_INTERFACE_DROPCR        (0)
      #define CFG_INTERFACE_ENABLEIRQ     (0)
      #define CFG_INTERFACE_IRQPORT       (0)
      #define CFG_INTERFACE_IRQPIN        (7)
      #define CFG_INTERFACE_SHORTERRORS   (0)
      #define CFG_INTERFACE_CONFIRMREADY  (0)
      #define CFG_INTERFACE_SHORTERRORS_UNKNOWNCOMMAND    "?"
      #define CFG_INTERFACE_SHORTERRORS_TOOMANYARGS       ">"
      #define CFG_INTERFACE_SHORTERRORS_TOOFEWARGS        "<"
      #define CFG_INTERFACE_CONFIRMREADY_TEXT             "."
    #endif

    #ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_INTERFACE
      #define CFG_INTERFACE_MAXMSGSIZE    (256)
      #define CFG_INTERFACE_PROMPT        ">>"
      #define CFG_INTERFACE_SILENTMODE    (1)
      #define CFG_INTERFACE_DROPCR        (1)
      #define CFG_INTERFACE_ENABLEIRQ     (1)
      #define CFG_INTERFACE_IRQPORT       (0)
      #define CFG_INTERFACE_IRQPIN        (7)
      #define CFG_INTERFACE_SHORTERRORS   (1)
      #define CFG_INTERFACE_CONFIRMREADY  (0)
      #define CFG_INTERFACE_SHORTERRORS_UNKNOWNCOMMAND    "?"
      #define CFG_INTERFACE_SHORTERRORS_TOOMANYARGS       ">"
      #define CFG_INTERFACE_SHORTERRORS_TOOFEWARGS        "<"
      #define CFG_INTERFACE_CONFIRMREADY_TEXT             "."
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_INTERFACE
      #define CFG_INTERFACE_MAXMSGSIZE    (256)
      #define CFG_INTERFACE_PROMPT        "CMD >> "
      #define CFG_INTERFACE_SILENTMODE    (0)
      #define CFG_INTERFACE_DROPCR        (0)
      #define CFG_INTERFACE_ENABLEIRQ     (0)
      #define CFG_INTERFACE_IRQPORT       (0)
      #define CFG_INTERFACE_IRQPIN        (7)
      #define CFG_INTERFACE_SHORTERRORS   (0)
      #define CFG_INTERFACE_CONFIRMREADY  (0)
    #endif
/*=========================================================================*/


/*=========================================================================
    PWM SETTINGS
    -----------------------------------------------------------------------

    CFG_PWM                     If this is defined, a basic PWM driver
                                will be included using 16-bit Timer 1 and
                                Pin 1.9 (MAT0) for the PWM output.  In
                                order to allow for a fixed number of
                                pulses to be generated, some PWM-specific
                                code is required in the 16-Bit Timer 1
                                ISR.  See "core/timer16/timer16.c" for
                                more information.
    CFG_PWM_DEFAULT_PULSEWIDTH  The default pulse width in ticks
    CFG_PWM_DEFAULT_DUTYCYCLE   The default duty cycle in percent

    DEPENDENCIES:               PWM output requires the use of 16-bit
                                timer 1 and pin 1.9 (CT16B1_MAT0).
    -----------------------------------------------------------------------*/
    // #define CFG_PWM
    #define CFG_PWM_DEFAULT_PULSEWIDTH  (CFG_CPU_CCLK / 1000)
    #define CFG_PWM_DEFAULT_DUTYCYCLE   (50)
/*=========================================================================*/


/*=========================================================================
    STEPPER MOTOR SETTINGS
    -----------------------------------------------------------------------

    CFG_STEPPER                 If this is defined, a simple bi-polar 
                                stepper motor will be included for common
                                H-bridge chips like the L293D or SN754410N

    DEPENDENCIES:               STEPPER requires the use of pins 3.0-3 and
                                32-bit Timer 0.
    -----------------------------------------------------------------------*/
    // #define CFG_STEPPER
/*=========================================================================*/


/*=========================================================================
    EEPROM
    -----------------------------------------------------------------------

    CFG_I2CEEPROM             If defined, drivers for the onboard EEPROM
                              will be included during build
    CFG_I2CEEPROM_SIZE        The number of bytes available on the EEPROM

    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      #define CFG_I2CEEPROM
      #define CFG_I2CEEPROM_SIZE          (3072)
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_I2CEEPROM
      #define CFG_I2CEEPROM_SIZE          (3072)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      #define CFG_I2CEEPROM
      #define CFG_I2CEEPROM_SIZE          (3072)
    #endif
/*=========================================================================*/


/*=========================================================================
    EEPROM MEMORY MAP
    -----------------------------------------------------------------------
    EEPROM is used to persist certain user modifiable values to make
    sure that these changes remain in effect after a reset or hard
    power-down.  The addresses in EEPROM for these various system
    settings/values are defined below.  The first 256 bytes of EEPROM
    are reserved for this (0x0000..0x00FF).

    CFG_EEPROM_RESERVED       The last byte of reserved EEPROM memory

          EEPROM Address (0x0000..0x00FF)
          ===============================
          0 1 2 3 4 5 6 7 8 9 A B C D E F
    000x  x x x x x x x x . x x . . . . .   Chibi
    001x  . . . . . . . . . . . . . . . .   
    002x  x x x x . . . . . . . . . . . .   UART
    003x  x x x x x x x x x x x x x x x x   Touch Screen Calibration
    004x  x x x x x x x x x x x x x x . .   Touch Screen Calibration
    005x  . . . . . . . . . . . . . . . .
    006x  . . . . . . . . . . . . . . . .
    007x  . . . . . . . . . . . . . . . .
    008x  . . . . . . . . . . . . . . . .
    009x  . . . . . . . . . . . . . . . .
    00Ax  . . . . . . . . . . . . . . . .
    00Bx  . . . . . . . . . . . . . . . .
    00Cx  . . . . . . . . . . . . . . . .
    00Dx  . . . . . . . . . . . . . . . .
    00Ex  . . . . . . . . . . . . . . . .
    00Fx  . . . . . . . . . . . . . . . .

    -----------------------------------------------------------------------*/
    #define CFG_EEPROM_RESERVED                 (0x00FF)              // Protect first 256 bytes of memory
    #define CFG_EEPROM_CHIBI_IEEEADDR           (uint16_t)(0x0000)    // 8
    #define CFG_EEPROM_CHIBI_SHORTADDR          (uint16_t)(0x0009)    // 2
    #define CFG_EEPROM_UART_SPEED               (uint16_t)(0x0020)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CALIBRATED   (uint16_t)(0x0030)    // 1
    #define CFG_EEPROM_TOUCHSCREEN_CAL_AN       (uint16_t)(0x0031)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_BN       (uint16_t)(0x0035)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_CN       (uint16_t)(0x0039)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_DN       (uint16_t)(0x003D)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_EN       (uint16_t)(0x0041)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_FN       (uint16_t)(0x0045)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER  (uint16_t)(0x0049)    // 4
    #define CFG_EEPROM_TOUCHSCREEN_THRESHHOLD   (uint16_t)(0x004D)    // 1
/*=========================================================================*/


/*=========================================================================
    LM75B TEMPERATURE SENSOR
    -----------------------------------------------------------------------

    CFG_LM75B                 If defined, drivers for an optional LM75B
                              temperature sensor will be included during
                              build (requires external HW)

    -----------------------------------------------------------------------*/
    // #define CFG_LM75B
/*=========================================================================*/


/*=========================================================================
    CHIBI WIRELESS STACK
    -----------------------------------------------------------------------

    CFG_CHIBI                   If defined, the CHIBI wireless stack will be
                                included during build.  Requires external HW.
    CFG_CHIBI_MODE              The mode to use when receiving and transmitting
                                wireless data.  See chb_drvr.h for possible values
    CFG_CHIBI_POWER             The power level to use when transmitting.  See
                                chb_drvr.h for possible values
    CFG_CHIBI_CHANNEL           802.15.4 Channel (0 = 868MHz, 1-10 = 915MHz)
    CFG_CHIBI_PANID             16-bit PAN Identifier (ex.0x1234)
    CFG_CHIBI_PROMISCUOUS       Set to 1 to enabled promiscuous mode or
                                0 to disable it.  If promiscuous mode is
                                enabled be sure to set CFG_CHIBI_BUFFERSIZE
                                to an appropriately large value (ex. 1024)
    CFG_CHIBI_BUFFERSIZE        The size of the message buffer in bytes

    DEPENDENCIES:               Chibi requires the use of SSP0, 16-bit timer
                                0 and pins 3.1, 3.2, 3.3.  It also requires
                                the presence of CFG_I2CEEPROM.

    NOTE:                       These settings are not relevant to all boards!
                                'tools/schematics/AT86RF212LPC1114_v1.6.pdf'
                                show how 'CHIBI' is meant to be connected
    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      // #define CFG_CHIBI
      #define CFG_CHIBI_MODE              (0)                 // OQPSK_868MHZ
      #define CFG_CHIBI_POWER             (0xE9)              // CHB_PWR_EU2_3DBM
      #define CFG_CHIBI_CHANNEL           (0)                 // 868-868.6 MHz
      #define CFG_CHIBI_PANID             (0x1234)
      #define CFG_CHIBI_PROMISCUOUS       (0)
      #define CFG_CHIBI_BUFFERSIZE        (128)
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      // #define CFG_CHIBI
      #define CFG_CHIBI_MODE              (0)                 // OQPSK_868MHZ
      #define CFG_CHIBI_POWER             (0xE9)              // CHB_PWR_EU2_3DBM
      #define CFG_CHIBI_CHANNEL           (0)                 // 868-868.6 MHz
      #define CFG_CHIBI_PANID             (0x1234)
      #define CFG_CHIBI_PROMISCUOUS       (0)
      #define CFG_CHIBI_BUFFERSIZE        (128)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      #define CFG_CHIBI
      #define CFG_CHIBI_MODE              (0)                 // OQPSK_868MHZ
      #define CFG_CHIBI_POWER             (0xE9)              // CHB_PWR_EU2_3DBM
      #define CFG_CHIBI_CHANNEL           (0)                 // 868-868.6 MHz
      #define CFG_CHIBI_PANID             (0x1234)
      #define CFG_CHIBI_PROMISCUOUS       (0)
      #define CFG_CHIBI_BUFFERSIZE        (1024)
    #endif
/*=========================================================================*/


/*=========================================================================
    TFT LCD
    -----------------------------------------------------------------------

    CFG_TFTLCD                  If defined, this will cause drivers for
                                a pre-determined LCD screen to be included
                                during build.  Only one LCD driver can be 
                                included during the build process (for ex.
                                'drivers/lcd/hw/ILI9325.c')
    CFG_TFTLCD_INCLUDESMALLFONTS If set to 1, smallfont support will be
                                included for 3x6, 5x8, 7x8 and 8x8 fonts.
                                This should only be enabled if these small
                                fonts are required since there is already
                                support for larger fonts generated with
                                Dot Factory 
                                http://www.pavius.net/downloads/tools/53-the-dot-factory
    CFG_TFTLCD_TS_DEFAULTTHRESHOLD  Default minimum threshold to trigger a
                                touch event with the touch screen (and exit
                                from 'tsWaitForEvent' in touchscreen.c).
                                Should be an 8-bit value somewhere between
                                8 and 75 in normal circumstances.  This is
                                the default value and may be overriden by
                                a value stored in EEPROM.
    CFG_TFTLCD_TS_KEYPADDELAY   The delay in milliseconds between key
                                presses in dialogue boxes

    PIN LAYOUT:                 The pin layout that is used by this driver
                                can be seen in the following schematic:
                                /tools/schematics/Breakout_TFTLCD_ILI9325_v1.3

    DEPENDENCIES:               TFTLCD requires the use of pins 1.8, 1.9,
                                1.10, 1.11, 3.3 and 2.1-9.
    -----------------------------------------------------------------------*/
    #ifdef CFG_BRD_LPC1343_REFDESIGN
      // #define CFG_TFTLCD
      #define CFG_TFTLCD_INCLUDESMALLFONTS   (0)
      #define CFG_TFTLCD_TS_DEFAULTTHRESHOLD (50)
      #define CFG_TFTLCD_TS_KEYPADDELAY      (100)
    #endif

    #if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
      #define CFG_TFTLCD
      #define CFG_TFTLCD_INCLUDESMALLFONTS   (0)
      #define CFG_TFTLCD_TS_DEFAULTTHRESHOLD (50)
      #define CFG_TFTLCD_TS_KEYPADDELAY      (100)
    #endif

    #ifdef CFG_BRD_LPC1343_802154USBSTICK
      // #define CFG_TFTLCD
      #define CFG_TFTLCD_INCLUDESMALLFONTS   (0)
      #define CFG_TFTLCD_TS_DEFAULTTHRESHOLD (50)
      #define CFG_TFTLCD_TS_KEYPADDELAY      (100)
    #endif
/*=========================================================================*/


/*=========================================================================
    Monochrome/Bitmap Graphic LCDs
    -----------------------------------------------------------------------

    CFG_ST7565                If defined, this will cause drivers for
                              the 128x64 pixel ST7565 LCD to be included
    CFG_SSD1306               If defined, this will cause drivers for
                              the 128x64 pixel SSD1306 OLED display to be
                              included
    CFG_SHARPMEM              If defined, this will cause drivers for
                              Sharp Memory Displays to be included

    DEPENDENCIES:             ST7565 requires the use of pins 2.1-6.
    DEPENDENCIES:             SSD1306 requires the use of pins 2.1-6.
    DEPENDENCIES:             SHARPMEM requires the use of pins 2.1-4.
    -----------------------------------------------------------------------*/
    // #define CFG_ST7565
    // #define CFG_SSD1306
    // #define CFG_SHARPMEM
/*=========================================================================*/


/*=========================================================================
    RSA Encryption
    -----------------------------------------------------------------------

    CFG_RSA                     If defined, support for basic RSA
                                encryption will be included.
    CFG_RSA_BITS                Indicates the number of bits used for
                                RSA encryption keys.  To keep code size
                                reasonable, RSA encryption is currently
                                limited to using 64-bit or 32-bit numbers,
                                with 64-bit providing higher security, and
                                32-bit providing smaller encrypted text
                                size.
                                  
    NOTE:                       Please note that Printf can not be
                                used to display 64-bit values (%lld)!
    -----------------------------------------------------------------------*/
    // #define CFG_RSA
    #define CFG_RSA_BITS                  (32)
/*=========================================================================*/




/*=========================================================================
  CONFIG FILE VALIDATION
  -------------------------------------------------------------------------
  Basic error checking to make sure that incompatible defines are not 
  enabled at the same time, etc.

  =========================================================================*/

#if !defined CFG_BRD_LPC1343_REFDESIGN && !defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB && !defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART && !defined CFG_BRD_LPC1343_802154USBSTICK
  #error "You must defined a target board (CFG_BRD_LPC1343_REFDESIGN or CFG_BRD_LPC1343_TFTLCDSTANDALONE or CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART or CFG_BRD_LPC1343_802154USBSTICK)"
#endif

#if defined CFG_PRINTF_USBCDC && defined CFG_PRINTF_UART
  #error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC cannot both be defined at once"
#endif

#if defined CFG_PRINTF_USBCDC && !defined CFG_USBCDC
  #error "CFG_PRINTF_CDC requires CFG_USBCDC to be defined as well"
#endif

#if defined CFG_USBCDC && defined CFG_USBHID
  #error "Only one USB class can be defined at a time (CFG_USBCDC or CFG_USBHID)"
#endif

#if defined CFG_SSP0_SCKPIN_2_11 && defined CFG_SSP0_SCKPIN_0_6
  #error "Only one SCK pin can be defined at a time for SSP0"
#endif

#if !defined CFG_SSP0_SCKPIN_2_11 && !defined CFG_SSP0_SCKPIN_0_6
  #error "An SCK pin must be selected for SSP0 (CFG_SSP0_SCKPIN_2_11 or CFG_SSP0_SCKPIN_0_6)"
#endif

#ifdef CFG_INTERFACE
  #if !defined CFG_PRINTF_UART && !defined CFG_PRINTF_USBCDC
    #error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC must be defined for for CFG_INTERFACE Input/Output"
  #endif
  #if defined CFG_PRINTF_USBCDC && CFG_INTERFACE_SILENTMODE == 1
    #error "CFG_INTERFACE_SILENTMODE typically isn't enabled with CFG_PRINTF_USBCDC"
  #endif
#endif

#ifdef CFG_CHIBI
  #if !defined CFG_I2CEEPROM
    #error "CFG_CHIBI requires CFG_I2CEEPROM to store and retrieve addresses"
  #endif
  #ifdef CFG_SDCARD
    #error "CFG_CHIBI and CFG_SDCARD can not be defined at the same time. Only one SPI block is available on the LPC1343."
  #endif
  #ifdef CFG_TFTLCD
    #error "CFG_CHIBI and CFG_TFTLCD can not be defined at the same time since they both use pins 1.8, 1.9 and 1.10."
  #endif
  #ifdef CFG_PWM
    #error "CFG_CHIBI and CFG_PWM can not be defined at the same time since they both use pin 1.9."
  #endif
  #if CFG_CHIBI_PROMISCUOUS != 0 && CFG_CHIBI_PROMISCUOUS != 1
    #error "CFG_CHIBI_PROMISCUOUS must be equal to either 1 or 0"
  #endif
#endif

#ifdef CFG_TFTLCD
  #ifdef CFG_ST7565
    #error "CFG_TFTLCD and CFG_ST7565 can not be defined at the same time."
  #endif
  #ifdef CFG_SSD1306
    #error "CFG_TFTLCD and CFG_SSD1306 can not be defined at the same time."
  #endif
  #ifdef CFG_PWM
    #error "CFG_TFTLCD and CFG_PWM can not be defined at the same time since they both use pin 1.9."
  #endif
  #if !defined CFG_I2CEEPROM
    #error "CFG_TFTLCD requires CFG_I2CEEPROM to store and retrieve configuration settings"
  #endif
#endif

#ifdef CFG_SDCARD
  #ifdef CFG_STEPPER
    #error  "CFG_SDCARD and CFG_STEPPER can not be defined at the same time since they both use pin 3.0."
  #endif
#endif

#ifdef CFG_ST7565
  #ifdef CFG_SSD1306
    #error "CFG_ST7565 and CFG_SSD1306 can not be defined at the same time"
  #endif
#endif

#ifdef CFG_RSA
  #if CFG_RSA_BITS != 64 && CFG_RSA_BITS != 32
    #error "CFG_RSA_BITS must be equal to either 32 or 64."
  #endif
#endif

#endif
