/*
 * audio_in.c
 *
 * Created: 23.01.2018 14:10:39
 *  Author: schiesser
 */ 

#include <asf.h>
#include "audio_in.h"
#include "conf_board.h"
#include "conf_audio.h"
#include "sd_management.h"
#include "recorder.h"

extern struct spi_module adc_spi_module;
extern struct spi_slave_inst adc_spi_slave;
extern struct tcc_module audio_syncer_module;
extern uint16_t audio_buffer[100];
extern struct rtc_module rtc_instance;
extern FIL file_object;

volatile uint32_t audio_frame_cnt = 0;

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
	struct spi_slave_inst config_spi_slave;
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

void audio_record_init(void) {
	FRESULT res;
	char *file_name;
	//char file_name[] = "0:my_file.wav";
	uint32_t bytes;
	
	generate_file_name(&file_name);
	printf("Generated file name: %s\n\r", &file_name);
	res = f_open(&file_object, (char const *)file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) {
		printf("Error while opening file: #%d\n\r", res);
	}
	res = f_write(&file_object, wave_header, 44, &bytes);
	if(res != FR_OK) {
		printf("Error while writing WAV header: #%d\n\r", res);
	}
	res = f_sync(&file_object);
	if(res != FR_OK) {
		printf("Error while syncing file: #%d\n\r", res);
	}
	res = f_close(&file_object);
	if(res != FR_OK) {
		printf("Error while closing file: #%d\n\r", res);
	}
	
	res = f_open(&file_object, (char const *)file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) {
		printf("Error while opening file: #%d\n\r", res);
	}
	res = f_write(&file_object, wave_header, 44, &bytes);
	if(res != FR_OK) {
		printf("Error while writing WAV header: #%d\n\r", res);
	}
	res = f_sync(&file_object);
	if(res != FR_OK) {
		printf("Error while syncing file: #%d\n\r", res);
	}
	res = f_close(&file_object);
	if(res != FR_OK) {
		printf("Error while closing file: #%d\n\r", res);
	}
}

void audio_record_1samp(void) {
	uint8_t adc_vals[2];
	for(uint32_t i = 0; i < 50; i++) {
		
	}
	port_pin_set_output_level(ADC_CONV_PIN, false);
	spi_read_buffer_wait(&adc_spi_module, adc_vals, 2, 0xFF);
	port_pin_set_output_level(ADC_CONV_PIN, true);
	audio_buffer[0] = ((uint16_t)adc_vals[0] << 8) || (adc_vals[1]);
}

static void audio_sync_reached_callback(void) {
	audio_record_1samp();
	audio_frame_cnt++;
	if(audio_frame_cnt >= 10000) {
		audio_frame_cnt = 0;
	}
}

void audio_sync_init(void) {
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.period = AUDIO_SYNC_44_1KHZ_CNT;
	config_tcc.compare.match[0] = AUDIO_SYNC_CONV_CNT;
	tcc_init(&audio_syncer_module, TCC0, &config_tcc);
	tcc_enable(&audio_syncer_module);
	tcc_stop_counter(&audio_syncer_module);
	
	tcc_register_callback(&audio_syncer_module, audio_sync_reached_callback, TCC_CALLBACK_OVERFLOW);
	tcc_register_callback(&audio_syncer_module, audio_sync_reached_callback, TCC_CALLBACK_CHANNEL_0);
	tcc_enable_callback(&audio_syncer_module, TCC_CALLBACK_OVERFLOW);
	tcc_enable_callback(&audio_syncer_module, TCC_CALLBACK_CHANNEL_0);
}