/*
    @file     ssd1306.c
    @author   K. Townsend (microBuilder.eu)

    @section DESCRIPTION

    Driver for 128x64 OLED display based on the SSD1306 controller.

    This driver is based on the SSD1306 Library from Limor Fried
    (Adafruit Industries) at: https://github.com/adafruit/SSD1306  
    
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
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "ssd1306.h"
#include "fonts.h"
#include "ssd1306_spi.h"

#define REFRESH	  do { chBSemSignal(&ssd1306_upd); } while (0);
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static binary_semaphore_t ssd1306_upd;
ssd1306_t SSD1306;

void SSD1306_Refresh(void);

#define	SSD1306PRIO	(NORMALPRIO+1)
static THD_WORKING_AREA(waSSD1306Thread, 256);

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/
static thread_t *ssd1306Thread_p;
static THD_FUNCTION(ssd1306Thread, arg) {
  (void)arg;

  chRegSetThreadName("ssd1306Thd");
  while (!chThdShouldTerminateX()) {
	chBSemWait(&ssd1306_upd);
	SSD1306_Refresh();
  }
  chThdExit((msg_t)0);
}

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Initialises the SSD1306 LCD display
*/
/**************************************************************************/
void SSD1306_Init(void)
{
	chBSemObjectInit(&ssd1306_upd, TRUE);
	ssd1306_spi_init();

	CMD(0x20); //Set Memory Addressing Mode
	CMD(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	CMD(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	CMD(0xC8); //Set COM Output Scan Direction
	CMD(0x00); //---set low column address
	CMD(0x10); //---set high column address
	CMD(0x40); //--set start line address
	CMD(0x81); //--set contrast control register
	CMD(0xFF);
	CMD(0xA1); //--set segment re-map 0 to 127
	CMD(0xA6); //--set normal display
	CMD(0xA8); //--set multiplex ratio(1 to 64)
	CMD(0x3F); //
	CMD(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	CMD(0xD3); //-set display offset
	CMD(0x00); //-not offset
	CMD(0xD5); //--set display clock divide ratio/oscillator frequency
	CMD(0xF0); //--set divide ratio
	CMD(0xD9); //--set pre-charge period
	CMD(0x22); //
	CMD(0xDA); //--set com pins hardware configuration
	CMD(0x12);
	CMD(0xDB); //--set vcomh
	CMD(0x20); //0x20,0.77xVcc
}

void SSD1306_On(void)
{
    // Enable the OLED panel
	CMD(SSD1306_CHARGEPUMP);                    //0x8D
	CMD(0x14);
	CMD(SSD1306_DISPLAYON);

    // Creates refresh thread
    if (!ssd1306Thread_p)
    	ssd1306Thread_p = chThdCreateStatic(waSSD1306Thread, sizeof(waSSD1306Thread), SSD1306PRIO, ssd1306Thread, NULL);

    REFRESH
}

void SSD1306_Off(void)
{
	// Terminates refresh thread
	if (ssd1306Thread_p) {
		chThdTerminate(ssd1306Thread_p);
		chBSemSignal(&ssd1306_upd);
		chThdWait(ssd1306Thread_p);
		ssd1306Thread_p = NULL;
	}

    // Disable the OLED panel
	CMD(SSD1306_CHARGEPUMP);                    //0x8D
	CMD(0x10);
    CMD(SSD1306_DISPLAYOFF);
}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void SSD1306_Refresh(void)
{
	uint8_t m;

	for (m = 0; m < 8; m++) {
		CMD(0xB0 + m);
		CMD(0x00);
		CMD(0x10);

		/* Write multi data */
		ssd1306_sendBuffer(&ssd1306_buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
	}
}

void SSD1306_ToggleInvert(void) {
	uint16_t i;

	/* Toggle invert */
	SSD1306.Inverted = !SSD1306.Inverted;

	/* Do memory toggle */
	for (i = 0; i < sizeof(ssd1306_buffer); i++) {
		ssd1306_buffer[i] = ~ssd1306_buffer[i];
	}
}

void SSD1306_Fill(SSD1306_COLOR_t color) {
	/* Set memory */
	memset(ssd1306_buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(ssd1306_buffer));
}

void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR_t)!color;
	}

	/* Set color */
	if (color == SSD1306_COLOR_WHITE) {
		ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void SSD1306_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

const tChar *find_char_by_code(int code, const tFont *font)
{
    int count = font->length;
    int first = 0;
    int last = count - 1;
    int mid = 0;

    if (count > 0) {
        if ((code >= font->chars[0].code) && (code <= font->chars[count - 1].code)) {
            while (last >= first) {
                mid = first + ((last - first) / 2);

                if (font->chars[mid].code < code)
                    first = mid + 1;
                else
                    if (font->chars[mid].code > code)
                        last = mid - 1;
                    else
                        break;
            }

            if (font->chars[mid].code == code)
                return (&font->chars[mid]);
        }
    }

    return (0);
}

char SSD1306_Putc(char ch, const tFont* Font, SSD1306_COLOR_t color) {
	uint32_t i, j;

	const tChar *c = find_char_by_code(ch, Font);
	if (c == 0) return 0;

	/* Check available space in LCD */
	if (
		SSD1306_WIDTH <= (SSD1306.CurrentX + c->image->width) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + c->image->height)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < c->image->height; i++) {
		uint16_t d = c->image->data[i];
		d = d << (c->image->dataSize - c->image->width);
		for (j = 0; j < c->image->width; j++) {
			if ((d << j) & 0x8000) {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
			} else {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
			}
		}
	}

	/* Increase pointer */
	SSD1306.CurrentX += c->image->width;

	/* Return character written */
	return ch;
}

char SSD1306_Puts(char* str, const tFont* Font, SSD1306_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (SSD1306_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}


void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SSD1306_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SSD1306_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		SSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	/* Draw 4 lines */
	SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	uint8_t i;

	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		SSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	/* Draw lines */
	SSD1306_DrawLine(x1, y1, x2, y2, color);
	SSD1306_DrawLine(x2, y2, x3, y3, color);
	SSD1306_DrawLine(x3, y3, x1, y1, color);
}


void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawPixel(x0 + x, y0 + y, c);
        SSD1306_DrawPixel(x0 - x, y0 + y, c);
        SSD1306_DrawPixel(x0 + x, y0 - y, c);
        SSD1306_DrawPixel(x0 - x, y0 - y, c);

        SSD1306_DrawPixel(x0 + y, y0 + x, c);
        SSD1306_DrawPixel(x0 - y, y0 + x, c);
        SSD1306_DrawPixel(x0 + y, y0 - x, c);
        SSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}

void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);
    SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}
