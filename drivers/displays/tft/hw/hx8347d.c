/**************************************************************************/
/*!
    @file     hx8347d.c
    @author   Tauon {TauonTeilchen} Jabber ID Tauon[at]jabber.ccc.de

    @section  DESCRIPTION
	Driver for hx8347h 240x320 pixel TFT LCD displays.
    Is written for MI0283QT-2 LCD from watterott.com
    More infos: http://www.watterott.com/de/MI0283QT-2-Adapter
    http://www.watterott.com/index.php?page=product&info=1597&dl_media=3202

    @section  LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, TauonTeilchen 
    ----------------------------------------------------------------------------
    "THE ClubMate-WARE LICENSE" (Revision 42):
    JID: <Tauon@jabber.ccc.de> wrote this file. As long as you retain this notice you
    can do whatever you want with this stuff. If we meet some day, and you think
    this stuff is worth it, you can buy me a ClubMate in return Tauon
    ----------------------------------------------------------------------------
    ----------------------------------------------------------------------------
    "THE ClubMate-WARE LICENSE" (Revision 42):
    JID: <Tauon@jabber.ccc.de> schrieb diese Datei. Solange Sie diesen Vermerk nicht entfernen, k�nnen
    Sie mit dem Material machen, was Sie m�chten. Wenn wir uns eines Tages treffen und Sie
    denken, das Material ist es wert, k�nnen Sie mir daf�r ein ClubMate ausgeben. Tauon
    ----------------------------------------------------------------------------

*/
/**************************************************************************/
#include "drivers/displays/tft/hw/hx8347d.h"

#define LCD_ID                      (0x00)
#define LCD_DATA                    ((0x72)|(LCD_ID<<2))
#define LCD_REGISTER                ((0x70)|(LCD_ID<<2))

#define LCD_BACK_LIGHT              6
#define LCD_RST                     5
#define LCD_CS                      4

// Macros for control line state
#define LCD_CS_ENABLE()             GPIO_GPIO2DATA &= ~0x10  // gpioSetValue(2, 4, 0)
#define LCD_CS_DISABLE()            GPIO_GPIO2DATA |=  0x10  // gpioSetValue(2, 4, 1)
#define LCD_RST_ENABLE()            GPIO_GPIO2DATA &=  ~0x20 // gpioSetValue(2, 5, 0)
#define LCD_RST_DISABLE()           GPIO_GPIO2DATA |=   0x20 // gpioSetValue(2, 5, 1)

#define Himax ID                    0x00
#define Display_Mode_Control        0x01

#define Column_Address_Start_2      0x02
#define Column_Address_Start_1      0x03
#define Column_Address_End_2        0x04
#define Column_Address_End_1        0x05

#define Row_Address_Start_2         0x06
#define Row_Address_Start_1         0x07
#define Row_Address_End_2           0x08
#define Row_Address_End_1           0x09

#define Partial_Area_Start_Row_2    0x0A
#define Partial_Area_Start_Row_1    0x0B
#define Partial_Area_End_Row_2      0x0C
#define Partial_Area_End_Row_1      0x0D

#define TFA_REGISTER                0x0E
#define VSA_REGISTER                0x10
#define BFA_REGISTER                0x12
#define VSP_REGISTER                0x14

#define COLMOD                      0x17
#define OSC_Control_1               0x18
#define OSC_Control_2               0x19
#define Power_Control_1             0x1A
#define Power_Control_2             0x1B
#define Power_Control_3             0x1C
#define Power_Control_4             0x1D
#define Power_Control_5             0x1E
#define Power_Control_6             0x1F
#define VCOM_Control_1              0x23
#define VCOM_Control_2              0x24
#define VCOM_Control_3              0x25
#define Display_Control_1           0x26
#define Display_Control_2           0x27
#define Display_Control_3           0x28
#define Source_OP_Control_Normal    0xE8
#define Source_OP_Control_IDLE      0xE9
#define Power_Control_Internal_1    0xEA
#define Power_Control_Internal_2    0xEB
#define Source_Control_Internal_1   0xEC
#define Source_Control_Internal_2   0xED

