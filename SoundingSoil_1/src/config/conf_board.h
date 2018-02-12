/**
 * \file
 *
 * \brief SAM D21 Xplained Pro board configuration.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

#define UI_LED_ACTIVE			false
#define UI_LED_INACTIVE			!UI_LED_ACTIVE
#define UI_BUT_ACTIVE			false
#define UI_BUT_INACTIVE			!UI_BUT_ACTIVE

/* LED definitions */
#define UI_LED_1_NAME			"LED1"
#define UI_LED_1_PIN			PIN_PA12
#define UI_LED_1_ACTIVE			UI_LED_ACTIVE
#define UI_LED_1_INACTIVE		UI_LED_INACTIVE
#define UI_LED_REC				UI_LED_1_PIN

#define UI_LED_2_NAME			"LED2"
#define UI_LED_2_PIN			PIN_PA13
#define UI_LED_2_ACTIVE			UI_LED_ACTIVE
#define UI_LED_2_INACTIVE		UI_LED_INACTIVE

#define UI_LED_3_NAME			"LED3"
#define UI_LED_3_PIN			PIN_PA15
#define UI_LED_3_ACTIVE			UI_LED_ACTIVE
#define UI_LED_3_INACTIVE		UI_LED_INACTIVE
#define UI_LED_MON				UI_LED_3_PIN

/* Buttons definitions */
#define UI_BUT_1_NAME			"SW1"
#define UI_BUT_1_PIN			PIN_PA28
#define UI_BUT_1_ACTIVE			UI_BUT_ACTIVE
#define UI_BUT_1_INACTIVE		UI_BUT_INACTIVE
#define UI_BUT_1_EIC_PIN		PIN_PA28A_EIC_EXTINT8
#define UI_BUT_1_EIC_MUX		MUX_PA28A_EIC_EXTINT8
#define UI_BUT_1_EIC_PINMUX		PINMUX_PA28A_EIC_EXTINT8
#define UI_BUT_1_EIC_LINE		8

#define UI_BUT_2_NAME			"SW2"
#define UI_BUT_2_PIN			PIN_PA02
#define UI_BUT_2_ACTIVE			UI_BUT_ACTIVE
#define UI_BUT_2_INACTIVE		UI_BUT_INACTIVE
#define UI_BUT_2_EIC_PIN		PIN_PA02A_EIC_EXTINT2
#define UI_BUT_2_EIC_MUX		MUX_PA02A_EIC_EXTINT2
#define UI_BUT_2_EIC_PINMUX		PINMUX_PA02A_EIC_EXTINT2
#define UI_BUT_2_EIC_LINE		2

#define UI_BUT_3_NAME			"SW3"
#define UI_BUT_3_PIN			PIN_PA03
#define UI_BUT_3_ACTIVE			UI_BUT_ACTIVE
#define UI_BUT_3_INACTIVE		UI_BUT_INACTIVE
#define UI_BUT_3_EIC_PIN		PIN_PA03A_EIC_EXTINT3
#define UI_BUT_3_EIC_MUX		MUX_PA03A_EIC_EXTINT3
#define UI_BUT_3_EIC_PINMUX		PINMUX_PA03A_EIC_EXTINT3
#define UI_BUT_3_EIC_LINE		3

/* CDC port settings */
#define UI_CDC_MODULE			SERCOM3
#define UI_CDC_MUX_SETTING		USART_RX_1_TX_0_XCK_1
#define UI_CDC_PINMUX_PAD0		PINMUX_PA22C_SERCOM3_PAD0
#define UI_CDC_PINMUX_PAD1		PINMUX_PA23C_SERCOM3_PAD1
#define UI_CDC_PINMUX_PAD2		PINMUX_UNUSED
#define UI_CDC_PINMUX_PAD3		PINMUX_UNUSED
#define UI_CDC_DMAC_ID_TX		SERCOM3_DMAC_ID_TX
#define UI_CDC_DMAC_ID_RX		SERCOM3_DMAC_ID_RX

/* SPI port settings for ADC device */
#define ADC_SPI_MODULE			SERCOM1
#define ADC_SPI_MUX_SETTING		SPI_SIGNAL_MUX_SETTING_C
#define ADC_SPI_SS_PIN			PIN_PB15 // Not used -> dummy pin
#define ADC_SPI_PINMUX_PAD0		PINMUX_PA16C_SERCOM1_PAD0 // MOSI
#define ADC_SPI_PINMUX_PAD1		PINMUX_PA17C_SERCOM1_PAD1 // SCK
#define ADC_SPI_PINMUX_PAD2		PINMUX_PA18C_SERCOM1_PAD2 // MISO
#define ADC_SPI_PINMUX_PAD3		PINMUX_UNUSED
#define ADC_SPI_BAUDRATE		500000
#define ADC_CONV_PIN			PIN_PA20

/* Enable USB VBUS detect */
#define CONF_BOARD_USB_VBUS_DETECT

#endif /* CONF_BOARD_H_INCLUDED */
