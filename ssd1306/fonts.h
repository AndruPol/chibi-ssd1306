/*
 * Font.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: andru
 */

#ifndef SSD1306_FONT_H_
#define SSD1306_FONT_H_

typedef struct {
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

typedef struct {
    long int code;
    const tImage *image;
} tChar;

typedef struct {
    int length;
    const tChar *chars;
} tFont;

#define SYM_SPACE	0x20
#define SYM_OUTPUT	0x21
#define SYM_SOUND	0x22
#define SYM_LED		0x23
#define SYM_USB		0x24
#define SYM_TEMP	0x25
#define SYM_HUM		0x26
#define SYM_CO2		0x27
#define SYM_OFF		0x28
#define SYM_ON		0x29


extern const tFont Font8x13;
extern const tFont Font10x17;
extern const tFont Font16x24;
extern const tFont Symbol15x15;

#endif /* SSD1306_FONT_H_ */
