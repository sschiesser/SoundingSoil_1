/**
 * \file
 *
 * \brief Common User Interface for MSC application
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#ifndef _UI_H_
#define _UI_H_

//#define UI_LED_ACTIVE			false
//#define UI_LED_INACTIVE			!UI_LED_ACTIVE
//#define UI_BUT_ACTIVE			false
//#define UI_BUT_INACTIVE			!UI_BUT_ACTIVE
//
///* LED definitions */
//#define UI_LED_1_NAME			"LED1"
//#define UI_LED_1_PIN			PIN_PA12
//#define UI_LED_1_ACTIVE			UI_LED_ACTIVE
//#define UI_LED_1_INACTIVE		UI_LED_INACTIVE
//
//#define UI_LED_2_NAME			"LED2"
//#define UI_LED_2_PIN			PIN_PA13
//#define UI_LED_2_ACTIVE			UI_LED_ACTIVE
//#define UI_LED_2_INACTIVE		UI_LED_INACTIVE
//
//#define UI_LED_3_NAME			"LED3"
//#define UI_LED_3_PIN			PIN_PA15
//#define UI_LED_3_ACTIVE			UI_LED_ACTIVE
//#define UI_LED_3_INACTIVE		UI_LED_INACTIVE
//
///* Buttons definitions */
//#define UI_BUT_1_NAME			"SW1"
//#define UI_BUT_1_PIN			PIN_PA28
//#define UI_BUT_1_ACTIVE			UI_BUT_ACTIVE
//#define UI_BUT_1_INACTIVE		UI_BUT_INACTIVE
//#define UI_BUT_1_EIC_PIN		PIN_PA28A_EIC_EXTINT8
//#define UI_BUT_1_MUX			MUX_PA28A_EIC_EXTINT8
//#define UI_BUT_1_PINMUX			PINMUX_PA28A_EIC_EXTINT8
//#define UI_BUT_1_EIC_LINE		8
//
//#define UI_BUT_2_NAME			"SW2"
//#define UI_BUT_2_PIN			PIN_PA02
//#define UI_BUT_2_ACTIVE			UI_BUT_ACTIVE
//#define UI_BUT_2_INACTIVE		UI_BUT_INACTIVE
//#define UI_BUT_2_EIC_PIN		PIN_PA02A_EIC_EXTINT2
//#define UI_BUT_2_MUX			MUX_PA02A_EIC_EXTINT2
//#define UI_BUT_2_PINMUX			PINMUX_PA02A_EIC_EXTINT2
//#define UI_BUT_2_EIC_LINE		2
//
//#define UI_BUT_3_NAME			"SW3"
//#define UI_BUT_3_PIN			PIN_PA03
//#define UI_BUT_3_ACTIVE			UI_BUT_ACTIVE
//#define UI_BUT_3_INACTIVE		UI_BUT_INACTIVE
//#define UI_BUT_3_EIC_PIN		PIN_PA03A_EIC_EXTINT3
//#define UI_BUT_3_MUX			MUX_PA03A_EIC_EXTINT3
//#define UI_BUT_3_PINMUX			PINMUX_PA03A_EIC_EXTINT3
//#define UI_BUT_3_EIC_LINE		3


//! \brief Initializes the user interface
void ui_init(void);

//! \brief Enters the user interface in power down mode
void ui_powerdown(void);

//! \brief Exits the user interface of power down mode
void ui_wakeup(void);

//! \name Callback to show the MSC read and write access
//! @{
void ui_start_read(void);
void ui_stop_read(void);
void ui_start_write(void);
void ui_stop_write(void);
//! @}

/*! \brief This process is called each 1ms
 * It is called only if the USB interface is enabled.
 *
 * \param framenumber  Current frame number
 */
void ui_process(uint16_t framenumber);

#endif // _UI_H_