#define OSC_Control_2_OSC_EN        0x01
#define Display_Control_3_GON       0x20
#define Display_Control_3_DTE       0x10
#define Display_Control_3_D0        0x04
#define Display_Control_3_D1        0x08
#define Power_Control_6_STB         0x01
#define Display_Mode_Control_DP_STB_S 0x40
#define Display_Mode_Control_DP_STB 0x80

uint16_t offset;

/*************************************************/
/* Private Methods                               */
/*************************************************/
void lcd_drawstart(void);
void lcd_cmd(uint16_t reg, uint16_t param);
void lcd_clear(uint16_t color);
void lcd_draw(uint16_t color);
void lcd_drawstart(void);
void lcd_drawstop(void);
void hx8347d_DisplayOnFlow(void);
void hx8347d_DisplayOffFlow(void);
void lcd_area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


void hx8347d_Scroll(uint16_t tfa,uint16_t vsa,uint16_t bfa, uint16_t vsp)
{
  lcd_cmd(TFA_REGISTER    , tfa >> 8);
  lcd_cmd(TFA_REGISTER + 1, tfa & 0xFF);

  lcd_cmd(VSA_REGISTER    , vsa >> 8);
  lcd_cmd(VSA_REGISTER + 1, vsa & 0xFF);

  lcd_cmd(BFA_REGISTER    , bfa >> 8);
  lcd_cmd(BFA_REGISTER + 1, bfa & 0xFF);

  lcd_cmd(VSP_REGISTER    , vsp>> 8);
  lcd_cmd(VSP_REGISTER + 1, vsp & 0xFF);
  lcd_cmd(0x01, 0x08); //scroll on
}

void displayOnFlow(void)
{
  lcd_cmd(Display_Control_3, 0x0038);
  systickDelay(4);
  lcd_cmd(Display_Control_3, 0x003C);
}
void displayOffFlow(void)
{
  lcd_cmd(Display_Control_3, Display_Control_3_GON | Display_Control_3_DTE | Display_Control_3_D1);
  systickDelay(4);
  lcd_cmd(Display_Control_3, Display_Control_3_D0);
}

void lcd_cmd(uint16_t reg, uint16_t param)
{
  uint8_t b_first[2];
  uint8_t b_sec[2];
  LCD_CS_ENABLE();

  b_first[0] = LCD_REGISTER;
  b_first[1] = reg;

  sspSend(0, b_first, 2);
  LCD_CS_DISABLE();

  b_sec[0] = LCD_DATA;
  b_sec[1] = param;
  LCD_CS_ENABLE();

  sspSend(0, b_sec, 2);
  LCD_CS_DISABLE();

  return;
}

void lcd_clear(uint16_t color)
{
  unsigned int i;

  lcd_area(0, 0, (hx8347dProperties.width -1), (hx8347dProperties.height-1));

  lcd_drawstart();
  for(i=(hx8347dProperties.width*hx8347dProperties.height/8); i!=0; i--)
  {
    lcd_draw(color); //1
    lcd_draw(color); //2
    lcd_draw(color); //3
    lcd_draw(color); //4
    lcd_draw(color); //5
    lcd_draw(color); //6
    lcd_draw(color); //7
    lcd_draw(color); //8
  }
  lcd_drawstop();

  return;
}

void lcd_draw(uint16_t color)
{
  // Writing data in 16Bit mode for saving a lot of time
  /* Move on only if NOT busy and TX FIFO not full. */
  while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_NOTFULL | SSP_SSP0SR_BSY_BUSY)) != SSP_SSP0SR_TNF_NOTFULL);
  SSP_SSP0DR = color;

  while ( (SSP_SSP0SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY );
  /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
  on MISO. Otherwise, when SSP0Receive() is called, previous data byte
  is left in the FIFO. */
  uint8_t Dummy = SSP_SSP0DR;
  return;
}

void lcd_drawstop(void)
{
  while ((SSP_SSP0SR &  SSP_SSP0SR_TFE_MASK ) != SSP_SSP0SR_TFE_EMPTY );
  LCD_CS_DISABLE();

  // init 8Bit SPI Mode
  SSP_SSP0CR0 &= ~SSP_SSP0CR0_DSS_MASK;
  SSP_SSP0CR0 |= SSP_SSP0CR0_DSS_8BIT;

  return;
}

