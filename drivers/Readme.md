# Drivers

This folder contains basic drivers for a number of HW peripherals,
and a few SW stacks (FAT32, a light-weight 802.15.4 wireless stack
named Chibi, etc.).  It also contains a basic but relatively robust
graphic library with support for display custom fonts, loading and
saving Bitmap images, touch screen calibration, etc.

## dac

Digital to Analog Converters

## displays

Drivers and functions to work with a wide variety of display types.

This folder is sub-divided into the following categories:

/bitmap      Monochrome displays such as basic OLEDs, and 128x64 pixel
             graphic displays

/character   Text-only displays (VFD, etc.)

/segment     Segment displays (8x8 LED arrays, 7-segment displays, etc.)

/tft         Color graphical displays, such as TFT LCDs, RGB OLEDs, etc.
             This folder also contains a small graphics library, code to
			 load and save bitmap images, and functions to work with
			 several font types.

## fatfs

Chan's FATFS, enabling you to read and write to inexpensive SD cards

## motor

Motor-control related drivers

## rf

Anything related to RF, including NFC and 802.15.4 wireless

## rsa

A very minimal example of RSA encryption and decryption

## sensors

Drivers for a variety of sensors (light, temperature, etc.)

## storage 

Drivers for different types of storage, such as SPI NOR flash, EEPROM, etc.
