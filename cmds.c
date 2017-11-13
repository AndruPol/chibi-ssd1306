/*
 * cmds.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

#include "cmds.h"
#include "usbcfg.h"

#include "ssd1306.h"
#include "fonts.h"

static SSD1306_COLOR_t color = SSD1306_COLOR_WHITE;
static const tFont *font = &Font8x13;

void cmd_on(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_off(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_cls(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_pixel(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_xy(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_putc(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_puts(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_color(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_font(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_line(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_rect(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_frect(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_circle(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_fcircle(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]);

const ShellCommand commands[] = {
  {"on", cmd_on},
  {"off", cmd_off},
  {"cls", cmd_cls},
  {"pixel", cmd_pixel},
  {"xy", cmd_xy},
  {"putc", cmd_putc},
  {"puts", cmd_puts},
  {"color", cmd_color},
  {"font", cmd_font},
  {"line", cmd_line},
  {"rect", cmd_rect},
  {"frect", cmd_frect},
  {"circle", cmd_circle},
  {"fcircle", cmd_fcircle},
  {"reboot", cmd_reboot},
  {NULL, NULL}
};

const ShellConfig cmd_cfg = {
  (BaseSequentialStream *) &CON,
  commands
};

/*
 * command functions start here
*/

/*
 * cmd on
*/
void cmd_on(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc > 0) {
		chprintf(chp, "Usage: on");
		return;
	}
	SSD1306_On();
}

/*
 * cmd off
*/
void cmd_off(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc > 0) {
		chprintf(chp, "Usage: off");
		return;
	}
	SSD1306_Off();
}

/*
 * cmd cls
*/
void cmd_cls(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc > 0) {
		chprintf(chp, "Usage: cls");
		return;
	}
	SSD1306_Fill(!color);
	SSD1306_Refresh();
}

/*
 * cmd pixel
*/
void cmd_pixel(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 2) {
		chprintf(chp, "Usage: pixel x y");
		return;
	}
	uint8_t x,y;
	x = atoi(argv[0]);
	y = atoi(argv[1]);

	SSD1306_DrawPixel(x, y, color);
	SSD1306_Refresh();
}

/*
 * cmd xy
*/
void cmd_xy(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 2) {
		chprintf(chp, "Usage: xy x y");
		return;
	}
	uint16_t x,y;
	x = atoi(argv[0]);
	y = atoi(argv[1]);

	SSD1306_GotoXY(x, y);
}

/*
 * cmd putc
*/
void cmd_putc(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 1) {
		chprintf(chp, "Usage: putc c");
		return;
	}

	char ch = *argv[0];
	SSD1306_Putc(ch, font, color);
	SSD1306_Refresh();
}

/*
 * cmd puts
*/
void cmd_puts(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 1) {
		chprintf(chp, "Usage: puts string");
		return;
	}

	SSD1306_Puts(argv[0], font, color);
	SSD1306_Refresh();
}

/*
 * cmd color
*/
void cmd_color(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 1) {
		chprintf(chp, "Usage: color 0|1");
		return;
	}

	char col = atoi(argv[0]);
	if (col == 0)
		color = SSD1306_COLOR_BLACK;
	else
		color = SSD1306_COLOR_WHITE;
}

/*
 * cmd font
*/
void cmd_font(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 1) {
		chprintf(chp, "Usage: font 0-6");
		return;
	}

	char f = atoi(argv[0]);
	if (f == 0)
		font = &Font8x13;
	if (f == 1)
		font = &Font10x17;
	if (f == 2)
		font = &Font16x24;
	if (f == 3)
		font = &Symbol15x15;
}

/*
 * cmd line
*/
void cmd_line(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 4) {
		chprintf(chp, "Usage: line x1 y1 x2 y2");
		return;
	}
	uint16_t x1, y1, x2, y2;
	x1 = atoi(argv[0]);
	y1 = atoi(argv[1]);
	x2 = atoi(argv[2]);
	y2 = atoi(argv[3]);

	SSD1306_DrawLine(x1, y1, x2, y2, color);
	SSD1306_Refresh();
}

/*
 * cmd rect
*/
void cmd_rect(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 4) {
		chprintf(chp, "Usage: rect x y w h");
		return;
	}
	uint16_t x, y, w, h;
	x = atoi(argv[0]);
	y = atoi(argv[1]);
	w = atoi(argv[2]);
	h = atoi(argv[3]);

	SSD1306_DrawRectangle(x, y, w, h, color);
	SSD1306_Refresh();
}

/*
 * cmd frect
*/
void cmd_frect(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 4) {
		chprintf(chp, "Usage: frect x y w h");
		return;
	}
	uint16_t x, y, w, h;
	x = atoi(argv[0]);
	y = atoi(argv[1]);
	w = atoi(argv[2]);
	h = atoi(argv[3]);

	SSD1306_DrawFilledRectangle(x, y, w, h, color);
	SSD1306_Refresh();
}

/*
 * cmd circle
*/
void cmd_circle(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 3) {
		chprintf(chp, "Usage: circle x y r");
		return;
	}
	uint16_t x, y, r;
	x = atoi(argv[0]);
	y = atoi(argv[1]);
	r = atoi(argv[2]);

	SSD1306_DrawCircle(x, y, r, color);
	SSD1306_Refresh();
}

/*
 * cmd fcircle
*/
void cmd_fcircle(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)chp;

	if (argc != 3) {
		chprintf(chp, "Usage: fcircle x y r");
		return;
	}
	uint16_t x, y, r;
	x = atoi(argv[0]);
	y = atoi(argv[1]);
	r = atoi(argv[2]);

	SSD1306_DrawFilledCircle(x, y, r, color);
	SSD1306_Refresh();
}

/*
 * cmd reboot
*/
void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]){
  (void)argv;
  (void)chp;

  if (argc > 0) {
    chprintf(chp, "Usage: reboot");
    return;
  }

  chThdSleepMilliseconds(100);
  NVIC_SystemReset();
}