void lcd_drawstart(void)
{
  LCD_CS_ENABLE();
  uint8_t b_first[2];
  uint8_t b_sec[1];
  b_first[0] = LCD_REGISTER;
  b_first[1] = 0x22;
  sspSend(0, b_first, 2);
  LCD_CS_DISABLE();

  LCD_CS_ENABLE();
  b_sec[0] = LCD_DATA;
  sspSend(0, b_sec, 1);


  // Assign config values to SSP0CR0
  // init 16Bit SPI Mode for fast data transmitting
  SSP_SSP0CR0 &= ~SSP_SSP0CR0_DSS_MASK;
  SSP_SSP0CR0 |= SSP_SSP0CR0_DSS_16BIT;

  return;
}

void lcd_area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if(hx8347dPOrientation == LCD_ORIENTATION_PORTRAIT)
  {
    y0 = ((320-offset)+ y0) % 320;
    y1 = ((320-offset)+ y1) % 320;
  }
  else
  {
    x0 = ((320-offset)+ x0) % 320;
    x1 = ((320-offset)+ x1) % 320;		
  }
  lcd_cmd(Column_Address_Start_1, (x0>>0)); //set x0
  lcd_cmd(Column_Address_Start_2, (x0>>8)); //set x0
  lcd_cmd(Column_Address_End_1  , (x1>>0)); //set x1
  lcd_cmd(Column_Address_End_2  , (x1>>8)); //set x1
  lcd_cmd(Row_Address_Start_1   , (y0>>0)); //set y0
  lcd_cmd(Row_Address_Start_2   , (y0>>8)); //set y0
  lcd_cmd(Row_Address_End_1     , (y1>>0)); //set y1
  lcd_cmd(Row_Address_End_2     , (y1>>8)); //set y1

  return;
}

void lcd_cursor(uint16_t x, uint16_t y)
{
  lcd_area(x, y, x, y);
  return;
}

void lcd_data(uint16_t c)
{
  LCD_CS_ENABLE();
  uint8_t b[3];
  b[0] = LCD_DATA;
  b[1] = c>>8;
  b[2] = c;
  sspSend(0, b, 3);

  LCD_CS_DISABLE();

  return;
}

