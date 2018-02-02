/**
 * \file
 *
 * \brief Main functions for MSC example
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

#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "audio_in.h"
#include "sd_management.h"

static volatile bool main_b_msc_enable = false;
//! Structure for UART module connected to CDC
struct usart_module cdc_uart_module;
//! Structure for SPI module connected to ADC
struct spi_module adc_spi_module;
//! Structure for SPI slave (ADC device)
struct spi_slave_inst adc_spi_slave;
//! Structure for audio synchronization (TCC)
struct tcc_module audio_syncer_module;
//! Bools for recording & monitoring state
bool recording_on = false;
bool recording_request = false;
bool monitoring_on = false;
//! Array for read ADC values
uint16_t audio_buffer[100] = {0};
//! Struct for the RTC calendar
struct rtc_module rtc_instance;

static void calendar_init(void)
{
	struct rtc_calendar_config config_rtc_calendar;
	rtc_calendar_get_config_defaults(&config_rtc_calendar);
	config_rtc_calendar.clock_24h = true;
	rtc_calendar_init(&rtc_instance, RTC, &config_rtc_calendar);
	rtc_calendar_enable(&rtc_instance);
	
	struct rtc_calendar_time current_time;
	current_time.year = 2018;
	current_time.month = 02;
	current_time.day = 02;
	current_time.hour = 11;
	current_time.minute = 39;
	current_time.second = 42;
	rtc_calendar_set_time(&rtc_instance, &current_time);
	rtc_calendar_swap_time_mode(&rtc_instance);
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	system_init();
	
	delay_init();
	
	calendar_init();
	
	ui_lb_init();
	ui_powerdown();
	ui_cdc_init();
	
	audio_in_init();
	audio_sync_init();
	
	sd_mmc_init();

	memories_initialization();

	ui_configure_callback();
	
	// Start USB stack to authorize VBus monitoring
	udc_start();
	
	/* The main loop manages only the power mode
	 * because the USB management & button detection
	 * are done by interrupt */
	while (true) {
		if(recording_request) {
			/* Testing if SD card is present */
			if(sd_test_availability()) {
				/* Mounting file system from SD card */
				if(sd_mount_fs()) {
					printf("SD card mounted!\n\r");
					audio_record_init();
					recording_on = true;
				}
				else {
					printf("Invalid drive!!\n\r");
				}
				recording_request = false;
			}
		}
		
		else if(recording_on) {

		}
		
		else if (main_b_msc_enable) {
			if (!udi_msc_process_trans()) {
				//sleepmgr_enter_sleep();
			}
		}
		//else {
			//sleepmgr_enter_sleep();
		//}
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_msc_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

/**
 * \mainpage ASF USB Device MSC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device Mass Storage
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * The example uses all memories available on the board and connects these to
 * USB Device Mass Storage stack. After loading firmware, connect the board
 * (EVKxx,Xplain,...) to the USB Host. When connected to a USB host system
 * this application allows to display all available memories as a
 * removable disks in the Unix/Mac/Windows operating systems.
 * \note
 * This example uses the native MSC driver on Unix/Mac/Windows OS, except for Win98.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/msc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds)
 *
 * <SUP>1</SUP> The memory data transfers are done outside USB interrupt routine.
 * This is done in the MSC process ("udi_msc_process_trans()") called by main loop.
 */
