/**************************************************************************/
/*! 
    @file     ina219.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Driver for the TI INA219 current/power monitor

    @section DESCRIPTION

    The INA219 is an I2C-based current/power monitor that monitors the
	voltage drop across a shunt resistor, as well as the supply voltage.

    @section EXAMPLE
    @code
    ina219Init();

    int16_t current = 0;
    int16_t power = 0;
    int16_t current_mA = 0;
    int16_t power_mW = 0;
    int16_t busvoltage = 0;
    int16_t shuntvoltage = 0;
    int16_t loadVoltage = 0;

    while(1)
    {
      shuntvoltage  = ina219GetShuntVoltage();
      busvoltage    = ina219GetBusVoltage();
      power         = ina219GetPower();
      current       = ina219GetCurrent();
      power_mW      = ina219GetPower_mW();
      current_mA    = ina219GetCurrent_mA();
      loadVoltage   = busvoltage + (shuntvoltage / 100);
      printf("%-15s %6d = %d.%dmV (%duV) \r\n", "Shunt Voltage:", shuntvoltage, shuntvoltage / 100, shuntvoltage % 100, shuntvoltage * 10);
      printf("%-15s %6d = %d.%dV \r\n",         "Bus Voltage:",   busvoltage, busvoltage / 1000, busvoltage % 1000);
      printf("%-15s %6d = %d.%dV \r\n",         "Load Voltage:",  loadVoltage, loadVoltage / 1000, loadVoltage % 1000);
      printf("%-15s %6d = %dmW \r\n",           "Power:",         power, power_mW);
      printf("%-15s %6d = %dmA \r\n",           "Current:",       current, current_mA);
      printf("\r\n");
      systickDelay(5000);
    }
    @endcode

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012 Kevin Townsend
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
#include "ina219.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];

// The following multipliers are used to convert raw current and power
// values to mA and mW, taking into account the current config settings
uint32_t ina219_currentDivider_mA = 0;
uint32_t ina219_powerDivider_mW   = 0;

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
static void ina219WriteRegister (uint8_t reg, uint16_t value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 4;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = INA219_ADDRESS;        // I2C device address
  I2CMasterBuffer[1] = reg;                   // Register
  I2CMasterBuffer[2] = value >> 8;            // Upper 8-bits
  I2CMasterBuffer[3] = value & 0xFF;          // Lower 8-bits
  i2cEngine();
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
static void ina219Read16(uint8_t reg, uint16_t *value)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = INA219_ADDRESS;            // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = INA219_ADDRESS | INA219_READ;  
  i2cEngine();

  // Shift values to create properly formed integer
  *value = ((I2CSlaveBuffer[0] << 8) | I2CSlaveBuffer[1]);
}

/**************************************************************************/
/*! 
    @brief  Configures to INA219 to be able to measure up to 32V and 2A
            of current.  Each unit of current corresponds to 100uA, and
            each unit of power corresponds to 2mW. Counter overflow
            occurs at 3.2A.
			
    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
void ina219SetCalibration_32V_2A(void)
{
  // By default we use a pretty huge range for the input voltage,
  // which probably isn't the most appropriate choice for system
  // that don't use a lot of power.  But all of the calculations
  // are shown below if you want to change the settings.  You will
  // also need to change any relevant register settings, such as
  // setting the VBUS_MAX to 16V instead of 32V, etc.

  // VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
  // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
  // RSHUNT = 0.1               (Resistor value in ohms)
  
  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 3.2A
  
  // 2. Determine max expected current
  // MaxExpected_I = 2.0A
  
  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.000061              (61µA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0,000488              (488µA per bit)
  
  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.0001 (100µA per bit)
  
  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 4096 (0x1000)
  
  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.002 (2mW per bit)
  
  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 3.2767A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.32V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If
  
  // 8. Computer the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 3.2 * 32V
  // MaximumPower = 102.4W
  
  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 10;  // Current LSB = 100uA per bit (1000/100 = 10)
  ina219_powerDivider_mW = 2;     // Power LSB = 1mW per bit (2/1)

  // Set Calibration register to 'Cal' calculated above	
  ina219WriteRegister(INA219_REG_CALIBRATION, 0x1000);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219WriteRegister(INA219_REG_CONFIG, config);
}

/**************************************************************************/
/*! 
    @brief  Configures to INA219 to be able to measure up to 32V and 1A
            of current.  Each unit of current corresponds to 40uA, and each
            unit of power corresponds to 800µW. Counter overflow occurs at
            1.3A.
			
    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
void ina219SetCalibration_32V_1A(void)
{
  // By default we use a pretty huge range for the input voltage,
  // which probably isn't the most appropriate choice for system
  // that don't use a lot of power.  But all of the calculations
  // are shown below if you want to change the settings.  You will
  // also need to change any relevant register settings, such as
  // setting the VBUS_MAX to 16V instead of 32V, etc.

  // VBUS_MAX = 32V		(Assumes 32V, can also be set to 16V)
  // VSHUNT_MAX = 0.32	(Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
  // RSHUNT = 0.1			(Resistor value in ohms)

  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 3.2A

  // 2. Determine max expected current
  // MaxExpected_I = 1.0A

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.0000305             (30.5µA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0.000244              (244µA per bit)

  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.0000400 (40µA per bit)

  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 10240 (0x2800)

  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.0008 (800µW per bit)

  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 1.31068A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // ... In this case, we're good though since Max_Current is less than MaxPossible_I
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.131068V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If

  // 8. Computer the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 1.31068 * 32V
  // MaximumPower = 41.94176W

  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 25;      // Current LSB = 40uA per bit (1000/40 = 25)
  ina219_powerDivider_mW = 1;         // Power LSB = 800µW per bit

  // Set Calibration register to 'Cal' calculated above	
  ina219WriteRegister(INA219_REG_CALIBRATION, 0x2800);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219WriteRegister(INA219_REG_CONFIG, config);
}

/**************************************************************************/
/*! 
    @brief  Configures to INA219 to be able to measure up to 16V and 500mA
            of current.  Each unit of current corresponds to 25uA, and each
            unit of power corresponds to 500µW. Counter overflow occurs at
            800mA.
			
    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
void ina219SetCalibration_16V_500mA(void)
{
  // VBUS_MAX = 16V
  // VSHUNT_MAX = 0.08	(Assumes Gain 2, 80mV, can also be 0.32, 0.16, 0.04)
  // RSHUNT = 0.1			(Resistor value in ohms)

  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 0.8A

  // 2. Determine max expected current
  // MaxExpected_I = 0.5A

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.0000153             (15.3µA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0.0001221             (122µA per bit)

  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.0000250 (25µA per bit)

  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 16384 (0x4000)

  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.0005 (500µW per bit)

  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 0.819175 (819 mA before overflow)
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_Current_Before_Overflow = 0.8A
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.8 * 0.1
  // Max_ShuntVoltage = 0.08V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If

  // 8. Computer the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 0.8 * 16V
  // MaximumPower = 12.8W

  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 40;    // Current LSB = 25uA per bit (1000/25 = 40)
  ina219_powerDivider_mW =    1;    // Power LSB = 500µW per bit

  // Set Calibration register to 'Cal' calculated above	
  ina219WriteRegister(INA219_REG_CALIBRATION, 0x4000);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_2_80MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219WriteRegister(INA219_REG_CONFIG, config);
}

/**************************************************************************/
/*! 
    @brief  Configures to INA219 to be able to measure up to 16V and 200mA
            of current.  Each unit of current corresponds to 10uA, and each
            unit of power corresponds to 200µW. Counter overflow occurs at
            327mA.
			
    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
void ina219SetCalibration_16V_200mA(void)
{
  // VBUS_MAX = 16V
  // VSHUNT_MAX = 0.04	(Assumes Gain 1, 40mV, can also be 0.32, 0.16, 0.08)
  // RSHUNT = 0.1			(Resistor value in ohms)

  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 0.4A

  // 2. Determine max expected current
  // MaxExpected_I = 0.2A

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.000006104	          (6.104µA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0,000048828           (48.82µA per bit)

  // 4. Choose an LSB between the min and max values
  // CurrentLSB = 0.000010 (10µA per bit)

  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 40960 (0xA000)

  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.0002 (200µW per bit)

  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 0.32767 (328 mA before overflow)
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.032767V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If

  // 8. Computer the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 0.32767 * 16V
  // MaximumPower = 5.24W

  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 100;      // Current LSB = 10uA per bit (1000/10 = 100)
  ina219_powerDivider_mW =     1;      // Power LSB = 200µW per bit

  // Set Calibration register to 'Cal' calculated above	
  ina219WriteRegister(INA219_REG_CALIBRATION, 0xA000);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  ina219WriteRegister(INA219_REG_CONFIG, config);
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
void ina219Init(void)
{
  // Reset INA219 (set to default values)
  ina219WriteRegister(INA219_REG_CONFIG, INA219_CONFIG_RESET);

  // Setup chip for 32V and 2A by default
  ina219SetCalibration_32V_2A();
}

/**************************************************************************/
/*! 
    @brief  Gets the shunt voltage (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t ina219GetShuntVoltage(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_SHUNTVOLTAGE, &value);
  return value;
}

/**************************************************************************/
/*! 
    @brief  Gets the shunt voltage (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t ina219GetBusVoltage(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_BUSVOLTAGE, &value);
  // Shift to the right 3 to drop CNVR and OVF and then multiply by LSB
  return (value >> 3) * 4;
}

/**************************************************************************/
/*! 
    @brief  Gets the raw power value (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t ina219GetPower(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_POWER, &value);
  return value;
}

/**************************************************************************/
/*! 
    @brief  Gets the power value in mW, taking into account the config
            settings and power LSB
*/
/**************************************************************************/
int16_t ina219GetPower_mW(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_POWER, &value);
  return value / ina219_powerDivider_mW;
}

/**************************************************************************/
/*! 
    @brief  Gets the raw current value (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t ina219GetCurrent(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_CURRENT, &value);
  return value;
}

/**************************************************************************/
/*! 
    @brief  Gets the current value in mA, taking into account the
            config settings and current LSB
*/
/**************************************************************************/
int16_t ina219GetCurrent_mA(void)
{
  uint16_t value;
  ina219Read16(INA219_REG_CURRENT, &value);
  return value / ina219_currentDivider_mA;
}


