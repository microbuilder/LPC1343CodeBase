/**************************************************************************/
/*! 
    @file     alphanumeric.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Shows an alpha-numeric input dialogue

    @section Example

    @code 
    #include "drivers/lcd/tft/dialogues/alphanumeric.h"

    // Print results from an alpha-numeric dialogue
    char* results = alphaShowDialogue();
    printf("%s\r\n", results);

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
#include <string.h>

#include "alphanumeric.h"

#include "core/systick/systick.h"
#include "drivers/lcd/tft/lcd.h"
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/touchscreen.h"
#include "drivers/lcd/tft/fonts/dejavusans9.h"

// Background and text input region colors
#define ALPHA_COLOR_BACKGROUND    COLOR_GRAY_15
#define ALPHA_COLOR_INPUTFILL     COLOR_GRAY_30
#define ALPHA_COLOR_INPUTTEXT     COLOR_WHITE
// Button colors
#define ALPHA_COLOR_BTN_BORDER    COLOR_GRAY_30
#define ALPHA_COLOR_BTN_FILL      COLOR_GRAY_30
#define ALPHA_COLOR_BTN_FONT      COLOR_WHITE
// Active button colors
#define ALPHA_COLOR_BTNEN_BORDER  COLOR_THEME_DEFAULT_DARKER
#define ALPHA_COLOR_BTNEN_FILL    COLOR_THEME_DEFAULT_BASE
#define ALPHA_COLOR_BTNEN_FONT    COLOR_BLACK

/* This kind of messes with your head, but defining the pixel locations
   this way allows the calculator example to be rendered on another
   TFT LCD with a different screen resolution or orientation without 
   having to rewrite all the individual pixel-level code                  */

#define ALPHA_BTN_SPACING   (5)
#define ALPHA_BTN_WIDTH     ((lcdGetWidth() - (ALPHA_BTN_SPACING * 6)) / 5)
#define ALPHA_KEYPAD_TOP    ((lcdGetHeight() / 7) + (ALPHA_BTN_SPACING * 2))
#define ALPHA_BTN_HEIGHT    (((lcdGetHeight() - ALPHA_KEYPAD_TOP) - (ALPHA_BTN_SPACING * 7)) / 6)
// #define ALPHA_BTN_WIDTH     ((240 - (ALPHA_BTN_SPACING * 6)) / 5)
// #define ALPHA_KEYPAD_TOP    ((320 / 6) + (ALPHA_BTN_SPACING * 2))
// #define ALPHA_BTN_HEIGHT    (((320 - ALPHA_KEYPAD_TOP) - (ALPHA_BTN_SPACING * 7)) / 6)

/* X/Y positions for buttons */
#define ALPHA_ROW1_TOP      (ALPHA_KEYPAD_TOP + ALPHA_BTN_SPACING)
#define ALPHA_ROW2_TOP      (ALPHA_KEYPAD_TOP + ALPHA_BTN_HEIGHT + (ALPHA_BTN_SPACING * 2))
#define ALPHA_ROW3_TOP      (ALPHA_KEYPAD_TOP + ((ALPHA_BTN_HEIGHT * 2) + ((ALPHA_BTN_SPACING) *  3)))
#define ALPHA_ROW4_TOP      (ALPHA_KEYPAD_TOP + ((ALPHA_BTN_HEIGHT * 3) + ((ALPHA_BTN_SPACING) *  4)))
#define ALPHA_ROW5_TOP      (ALPHA_KEYPAD_TOP + ((ALPHA_BTN_HEIGHT * 4) + ((ALPHA_BTN_SPACING) *  5)))
#define ALPHA_ROW6_TOP      (ALPHA_KEYPAD_TOP + ((ALPHA_BTN_HEIGHT * 5) + ((ALPHA_BTN_SPACING) *  6)))
#define ALPHA_COL1_LEFT     (ALPHA_BTN_SPACING)
#define ALPHA_COL2_LEFT     ((ALPHA_BTN_SPACING * 2) + ALPHA_BTN_WIDTH)
#define ALPHA_COL3_LEFT     ((ALPHA_BTN_SPACING * 3) + (ALPHA_BTN_WIDTH * 2))
#define ALPHA_COL4_LEFT     ((ALPHA_BTN_SPACING * 4) + (ALPHA_BTN_WIDTH * 3))
#define ALPHA_COL5_LEFT     ((ALPHA_BTN_SPACING * 5) + (ALPHA_BTN_WIDTH * 4))