void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  uint32_t size;
  uint16_t tmp, i;

  if(x0 > x1)
  {
    tmp = x0;
    x0  = x1;
    x1  = tmp;
  }
  if(y0 > y1)
  {
    tmp = y0;
    y0  = y1;
    y1  = tmp;
  }

  if(x1 >= hx8347dProperties.width)
  {
    x1 = hx8347dProperties.width-1;
  }
  if(y1 >=  hx8347dProperties.height)
  {
    y1 =  hx8347dProperties.height-1;
  }

  lcd_area(x0, y0, x1, y1);

  lcd_drawstart();
  size = (uint32_t)(1+(x1-x0)) * (uint32_t)(1+(y1-y0));
  tmp  = size/8;
  if(tmp != 0)
  {
    for(i=tmp; i!=0; i--)
    {
    	lcd_draw(color); //1
    	lcd_draw(color); //2
    	lcd_draw(color); //3
    	lcd_draw(color); //4
    	lcd_draw(color); //5
    	lcd_draw(color); //6
    	lcd_draw(color); //7
    	lcd_draw(color); //8
    }
    for(i=size-tmp; i!=0; i--)
    {
    	lcd_draw(color);
    }
  }
  else
  {
    for(i=size; i!=0; i--)
    {
    	lcd_draw(color);
    }
  }
  lcd_drawstop();

  return;
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/**************************************************************************/
/*!
    @brief  Configures any pins or HW and initialises the LCD controller
*/
/**************************************************************************/
void lcdInit(void)
{
  sspInit(0,0,0);

  gpioSetDir(2, LCD_CS, gpioDirection_Output);
  gpioSetDir(2, LCD_RST, gpioDirection_Output);
  gpioSetDir(2, LCD_BACK_LIGHT, gpioDirection_Output);

  //reset
  LCD_CS_DISABLE();
  systickDelay(1);
  LCD_RST_ENABLE();
  systickDelay(50);
  LCD_RST_DISABLE();
  systickDelay(50);

  //driving ability
  lcd_cmd(Power_Control_Internal_1 , 0x0000);
  lcd_cmd(Power_Control_Internal_2 , 0x0020);
  lcd_cmd(Source_Control_Internal_1, 0x000C);
  lcd_cmd(Source_Control_Internal_2, 0x00C4);
  lcd_cmd(Source_OP_Control_Normal , 0x0040);
  lcd_cmd(Source_OP_Control_IDLE   , 0x0038);
  lcd_cmd(0xF1, 0x0001);
  lcd_cmd(0xF2, 0x0010);
  lcd_cmd(0x27, 0x00A3);

  //power voltage
  lcd_cmd(Power_Control_2, 0x001B);
  lcd_cmd(Power_Control_1, 0x0001);
  lcd_cmd(VCOM_Control_2 , 0x002F);
  lcd_cmd(VCOM_Control_3 , 0x0057);

  //VCOM offset
  lcd_cmd(VCOM_Control_1, 0x008D); //for flicker adjust

  //power on
  lcd_cmd(OSC_Control_1       , 0x0036);
  lcd_cmd(OSC_Control_2       , 0x0001);        //start osc
  lcd_cmd(Display_Mode_Control, 0x0000); //wakeup
  lcd_cmd(Power_Control_6     , 0x0088);
  systickDelay(5);
  lcd_cmd(Power_Control_6, 0x0080);
  systickDelay(5);
  lcd_cmd(Power_Control_6, 0x0090);
  systickDelay(5);
  lcd_cmd(Power_Control_6, 0x00D0);
  systickDelay(5);

  //color selection
  lcd_cmd(COLMOD, 0x0005); //0x0005=65k, 0x0006=262k

  //panel characteristic
  lcd_cmd(0x36, 0x0000);

  //display on
  lcd_cmd(0x28, 0x0038);
  systickDelay(40);
  lcd_cmd(0x28, 0x003C);

  lcdSetOrientation(hx8347dPOrientation);
  offset = 0;
  return;
}
/**************************************************************************/
/*!
    @brief  Enables or disables the LCD backlight
*/
/**************************************************************************/
void lcdBacklight(bool state)
{
  gpioSetValue(2, LCD_BACK_LIGHT, state);
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTest(void)
{
  lcdFillRGB(COLOR_CYAN);
}

/**************************************************************************/
/*!
    @brief  Fills the LCD with the specified 16-bit color
*/
/**************************************************************************/
void lcdFillRGB(uint16_t data)
{
  lcd_clear(data);
}

/**************************************************************************/
/*!
    @brief  Draws a single pixel at the specified X/Y location
*/
/**************************************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if((x >= hx8347dProperties.width) ||
     (y >= hx8347dProperties.height))
  {
    return;
  }

  fillRect(x,y,x,y,color);

  return;
}

/**************************************************************************/
/*!
    @brief  Draws an array of consecutive RGB565 pixels (much
            faster than addressing each pixel individually)
*/
/**************************************************************************/
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len)
{
  lcd_area(x, y, x + len, y);
  int i;
  lcd_drawstart();
  for(i = 0; i < len; i++)
  {
    lcd_draw(*data);
    data++;
  }
  lcd_drawstop();
}

/**************************************************************************/
/*!
    @brief  Optimised routine to draw a horizontal line faster than
            setting individual pixels
*/
/**************************************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  if (x1 < x0)
  {
    // Switch x1 and x0
    uint16_t x;
    x = x1;
    x1 = x0;
    x0 = x;
  }

  if(x0 >= hx8347dProperties.width)
  {
    x0 = hx8347dProperties.width-1;
  }
  if(x1 >= hx8347dProperties.width)
  {
    x1 = hx8347dProperties.width-1;
  }
  if(y >= hx8347dProperties.height)
  {
    y = hx8347dProperties.height-1;
  }

  fillRect(x0, y, x1, y, color);
}

/**************************************************************************/
/*!
    @brief  Optimised routine to draw a vertical line faster than
            setting individual pixels
*/
/**************************************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  if (y1 < y0)
  {
    // Switch y1 and y0
    uint16_t y;
    y = y1;
    y1 = y0;
    y0 = y;
  }

  if(x >= hx8347dProperties.width)
  {
    x = hx8347dProperties.width-1;
  }

  if(y0 >= hx8347dProperties.height)
  {
    y0 = hx8347dProperties.height-1;
  }
  if(y1 >= hx8347dProperties.height)
  {
    y1 = hx8347dProperties.height-1;
  }

  fillRect(x, y0, x, y1, color);
}

/**************************************************************************/
/*!
    @brief  Gets the 16-bit color of the pixel at the specified location
*/
/**************************************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  return 0;
}

/**************************************************************************/
/*!
    @brief  Sets the LCD orientation to horizontal and vertical
*/
/**************************************************************************/
void  lcdSetOrientation(lcdOrientation_t orientation)
{
  if(orientation == LCD_ORIENTATION_LANDSCAPE)
  {
    lcd_cmd(0x16, 0x00A8); //MY=1 MX=0 MV=1 ML=0 BGR=1
    hx8347dProperties.width  = 320;
    hx8347dProperties.height = 240;
  }
  else
  {
    //lcd_cmd(0x16, 0x0008); //MY=0 MX=0 MV=0 ML=0 BGR=1
    lcd_cmd(0x16, 0x00C8); //MY=1 MX=0 MV=1 ML=0 BGR=1
    hx8347dProperties.width  = 240;
    hx8347dProperties.height = 320;
  }
  hx8347dPOrientation = orientation;
  lcd_area(0, 0, (hx8347dProperties.width-1), (hx8347dProperties.height-1));
}

