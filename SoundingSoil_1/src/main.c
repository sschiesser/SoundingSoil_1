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
#include "conf_audio.h"
#include "conf_spi.h"
#include "sd_management.h"

static volatile bool main_b_msc_enable = false;
//! Structure for SPI module connected to ADC
struct spi_module adc_spi_module;
//! Structure for SPI slave (ADC device)
struct spi_slave_inst adc_spi_slave;
//! Structure for audio synchronization (TCC)
struct tcc_module audio_syncer_module;
//! Struct for the RTC calendar
struct rtc_module rtc_instance;
//! Structure for UART module connected to CDC
struct usart_module cdc_uart_module;

/********  DMA TEST ********/
#define BUF_LENGTH 20
//! Structure for DMA resource
struct dma_resource dma_resource_tx;
struct dma_resource dma_resource_rx;
static volatile bool transfer_tx_done = false;
static volatile bool transfer_rx_done = false;
static const uint8_t buffer_tx[BUF_LENGTH] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
};
uint8_t buffer_rx[BUF_LENGTH];
/********  DMA TEST ********/

//! Flags for recording, monitoring & syncing states
volatile bool rec_start_request = false;
volatile bool rec_stop_request = false;
volatile bool rec_init_done = false;
volatile bool rec_running = false;
volatile bool monitoring_on = false;
volatile bool sync_reached = false;
volatile bool chunk_full = false;
volatile bool audio_upper_buffer = false;

//! Double array for reading ADC values
uint8_t audio_buffer[2][AUDIO_CHUNK_SIZE];

//! Audio frame counter for writing chuncks
volatile uint32_t audio_frame_cnt = 0;
static uint32_t audio_total_samples = 0;

FATFS file_sys;
FIL file_object;


/********  DMA TEST ********/
COMPILER_ALIGNED(16)
DmacDescriptor dma_descriptor_tx SECTION_DMAC_DESCRIPTOR;
DmacDescriptor dma_descriptor_rx SECTION_DMAC_DESCRIPTOR;

static void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = ADC_SPI_SS_PIN;
	spi_attach_slave(&adc_spi_slave, &slave_dev_config);
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mode_specific.master.baudrate = ADC_SPI_BAUDRATE;
	config_spi_master.mux_setting = ADC_SPI_MUX_SETTING;

	config_spi_master.pinmux_pad0 = ADC_SPI_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = ADC_SPI_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = ADC_SPI_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = ADC_SPI_PINMUX_PAD3;

	spi_init(&adc_spi_module, ADC_SPI_MODULE, &config_spi_master);
	spi_enable(&adc_spi_module);

}

static void configure_spi_slave(void)
{
	struct spi_config config_spi_slave;
	spi_get_config_defaults(&config_spi_slave);
	config_spi_slave.mode = SPI_MODE_SLAVE;
	config_spi_slave.mode_specific.slave.preload_enable = true;
	config_spi_slave.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
	config_spi_slave.mux_setting = ADC_SPI_MUX_SETTING;

	config_spi_slave.pinmux_pad0 = ADC_SPI_PINMUX_PAD0;
	config_spi_slave.pinmux_pad1 = ADC_SPI_PINMUX_PAD1;
	config_spi_slave.pinmux_pad2 = ADC_SPI_PINMUX_PAD2;
	config_spi_slave.pinmux_pad3 = ADC_SPI_PINMUX_PAD3;

	spi_init(&adc_spi_module, ADC_SPI_MODULE, &config_spi_slave);
	spi_enable(&adc_spi_module);

}


static void dma_transfer_tx_callback(struct dma_resource* const resource)
{
	transfer_tx_done = true;
}

static void dma_transfer_rx_callback(struct dma_resource* const resource)
{
	transfer_rx_done = true;
}

static void configure_dma_resource_tx(struct dma_resource *tx_resource)
{
	struct dma_resource_config tx_config;
	dma_get_config_defaults(&tx_config);
	tx_config.peripheral_trigger = CONF_PERIPHERAL_TRIGGER_TX;
	tx_config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_allocate(tx_resource, &tx_config);
}

static void configure_dma_resource_rx(struct dma_resource *rx_resource)
{
	struct dma_resource_config rx_config;
	dma_get_config_defaults(&rx_config);
	//rx_config.peripheral_trigger = CONF_PERIPHERAL_TRIGGER_RX;
	rx_config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_allocate(rx_resource, &rx_config);
}

