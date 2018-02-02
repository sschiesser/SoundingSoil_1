/*
 * sd_management.c
 *
 * Created: 02.02.2018 10:00:12
 *  Author: schiesser
 */ 

#include <asf.h>
#include "sd_management.h"

extern bool recording_request;

bool sd_test_availability(void)
{
	Ctrl_status status;
	bool retVal = true;
	do {
		status = sd_mmc_test_unit_ready(SD_SLOT_NUMBER);
		if(status == CTRL_FAIL) {
			printf("Card install fail!\n\r");
			printf("Please unplug and re-plug the card.\n\r");
			while(CTRL_NO_PRESENT != sd_mmc_check(SD_SLOT_NUMBER)) {
			}
		}
		LED_Toggle(UI_LED_1_PIN);
		LED_Toggle(UI_LED_3_PIN);
		delay_ms(300);
		if(!recording_request) {
			retVal = false;
			break;
		}
	} while(status != CTRL_GOOD);
	LED_Off(UI_LED_1_PIN);
	LED_Off(UI_LED_3_PIN);
	return retVal;
}