/* Control which 'page' is currently shown on the keypad */
static uint8_t alphaPage = 0;

/* Keeps track of the string contents */
static uint8_t alphaString[80];
static uint8_t *alphaString_ptr;

/* For quick retrieval of button X/Y locqtions */
uint32_t alphaBtnX[5], alphaBtnY[6];

/* Array showing which characters should be displayed on each alphaPage */
/* You can rearrange the keypad by modifying the array contents below   */
/* --------------------    --------------------   --------------------   --------------------
    A   B   C   D BACK      a   b   c   d BACK      .   ,   :   ; BACK     7   8   9     BACK
    E   F   G   H   I       e   f   g   h   i       '   "   (   )          4   5   6            
    J   K   L   M   N       j   k   l   m   n       ?   !   {   }          7   8   9            
    O   P   Q   R   S       o   p   q   r   s       #   &   @   ~          .   0  SPC           
    T   U   V   W SHFT      t   u   v   w SHFT      %   =   /   \  SHFT                  SHFT   
    X   Y   Z  SPC OK       x   y   z  SPC OK       +   -   _  SPC  OK                    OK      
   --------------------    --------------------   --------------------   -------------------- */
char alphaKeys[4][6][5] =  {  {  { 'A', 'B', 'C', 'D', '<' },
                                 { 'E', 'F', 'G', 'H', 'I' },
                                 { 'J', 'K', 'L', 'M', 'N' },
                                 { 'O', 'P', 'Q', 'R', 'S' },
                                 { 'T', 'U', 'V', 'W', '*' },
                                 { 'X', 'Y', 'Z', ' ', '>' }   },

                              {  { 'a', 'b', 'c', 'd', '<' }, 
                                 { 'e', 'f', 'g', 'h', 'i' },
                                 { 'j', 'k', 'l', 'm', 'n' },
                                 { 'o', 'p', 'q', 'r', 's' },
                                 { 't', 'u', 'v', 'w', '*' },
                                 { 'x', 'y', 'z', ' ', '>' }  },

                              {  { '.', ',', ':', ';', '<' }, 
                                 { '\'', '\"', '(', ')', ' ' },
                                 { '?', '!', '{', '}', ' ' },
                                 { '#', '&', '@', '~', ' ' },
                                 { '%', '=', '/', '\\', '*' },
                                 { '+', '-', '_', ' ', '>' }  },

                              {  { '7', '8', '9', ' ', '<' }, 
                                 { '4', '5', '6', ' ', ' ' },
                                 { '1', '2', '3', ' ', ' ' },
                                 { '.', '0', ' ', ' ', ' ' },
                                 { ' ', ' ', ' ', ' ', '*'},
                                 { ' ', ' ', ' ', ' ', '>' }  }  };

