/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(UI_LED_1_PIN, &pin_conf);
	port_pin_set_output_level(UI_LED_1_PIN, UI_LED_INACTIVE);
	port_pin_set_config(UI_LED_2_PIN, &pin_conf);
	port_pin_set_output_level(UI_LED_2_PIN, UI_LED_INACTIVE);
	port_pin_set_config(UI_LED_3_PIN, &pin_conf);
	port_pin_set_output_level(UI_LED_3_PIN, UI_LED_INACTIVE);

	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(UI_BUT_1_PIN, &pin_conf);
	port_pin_set_config(UI_BUT_2_PIN, &pin_conf);
	port_pin_set_config(UI_BUT_3_PIN, &pin_conf);
}