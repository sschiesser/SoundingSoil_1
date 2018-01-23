/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "ui.h"

#define  LED_On(led_pin)          port_pin_set_output_level(led_pin, UI_LED_ACTIVE)
#define  LED_Off(led_pin)         port_pin_set_output_level(led_pin, UI_LED_INACTIVE)

void ui_init(void)
{
	/* Initialize LEDs */
	//LED_On(LED_0_PIN);
	LED_On(UI_LED_1_PIN);
	LED_On(UI_LED_2_PIN);
	LED_On(UI_LED_3_PIN);
}

void ui_powerdown(void)
{
	//LED_Off(LED_0_PIN);
	LED_Off(UI_LED_1_PIN);
	LED_Off(UI_LED_2_PIN);
	LED_Off(UI_LED_3_PIN);
}

void ui_wakeup(void)
{
	//LED_On(LED_0_PIN);
	LED_On(UI_LED_1_PIN);
	LED_On(UI_LED_2_PIN);
	LED_On(UI_LED_3_PIN);
}

void ui_start_read(void)
{
}

void ui_stop_read(void)
{
}

void ui_start_write(void)
{
}

void ui_stop_write(void)
{
}

void ui_process(uint16_t framenumber)
{
	if (0 == framenumber) {
		//LED_On(LED_0_PIN);
		LED_On(UI_LED_1_PIN);
		LED_On(UI_LED_2_PIN);
		LED_On(UI_LED_3_PIN);
	}
	if (1000 == framenumber) {
		//LED_Off(LED_0_PIN);
		LED_Off(UI_LED_1_PIN);
		LED_Off(UI_LED_2_PIN);
		LED_Off(UI_LED_3_PIN);
	}
}


/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM D21 Xplained Pro
 * - LED0 blinks when USB host has checked and enabled MSC interface
 *
 */