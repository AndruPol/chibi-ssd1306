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

#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"
#include "cmds.h"
#include "ssd1306.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
static const ShellConfig shell_cfg = {
  (BaseSequentialStream *)&SDU1,
  commands
};


/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

// called on kernel panic
void halt(void){
	port_disable();
	while(TRUE)
	{
	}
}

/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  thread_t *shelltp = NULL;

  halInit();
  chSysInit();

  SSD1306_Init();
  SSD1306_On();
  SSD1306_Fill(SSD1306_COLOR_WHITE);
  SSD1306_Refresh();

  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  shellInit();

  SSD1306_GotoXY(0, 16);                                      //set cursor
  SSD1306_Puts("Test font", &Font8x13, SSD1306_COLOR_WHITE); //put string
  SSD1306_Refresh();

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and listen for events.
  */
  while (TRUE) {

    if (!shelltp && SDU1.config->usbp->state == USB_ACTIVE)
      shelltp = shellCreate(&shell_cfg, SHELL_WA_SIZE, NORMALPRIO);
    else if (chThdTerminatedX(shelltp)) {
      chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
      shelltp = NULL;           /* Triggers spawning of a new shell.        */
    }

   	chThdSleepMilliseconds(1000);

  }
}
