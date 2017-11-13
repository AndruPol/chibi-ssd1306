/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the STM32F103C8T6 board.
 */

/*
 * Board identifier.
 */
#define BOARD_STM32F103C
#define BOARD_NAME             "STM32F103C8T6"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F103xB

/*
 * IO pins assignments.
 */

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *  0 	- PAL_MODE_INPUT_ANALOG
 *  2 	- PAL_MODE_UNCONNECTED, implemented as push pull output 2MHz.
 *  3 	- PAL_MODE_OUTPUT_PUSHPULL, 50MHz.
 *  4 	- PAL_MODE_RESET, implemented as input.
 *  7 	- PAL_MODE_OUTPUT_OPENDRAIN, 50MHz.
 *  8	- PAL_MODE_INPUT_PULLUP or PAL_MODE_INPUT_PULLDOWN depending on ODR.
 *  0xB	- PAL_MODE_STM32_ALTERNATE_PUSHPULL, 50MHz.
 *  0xF	- PAL_MODE_STM32_ALTERNATE_OPENDRAIN, 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA0  - PAL_MODE_RESET
 * PA1  - PAL_MODE_RESET
 * PA2  - PAL_MODE_RESET
 * PA3  - PAL_MODE_RESET
 * PA4  - PAL_MODE_OUTPUT_PUSHPULL
 * PA5  - PAL_MODE_STM32_ALTERNATE_PUSHPULL
 * PA6  - PAL_MODE_OUTPUT_PUSHPULL
 * PA7  - PAL_MODE_STM32_ALTERNATE_PUSHPULL
 * PA8  - PAL_MODE_RESET
 * PA9  - PAL_MODE_RESET
 * PA10 - PAL_MODE_RESET
 * PA11 - PAL_MODE_RESET
 * PA12 - PAL_MODE_RESET
 * PA13 - PAL_MODE_RESET
 * PA14 - PAL_MODE_RESET
 * PA15 - PAL_MODE_STM32_ALTERNATE_PUSHPULL
 */

#define GPIOA_PIN0              0
#define GPIOA_PIN1              1
#define GPIOA_PIN2              2
#define GPIOA_PIN3              3
#define GPIOA_LED_NSS           4
#define GPIOA_LED_SCK           5
#define GPIOA_LED_DC            6
#define GPIOA_LED_MOSI          7
#define GPIOA_PIN8              8
#define GPIOA_PIN9              9
#define GPIOA_PIN10             10
#define GPIOA_USB_DM            11
#define GPIOA_USB_DP            12
#define GPIOA_SWDIO             13
#define GPIOA_SWCLK             14
#define GPIOA_PIN15             15

#define VAL_GPIOACRL            0xB3B38888      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x84444444      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB0  - PAL_MODE_RESET
 * PB1  - PAL_MODE_RESET
 * PB2  - PAL_MODE_RESET
 * PB3  - PAL_MODE_RESET
 * PB4  - PAL_MODE_STM32_ALTERNATE_PUSHPULL
 * PB5  - PAL_MODE_RESET
 * PB6  - PAL_MODE_STM32_ALTERNATE_OPENDRAIN
 * PB7  - PAL_MODE_RESET
 * PB8  - PAL_MODE_RESET
 * PB9  - PAL_MODE_RESET
 * PB10 - PAL_MODE_RESET
 * PB11 - PAL_MODE_RESET
 * PB12 - PAL_MODE_OUTPUT_PUSHPULL
 * PB13 - PAL_MODE_OUTPUT_PUSHPULL
 * PB14 - PAL_MODE_OUTPUT_PUSHPULL
 * PB15 - PAL_MODE_RESET
 */

#define GPIOB_PIN0              0
#define GPIOB_PIN1              1
#define GPIOB_PIN2              2
#define GPIOB_PIN3              3
#define GPIOB_PIN4              4
#define GPIOB_PIN5              5
#define GPIOB_PIN6              6
#define GPIOB_PIN7              7
#define GPIOB_PIN8              8
#define GPIOB_PIN9              9
#define GPIOB_PIN10             10
#define GPIOB_PIN11             11
#define GPIOB_PIN12             12
#define GPIOB_PIN13             13
#define GPIOB_PIN14             14
#define GPIOB_PIN15             15

#define VAL_GPIOBCRL            0x88888888      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input with pull-up except:
 */

#define GPIOC_PIN13             13

#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88888888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 */

#define VAL_GPIODCRL            0x88888888      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */

#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palSetPadMode(GPIOA, GPIOA_USB_DP, PAL_MODE_INPUT);

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) { \
 palSetPadMode(GPIOA, GPIOA_USB_DP, PAL_MODE_OUTPUT_PUSHPULL); \
 palClearPad(GPIOA, GPIOA_USB_DP); \
}

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