static void setup_transfer_descriptor_tx(DmacDescriptor *tx_descriptor)
{
	struct dma_descriptor_config tx_descriptor_config;
	dma_descriptor_get_config_defaults(&tx_descriptor_config);
	tx_descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	tx_descriptor_config.dst_increment_enable = false;
	tx_descriptor_config.block_transfer_count = sizeof(buffer_tx)/sizeof(uint8_t);
	tx_descriptor_config.source_address = (uint32_t)buffer_tx + sizeof(buffer_tx);
	tx_descriptor_config.destination_address = (uint32_t)(&adc_spi_module.hw->SPI.DATA.reg);
	dma_descriptor_create(tx_descriptor, &tx_descriptor_config);
}

static void setup_transfer_descriptor_rx(DmacDescriptor *rx_descriptor)
{
	struct dma_descriptor_config rx_descriptor_config;
	dma_descriptor_get_config_defaults(&rx_descriptor_config);
	rx_descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	rx_descriptor_config.src_increment_enable = false;
	rx_descriptor_config.block_transfer_count = sizeof(buffer_rx)/sizeof(uint8_t);
	rx_descriptor_config.source_address = (uint32_t)(&adc_spi_module.hw->SPI.DATA.reg);
	rx_descriptor_config.destination_address = (uint32_t)buffer_rx + sizeof(buffer_rx);
	dma_descriptor_create(rx_descriptor, &rx_descriptor_config);
}
/********  DMA TEST ********/


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

static void generate_file_name(char *fn) {
	struct rtc_calendar_time current_time;
	rtc_calendar_get_time(&rtc_instance, &current_time);
	char temp_str[4] = "";
	sprintf(temp_str, "%d", SD_SLOT_NUMBER);
	fn[0] = temp_str[0];
	fn[1] = ':';
	sprintf(temp_str, "%02d", (current_time.year - 2000));
	fn[2] = temp_str[0];
	fn[3] = temp_str[1];
	sprintf(temp_str, "%02d", current_time.month);
	fn[4] = temp_str[0];
	fn[5] = temp_str[1];
	sprintf(temp_str, "%02d", current_time.day);
	fn[6] = temp_str[0];
	fn[7] = temp_str[1];
	fn[8] = '_';
	sprintf(temp_str, "%02d", current_time.hour);
	fn[9] = temp_str[0];
	fn[10] = temp_str[1];
	sprintf(temp_str, "%02d", current_time.minute);
	fn[11] = temp_str[0];
	fn[12] = temp_str[1];
	sprintf(temp_str, "%02d", current_time.second);
	fn[13] = temp_str[0];
	fn[14] = temp_str[1];
	fn[15] = '.';
	fn[16] = 'w';
	fn[17] = 'a';
	fn[18] = 'v';
	fn[19] = '\0';
	//printf("Generated fn: %s\n\r", fn);
}

void audio_in_init(void)
{
	/* Initializing the CONV pin */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(ADC_CONV_PIN, &pin_conf);
	port_pin_set_output_level(ADC_CONV_PIN, true);
	
	/* Initializing the SPI slave */
	struct spi_config config_spi_master;
	struct spi_slave_inst_config config_spi_slave;
	spi_slave_inst_get_config_defaults(&config_spi_slave);
	config_spi_slave.ss_pin = ADC_SPI_SS_PIN;
	spi_attach_slave(&adc_spi_slave, &config_spi_slave);
	
	/* Initializing the SPI master */
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = ADC_SPI_MUX_SETTING;
	config_spi_master.pinmux_pad0 = ADC_SPI_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = ADC_SPI_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = ADC_SPI_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = ADC_SPI_PINMUX_PAD3;
	config_spi_master.mode_specific.master.baudrate = ADC_SPI_BAUDRATE;
	spi_init(&adc_spi_module, ADC_SPI_MODULE, &config_spi_master);
	spi_enable(&adc_spi_module);
}

bool audio_record_init(void) {
	FRESULT res;
	//char file_name[18] = "180213_111445.wav";
	char file_name[] = "180212_140925.wav";
	uint32_t bytes;
	
	//generate_file_name(file_name);
	//printf("Generated file name: %s\n\r", &file_name);
	
	/* Mount file system */
	memset(&file_sys, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_SPI_MEM, &file_sys);
	if(res == FR_INVALID_DRIVE) {
		return false;
	}
	
	/* Open/create file */
	res = f_open(&file_object, file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) {
		printf("Error while opening file: #%d\n\r", res);
		return false;
	}
	
	/* Write wav header */
	res = f_write(&file_object, wave_header, 44, (UINT *)&bytes);
	if(res != FR_OK) {
		printf("Error while writing WAV header: #%d\n\r", res);
		f_close(&file_object);
		return false;
	}
	
	return true;
}

