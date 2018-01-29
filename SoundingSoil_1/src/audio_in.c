/*
 * audio_in.c
 *
 * Created: 23.01.2018 14:10:39
 *  Author: schiesser
 */ 

#include <asf.h>
#include "audio_in.h"
#include "conf_board.h"

extern struct spi_module adc_spi_module;
extern struct spi_slave_inst adc_spi_slave;

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
	spi_init(&adc_spi_module, ADC_SPI_MODULE, &config_spi_master);
	spi_enable(&adc_spi_module);
}