/**************************************************************************/
/*!
    @brief  Gets the current screen orientation (horizontal or vertical)
*/
/**************************************************************************/
lcdOrientation_t lcdGetOrientation(void)
{
  return hx8347dPOrientation;
}

/**************************************************************************/
/*!
    @brief  Gets the width in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetWidth(void)
{
  return hx8347dProperties.width;
}

/**************************************************************************/
/*!
    @brief  Gets the height in pixels of the LCD screen (varies depending
            on the current screen orientation)
*/
/**************************************************************************/
uint16_t lcdGetHeight(void)
{
  return hx8347dProperties.height;
}

/**************************************************************************/
/*!
    @brief  Scrolls the contents of the LCD screen vertically the
            specified number of pixels using a HW optimised routine
*/
/**************************************************************************/

void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  hx8347d_Scroll(0,320,0,(offset + pixels) % 320);
  if(hx8347dPOrientation == LCD_ORIENTATION_PORTRAIT)
  {
    fillRect(0, hx8347dProperties.height-pixels, hx8347dProperties.width, hx8347dProperties.height, fillColor);
  }
  else
  {
    fillRect(hx8347dProperties.width-pixels, 0, hx8347dProperties.width, hx8347dProperties.height, fillColor);
  }
  offset = (offset + pixels) % 320;
}
/**************************************************************************/
/*!
    @brief  Set LCD into standby
            In deep standby: LCD has lower power consumption
*/
/**************************************************************************/
void hx8347d_Standby(bool deep)
{
  displayOffFlow();
  lcd_cmd(Power_Control_6     , Power_Control_6_STB);
  if(deep)
  {
    lcd_cmd(Display_Mode_Control,Display_Mode_Control_DP_STB_S);
    lcd_cmd(Display_Mode_Control, Display_Mode_Control_DP_STB);
  }
  lcd_cmd(OSC_Control_2, ~OSC_Control_2_OSC_EN);
}
/**************************************************************************/
/*!
    @brief  Wakeup  LCD from standby
            Wakeup from deep standby you need min. 20ms
*/
/**************************************************************************/
void hx8347d_Wakeup(bool deep)
{
  lcd_cmd(OSC_Control_2, OSC_Control_2_OSC_EN);

  if(deep)
  {
    lcd_cmd(Display_Mode_Control,0x0000);
    systickDelay(20);
    lcd_cmd(Display_Mode_Control, 0x0000);
  }
  else
  {
    systickDelay(5);
  }

  lcd_cmd(Power_Control_6, 0x00D0);
  displayOnFlow();
}

/**************************************************************************/
/*!
    @brief  Gets the controller's 16-bit (4 hexdigit) ID
*/
/**************************************************************************/
uint16_t lcdGetControllerID(void)
{
  return 0x0;
}

/**************************************************************************/
/*!
    @brief  Returns the LCDs 'lcdProperties_t' that describes the LCDs
            generic capabilities and dimensions
*/
/**************************************************************************/
lcdProperties_t lcdGetProperties(void)
{
  return hx8347dProperties;
}