bool audio_record_close(void)
{
	FRESULT res;
	UINT bytes;
	bool retVal = true;
	
	((uint16_t *)&wave_header)[WAVE_FORMAT_NUM_CHANNEL_OFFSET/2] = AUDIO_NUM_CHANNELS;
	((uint16_t *)&wave_header)[WAVE_FORMAT_BITS_PER_SAMPLE_OFFSET/2] = AUDIO_BITS_PER_SAMPLE;
	((uint16_t *)&wave_header)[WAVE_FORMAT_BLOCK_ALIGN_OFFSET/2] = AUDIO_BITS_PER_SAMPLE/8 * AUDIO_NUM_CHANNELS;
	((uint32_t *)&wave_header)[WAVE_FORMAT_SAMPLE_RATE_OFFSET/4] = AUDIO_SAMPLING_RATE;
	((uint32_t *)&wave_header)[WAVE_FORMAT_BYTE_RATE_OFFSET/4] = AUDIO_SAMPLING_RATE * AUDIO_NUM_CHANNELS * AUDIO_BITS_PER_SAMPLE/8;
	((uint32_t *)&wave_header)[WAVE_FORMAT_SUBCHUNK2_SIZE_OFFSET/4] = audio_total_samples * AUDIO_BITS_PER_SAMPLE/8;
	((uint32_t *)&wave_header)[WAVE_FORMAT_CHUNK_SIZE_OFFSET/4] = (audio_total_samples * AUDIO_BITS_PER_SAMPLE/8) + 36;
	
	f_lseek(&file_object, 0);
	res = f_write(&file_object, wave_header, 44, &bytes);
	if(res != FR_OK) {
		retVal = false;
	}
	
	f_close(&file_object);
	audio_total_samples = 0;
	LED_Off(UI_LED_REC);
	
	return retVal;
}


void audio_record_1samp(bool ub) {
	uint8_t adc_vals[2];
	port_pin_set_output_level(ADC_CONV_PIN, false);
	spi_read_buffer_wait(&adc_spi_module, adc_vals, 2, 0xFF);
	port_pin_set_output_level(ADC_CONV_PIN, true);
	if(ub) {
		audio_buffer[1][0] = 0x56;
		audio_buffer[1][1] = 0x78;
		//audio_buffer[1][0] = adc_vals[0];
		//audio_buffer[1][1] = adc_vals[1];
	}
	else {
		audio_buffer[0][0] = 0x12;
		audio_buffer[0][1] = 0x34;
		//audio_buffer[0][0] =  adc_vals[0];
		//audio_buffer[0][1] = adc_vals[1];
	}
}

bool audio_write_1samp(bool ub)
{
	FRESULT res;
	uint32_t bytes;
	res = f_write(&file_object, audio_buffer[ub], 2, (UINT *)&bytes);
	//uint16_t buf[1] = {0xa5a5};
	//res = f_write(&file_object, buf, 2, (UINT *)&bytes);
	if(res != FR_OK) {
		f_close(&file_object);
		return false;
	}
	//else {
	//res = f_sync(&file_object);
	//if(res != FR_OK) {
	//f_close(&file_object);
	//return false;
	//}
	//}
	return true;
}

bool audio_write_chunck(bool ub)
{
	FRESULT res;
	UINT bytes;
	res = f_write(&file_object, (char *)audio_buffer[ub], AUDIO_CHUNK_SIZE, &bytes);
	if(res != FR_OK) {
		f_close(&file_object);
		return false;
	}
	else {
		res = f_sync(&file_object);
		if(res != FR_OK) {
			f_close(&file_object);
			return false;
		}
	}
	return true;
}

static void audio_sync_reached_callback(void)
{
	sync_reached = true;
}

