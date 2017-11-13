/*
 * ssd1306_spi.c
 *
 *  Created on: 05 нояб. 2017 г.
 *      Author: andru
 */

#include "hal.h"
#include "ssd1306_spi.h"

/*
 * SPI speed definitions.
 */
#define   SPI_DIV2	0
#define   SPI_DIV4  (SPI_CR1_BR_0)
#define   SPI_DIV8  (SPI_CR1_BR_1)
#define   SPI_DIV16 (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define   SPI_DIV32 (SPI_CR1_BR_2)
#define   SPI_DIV64 (SPI_CR1_BR_2 | SPI_CR1_BR_0)

/*
 * SPI configuration structure.
 */
static const SPIConfig ssd1306_spicfg = {
	NULL,
	SSD1306_CS_PORT,
	SSD1306_CS_PIN,
	SPI_DIV2,
};

void ssd1306_spi_init(void) {
	// DC pin setup
	palSetPadMode(SSD1306_DC_PORT, SSD1306_DC_PIN, PAL_MODE_OUTPUT_PUSHPULL);     			 /* DC.*/
	palClearPad( SSD1306_DC_PORT, SSD1306_DC_PIN);

	spiStart(&SSD1306_SPI, &ssd1306_spicfg);
	/* SPI1 I/O pins setup. */
	palSetPadMode(SSD1306_SCLK_PORT, SSD1306_SCLK_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   /* SCK. */
	palSetPadMode(SSD1306_SDAT_PORT, SSD1306_SDAT_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);   /* MOSI.*/
	palSetPadMode(SSD1306_CS_PORT, SSD1306_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL);     			 /* CS.*/
	palSetPad(SSD1306_CS_PORT, SSD1306_CS_PIN);

	// Reset the LCD
#if defined(SSD1306_RST_PORT) && defined(SSD1306_RST_PIN)
	palClearPad(SSD1306_RST_PORT, SSD1306_RST_PIN);
	chThdSleepMilliseconds(10);
	palSetPad(SSD1306_RST_PORT, SSD1306_RST_PIN);
	chThdSleepMilliseconds(10);
#else
	chThdSleepMilliseconds(300);	// -splash dots on power up
#endif
}

/**************************************************************************/
/*
    @brief SPI write
    @param[in]  byte
                The byte to send
*/
/**************************************************************************/
//inline
void ssd1306_sendByte(uint8_t byte) {
	spiAcquireBus(&SSD1306_SPI);
	spiSelect(&SSD1306_SPI);
	spiSend(&SSD1306_SPI, 1, &byte);
	spiUnselect(&SSD1306_SPI);
	spiReleaseBus(&SSD1306_SPI);
}

void ssd1306_sendBuffer(uint8_t *buf, uint16_t len) {
	palSetPad(SSD1306_DC_PORT, SSD1306_DC_PIN);

	spiAcquireBus(&SSD1306_SPI);
	spiSelect(&SSD1306_SPI);
	spiSend(&SSD1306_SPI, len, buf);
	spiUnselect(&SSD1306_SPI);
	spiReleaseBus(&SSD1306_SPI);
}