/**************************************************************************/
/*! 
    @brief  Renders the UI
*/
/**************************************************************************/
void alphaRenderButton(uint8_t alphaPage, uint8_t col, uint8_t row, bool selected)
{
  // Set colors depending on button state
  uint16_t border, fill, font;
  if (selected)
  {
    border = ALPHA_COLOR_BTNEN_BORDER;
    fill = ALPHA_COLOR_BTNEN_FILL;
    font = ALPHA_COLOR_BTNEN_FONT;
  }
  else
  {
    border = ALPHA_COLOR_BTN_BORDER;
    fill = ALPHA_COLOR_BTN_FILL;
    font = ALPHA_COLOR_BTN_FONT;
  }

  char c = alphaKeys[alphaPage][row][col];
  char key[2] = { alphaKeys[alphaPage][row][col], '\0' };
  // Handle special characters
  switch (c)
  {
    case '<':
      // Backspace
      drawButton (alphaBtnX[col], alphaBtnY[row], ALPHA_BTN_WIDTH, ALPHA_BTN_HEIGHT, &dejaVuSans9ptFontInfo, 7, border, fill, font, NULL); 
      drawArrow (alphaBtnX[col] + ALPHA_BTN_WIDTH / 2 - 3, alphaBtnY[row] + ALPHA_BTN_HEIGHT / 2, 7, DRAW_DIRECTION_LEFT, font);
      break;
    case '*':
      // Page Shift
      drawButton (alphaBtnX[col], alphaBtnY[row], ALPHA_BTN_WIDTH, ALPHA_BTN_HEIGHT, &dejaVuSans9ptFontInfo, 7, border, fill, font, NULL); 
      drawArrow (alphaBtnX[col] + ALPHA_BTN_WIDTH / 2, (alphaBtnY[row] + ALPHA_BTN_HEIGHT / 2) - 3, 7, DRAW_DIRECTION_UP, font);
      break;
    case '>':
      // OK
      drawButton (alphaBtnX[col], alphaBtnY[row], ALPHA_BTN_WIDTH, ALPHA_BTN_HEIGHT, &dejaVuSans9ptFontInfo, 7, border, fill, font, "OK"); 
      break;
    default:
      // Standard character
      drawButton (alphaBtnX[col], alphaBtnY[row], ALPHA_BTN_WIDTH, ALPHA_BTN_HEIGHT, &dejaVuSans9ptFontInfo, 7, border, fill, font, key); 
      break;
  }
}

/**************************************************************************/
/*! 
    @brief  Renders the UI
*/
/**************************************************************************/
void alphaRefreshScreen(void)
{
  uint8_t x, y;

  /* Draw keypad */
  for (y = 0; y < 6; y++)
  {
    for (x = 0; x < 5; x++)
    {
      alphaRenderButton(alphaPage, x, y, false);
    }
  }

  /* Render Text */
  drawRectangleRounded(ALPHA_BTN_SPACING, ALPHA_BTN_SPACING, lcdGetWidth() - 1 - ALPHA_BTN_SPACING, ALPHA_KEYPAD_TOP - ALPHA_BTN_SPACING, ALPHA_COLOR_INPUTFILL, 10, DRAW_ROUNDEDCORNERS_ALL);
  drawString(ALPHA_BTN_SPACING * 3, ALPHA_BTN_SPACING * 3, ALPHA_COLOR_INPUTTEXT, &dejaVuSans9ptFontInfo, (char *)&alphaString);
}

