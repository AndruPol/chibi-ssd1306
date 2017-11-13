/*
 * ssd1306_spi.h
 *
 *  Created on: 05 нояб. 2017 г.
 *      Author: andru
 */

#ifndef SSD1306_SPI_H_
#define SSD1306_SPI_H_

#define SSD1306_SPI 			SPID1

// Pin Definitions
#define SSD1306_DC_PORT         GPIOA     // Data/Command
#define SSD1306_DC_PIN          GPIOA_LED_DC
#if 0
#define SSD1306_RST_PORT        GPIOA     // Reset
#define SSD1306_RST_PIN         GPIOA_LED_RST
#endif
#define SSD1306_CS_PORT         GPIOA     // Select
#define SSD1306_CS_PIN          GPIOA_LED_NSS
#define SSD1306_SCLK_PORT       GPIOA     // Serial Clock
#define SSD1306_SCLK_PIN        GPIOA_LED_SCK
#define SSD1306_SDAT_PORT       GPIOA     // Serial Data
#define SSD1306_SDAT_PIN        GPIOA_LED_MOSI

#define CMD(c)    do { palClearPad( SSD1306_DC_PORT, SSD1306_DC_PIN); \
                       ssd1306_sendByte( c ); \
                     } while (0);
#define DATA(c)   do { palSetPad( SSD1306_DC_PORT, SSD1306_DC_PIN); \
                       ssd1306_sendByte( c ); \
                     } while (0);

void ssd1306_sendByte(uint8_t byte);
void ssd1306_sendBuffer(uint8_t *buf, uint16_t len);
void ssd1306_spi_init(void);


#endif /* SSD1306_SPI_H_ */
