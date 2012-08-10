/**************************************************************************/
/*! 
    @file     ds18b20.c
    @author   Albertas Mickenas (mic@wemakethings.net)
    @date     8 August 2012
    @version  1.0

    @section DESCRIPTION
    Driver for DALLAS DS18B20 temperature sensor.
 
    @section Example

    @code
     #include "core/cpu/cpu.h"
     #include "drivers/sensors/ds18b20/ds18b20.h"
     
     int main(void) {
         cpuInit();

         int32_t temp = 0;
         
         // Initialise the DS18B20
         ds18b20Init(3, 0, &IOCON_PIO3_0);
         while (1) {
             temp = ds18b20GetTemperature();
                      
             // Use modulus operator to display decimal value
             printf("Current Temperature: %d.%d C\n", temp / 10000, temp % 10000);
         }
     }    
    @endcode
	
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

#include "ds18b20.h"
#include "core/gpio/gpio.h"
#include "core/timer16/timer16.h"

#define CMD_CONVERTTEMP 0x44 
#define CMD_RSCRATCHPAD 0xbe 
#define CMD_WSCRATCHPAD 0x4e 
#define CMD_CPYSCRATCHPAD 0x48 
#define CMD_RECEEPROM 0xb8 
#define CMD_RPWRSUPPLY 0xb4 
#define CMD_SEARCHROM 0xf0 
#define CMD_READROM 0x33 
#define CMD_MATCHROM 0x55 
#define CMD_SKIPROM 0xcc 
#define CMD_ALARMSEARCH 0xec 
#define DECIMAL_STEPS_12BIT 625 //.0625 

uint32_t portNum = 0;
uint32_t bitPos = 0;

inline void pullLineLow() {
    gpioSetDir(portNum, bitPos, gpioDirection_Output);
    gpioSetValue(portNum, bitPos, 0);
}

inline void releaseLine() {
    gpioSetDir(portNum, bitPos, gpioDirection_Input);
}

inline void delayUs(uint16_t us) {
    timer16DelayUS(1, us);
}

inline uint32_t readLineState() {
    return gpioGetValue(portNum, bitPos );
}

void writeBit(uint8_t bit){ 
	//Pull line low for 1uS 
    pullLineLow(); 
	delayUs(1); 
	//If we want to write 1, release the line (if not will keep low) 
	if(bit) {
        releaseLine();
    }
	//Wait for 60uS and release the line 
	delayUs(60); 
	releaseLine(); 
} 

uint8_t readBit(void){ 
	uint8_t bit = 0; 
	//Pull line low for 1uS 
	pullLineLow(); 
	delayUs(1); 
	//Release line and wait for 14uS 
	releaseLine(); 
	delayUs(14); 
	//Read line value 
	if(readLineState()){
        bit=1;    
    } 
	//Wait for 45uS to end and return read value 
	delayUs(45); 
	return bit; 
} 

uint8_t readByte(void){ 
	uint8_t i = 8, n = 0; 
	while(i--){ 
		//Shift one position right and store read value 
		n >>= 1; 
		n |= (readBit() << 7); 
	} 
	return n; 
} 

void writeByte(uint8_t b){ 
	uint8_t i = 8; 
	while(i--){ 
		//Write actual bit and shift one position right to make the next bit ready 
		writeBit(b & 1); 
		b >>= 1; 
	} 
} 

uint32_t reset(){ 
	uint32_t i; 
	//Pull line low and wait for 480uS 
	pullLineLow(); 
	delayUs(480); 
	//Release line and wait for 60uS 
	releaseLine(); 
	delayUs(60); 
	//Store line value and wait until the completion of 480uS period 
	i = readLineState(); 
	delayUs(420); 
	//Return the value read from the presence pulse (0=OK, 1=WRONG) 
	return i; 
} 

/*!
 @brief Initializes DS18B20 sensor driver.
*/

void ds18b20Init(uint32_t portNumber, uint32_t bitPosition, volatile uint32_t *ioconReg) {
    portNum = portNumber;
    bitPos = bitPosition;
    timer16Init(1, TIMER16_DEFAULTINTERVAL);
    timer16Enable(1);
    gpioSetPullup(ioconReg, gpioPullupMode_Inactive);
}

/*!
    @brief Returns temperature read from BS18B20
 
    @note Reading is returned mutiplied by 10000 i.e. 
          for 25.8750 degrees the result will be 258750
*/
uint32_t ds18b20GetTemparature(){ 
	//Reset, skip ROM and start temperature conversion 
	if(reset()) {
        printf("DS18B20 is not responding%s", CFG_PRINTF_NEWLINE);
        return 0;
    }
	writeByte(CMD_SKIPROM); 
	writeByte(CMD_CONVERTTEMP); 
    	
    //Wait until conversion is complete 
    uint16_t timeout = 0xFFFF;
	while(!readBit() && timeout > 0) {
        timeout--;    
    }
    if(0 == timeout) {
        printf("BS18B20 temperature conversion has timed out%s", CFG_PRINTF_NEWLINE);
        return 0;
    }

	//Reset, skip ROM and send command to read Scratchpad 
	if(reset()) {
        printf("DS18B20 is not responding%s", CFG_PRINTF_NEWLINE);
        return 0;
    }
	writeByte(CMD_SKIPROM); 
	writeByte(CMD_RSCRATCHPAD); 
	
	uint8_t scratchpad[2] = {0, 0};
    //Read Scratchpad (only 2 first bytes) 
	scratchpad[0] = readByte(); 
	scratchpad[1] = readByte(); 
	
    //Store temperature integer digits and decimal digits 
	int8_t digit = scratchpad[0] >> 4; 
	digit |= (scratchpad[1] & 0x7) << 4; 
	//Store decimal digits 
	uint16_t decimal = scratchpad[0] & 0xf; 
	decimal *= DECIMAL_STEPS_12BIT; 
    
    return digit * 10000 + decimal;
}