void audio_sync_init(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.clock_source = GCLK_GENERATOR_0;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1;
	config_tcc.counter.period = AUDIO_SYNC_44_1KHZ_CNT;
	tcc_init(&audio_syncer_module, TCC0, &config_tcc);
	tcc_enable(&audio_syncer_module);
	//tcc_stop_counter(&audio_syncer_module);
	
	tcc_register_callback(&audio_syncer_module, (tcc_callback_t)audio_sync_reached_callback, TCC_CALLBACK_OVERFLOW);
	tcc_enable_callback(&audio_syncer_module, TCC_CALLBACK_OVERFLOW);
}
/*! \brief Main function. Execution starts here.
*/
int main(void)
{
	irq_initialize_vectors();

	// Initialize the sleep manager
	//sleepmgr_init();

	system_init();
	
	delay_init();
	
	//calendar_init();
	
	//ui_lb_init();
	//ui_powerdown();
	//ui_cdc_init();
	//ui_configure_callback();

	//sd_mmc_init();
	//memories_initialization();
	
	//audio_in_init();
	/********  DMA TEST ********/
	configure_spi_master();
	//configure_spi_slave();
	//configure_dma_resource_tx(&dma_resource_tx);
	configure_dma_resource_rx(&dma_resource_rx);
	//setup_transfer_descriptor_tx(&dma_descriptor_tx);
	setup_transfer_descriptor_rx(&dma_descriptor_rx);
	//dma_add_descriptor(&dma_resource_tx, &dma_descriptor_tx);
	dma_add_descriptor(&dma_resource_rx, &dma_descriptor_rx);
	//dma_register_callback(&dma_resource_tx, dma_transfer_tx_callback, DMA_CALLBACK_TRANSFER_DONE);
	dma_register_callback(&dma_resource_rx, dma_transfer_rx_callback, DMA_CALLBACK_TRANSFER_DONE);
	//dma_enable_callback(&dma_resource_tx, DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&dma_resource_rx, DMA_CALLBACK_TRANSFER_DONE);
	
	while(1) {
		spi_select_slave(&adc_spi_module, &adc_spi_slave, true);
		//dma_start_transfer_job(&dma_resource_tx);
		dma_start_transfer_job(&dma_resource_rx);
		while(!transfer_rx_done) {
		}
		spi_select_slave(&adc_spi_module, &adc_spi_slave, false);
		transfer_tx_done = false;
		delay_us(2000);
	}
	/********  DMA TEST ********/
	//audio_sync_init();
	
	//system_interrupt_enable_global();
	
	// Start USB stack to authorize VBus monitoring
	//udc_start();
	
	/* The main loop manages only the power mode
	* because the USB management & button detection
	* are done by interrupt */
	for(;;) {
		//if(rec_start_request) {
		///* Testing if SD card is present */
		//if(sd_test_availability()) {
		//if(audio_record_init()) {
		//rec_init_done = true;
		//}
		//rec_start_request = false;
		//}
		//}
		//
		//if(rec_stop_request) {
		//if(!audio_record_close()) {
		//printf("ERROR closing recorded file\n\r");
		//}
		//}
		//
		//if(rec_init_done) {
		//port_pin_toggle_output_level(PIN_PB12);
		//LED_On(UI_LED_REC);
		////dma_start_transfer_job(&dma_resource_rx);
		//audio_frame_cnt = 0;
		//rec_init_done = false;
		//rec_running = true;
		//}
		//
		//if(rec_running) {
		//port_pin_set_output_level(ADC_CONV_PIN, false);
		//spi_select_slave(&adc_spi_module, &adc_spi_slave, true);
		//dma_start_transfer_job(&dma_resource_rx);
		//while(!transfer_rx_done) {
		//
		//}
		//spi_select_slave(&adc_spi_module, &adc_spi_slave, false);
		//port_pin_set_output_level(ADC_CONV_PIN, true);
		//rec_running = false;
		//}
		//if(sync_reached) {
		//port_pin_toggle_output_level(PIN_PB12);
		//sync_reached = false;
		//if(transfer_rx_done) {
		//port_pin_toggle_output_level(PIN_PB12);
		//transfer_rx_done = false;
		//}
		//if(rec_running) {
		//port_pin_toggle_output_level(PIN_PB12);
		////audio_record_1samp(audio_upper_buffer);
		////audio_write_1samp(audio_upper_buffer);
		//audio_frame_cnt += 2;
		//if(audio_frame_cnt >= AUDIO_CHUNK_SIZE) {
		//audio_total_samples += audio_frame_cnt;
		//audio_frame_cnt = 0;
		//audio_write_chunck(audio_upper_buffer);
		//audio_upper_buffer = (audio_upper_buffer) ? false : true;
		//}
		//}
		//}
		
		//if (main_b_msc_enable) {
		//if (!udi_msc_process_trans()) {
		////sleepmgr_enter_sleep();
		//}
		//}
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