/**************************************************************************/
/*! 
    @brief  Processes the supplied touch data
*/
/**************************************************************************/
char alphaHandleTouchEvent(void)
{
  tsTouchData_t data;
  char result = '\0';
  uint8_t row, col;
  int32_t tsError = -1;

  // Blocking delay until a valid touch event occurs
  while (tsError)
  {
    tsError = tsWaitForEvent(&data, 0);
  }

  // Attempt to convert touch data to char
  if ((data.ylcd < ALPHA_ROW1_TOP) || (data.ylcd > ALPHA_ROW6_TOP + ALPHA_BTN_HEIGHT))
  {
    return result;
  }

  // Get column
  if ((data.xlcd > alphaBtnX[0]) && (data.xlcd < alphaBtnX[0] + ALPHA_BTN_WIDTH))
    col = 0;
  else if ((data.xlcd > alphaBtnX[1]) && (data.xlcd < alphaBtnX[1] + ALPHA_BTN_WIDTH))
    col = 1;
  else if ((data.xlcd > alphaBtnX[2]) && (data.xlcd < alphaBtnX[2] + ALPHA_BTN_WIDTH))
    col = 2;
  else if ((data.xlcd > alphaBtnX[3]) && (data.xlcd < alphaBtnX[3] + ALPHA_BTN_WIDTH))
    col = 3;
  else if ((data.xlcd > ALPHA_COL5_LEFT) && (data.xlcd < ALPHA_COL5_LEFT + ALPHA_BTN_WIDTH))
    col = 4;
  else
    return result;

  // Get row
  if ((data.ylcd > ALPHA_ROW1_TOP) && (data.ylcd < ALPHA_ROW1_TOP + ALPHA_BTN_HEIGHT))
    row = 0;
  else if ((data.ylcd > ALPHA_ROW2_TOP) && (data.ylcd < ALPHA_ROW2_TOP + ALPHA_BTN_HEIGHT))
    row = 1;
  else if ((data.ylcd > ALPHA_ROW3_TOP) && (data.ylcd < ALPHA_ROW3_TOP + ALPHA_BTN_HEIGHT))
    row = 2;
  else if ((data.ylcd > ALPHA_ROW4_TOP) && (data.ylcd < ALPHA_ROW4_TOP + ALPHA_BTN_HEIGHT))
    row = 3;
  else if ((data.ylcd > ALPHA_ROW5_TOP) && (data.ylcd < ALPHA_ROW5_TOP + ALPHA_BTN_HEIGHT))
    row = 4;
  else if ((data.ylcd > ALPHA_ROW6_TOP) && (data.ylcd < ALPHA_ROW6_TOP + ALPHA_BTN_HEIGHT))
    row = 5;
  else
    return result;

  // Match found ... update button and process the results
  alphaRenderButton(alphaPage, col, row, true);
  result = alphaKeys[alphaPage][row][col];
  
  if (result == '<')
  {
    // Trim character if backspace was pressed
    if (alphaString_ptr > alphaString)
    {
      alphaString_ptr--;
      *alphaString_ptr = '\0';
    }
  }
  else if (result == '*')
  {
    // Switch page if the shift button was pressed
    alphaPage++;
    if (alphaPage > 3)
    {
      alphaPage = 0;
    }
    // Update the UI
    alphaRefreshScreen();
  }
  else if (result == '>')
  {
    // OK button
    systickDelay(CFG_TFTLCD_TS_KEYPADDELAY);
    return '>';
  }
  else
  {
    // Add text to string buffer
    *alphaString_ptr++ = result;
  }

  // Brief delay
  systickDelay(CFG_TFTLCD_TS_KEYPADDELAY);

  // Return button to deselected state
  alphaRefreshScreen();
  return result;
}

/**************************************************************************/
/*! 
    @brief  Displays the dialogue box and waits for valid user input

    @section Example

    @code 
    #include "drivers/lcd/tft/dialogues/alphanumeric.h"

    // Print results from an alpha-numeric dialogue
    char* results = alphaShowDialogue();
    printf("%s\r\n", results);

    @endcode
*/
/**************************************************************************/
char* alphaShowDialogue(void)
{
  char result;

  /* These need to be instantiated here since the width and height of 
     the lcd is retrieved dynamically depending on screen orientation */
  alphaBtnX[0] = ALPHA_COL1_LEFT;
  alphaBtnX[1] = ALPHA_COL2_LEFT;
  alphaBtnX[2] = ALPHA_COL3_LEFT;
  alphaBtnX[3] = ALPHA_COL4_LEFT;
  alphaBtnX[4] = ALPHA_COL5_LEFT;
  alphaBtnY[0] = ALPHA_ROW1_TOP;
  alphaBtnY[1] = ALPHA_ROW2_TOP;
  alphaBtnY[2] = ALPHA_ROW3_TOP;
  alphaBtnY[3] = ALPHA_ROW4_TOP;
  alphaBtnY[4] = ALPHA_ROW5_TOP;
  alphaBtnY[5] = ALPHA_ROW6_TOP;

  /* Initialise the string buffer */
  memset(&alphaString[0], 0, sizeof(alphaString));
  alphaString_ptr = alphaString;
  alphaPage = 0;

  /* Draw the background and render the buttons */
  drawFill(ALPHA_COLOR_BACKGROUND);
  alphaRefreshScreen();

  /* Capture results until the 'OK' button is pressed */
  while(1)
  {
    result = alphaHandleTouchEvent();
    if (result == '>') return (char *)&alphaString;
  }
}
