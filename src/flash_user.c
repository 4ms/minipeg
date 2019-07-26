
#include "globals.h"

struct SystemSettings settings;

#define USER_FLASH_PAGE 0x08007C00

FLASH_Status write_settings(void)
{
	FLASH_Status status;

	flash_begin_open_program();

	status = flash_open_erase_page(USER_FLASH_PAGE);
	if (status != FLASH_COMPLETE) {
		flash_end_open_program();
		return status;
	}

	uint32_t sz = sizeof(struct SystemSettings)/2;
	settings.is_valid = VALID_SETTINGS;
	status = flash_open_program_halfword_array((uint16_t*)(&settings), USER_FLASH_PAGE, sz);

	flash_end_open_program();
	return status;
}

uint8_t read_settings(void)
{
	uint32_t sz = sizeof(struct SystemSettings)/2;
	flash_read_halfword_array((uint16_t*)(&settings), USER_FLASH_PAGE, sz);

	if (!check_settings_valid())
	{
		default_settings();
		return 0;
	}

	return 1;
}

uint8_t check_settings_valid(void)
{
	return settings.is_valid==VALID_SETTINGS;
}

void default_settings(void)
{
	default_calibration();
	settings.limit_skew = 0;
	settings.async_can_sustain = 0;
	settings.no_free_running_ping = 0;
	settings.trigout_is_trig = 0;
	settings.trigin_function = TRIGIN_IS_ASYNC;
	settings.trigout_function = TRIGOUT_IS_ENDOFFALL;
	settings.cycle_jack_behavior = CYCLE_JACK_RISING_EDGE_TOGGLES;
	settings.start_clk_time = 8000;
	settings.start_cycle_on = 1;
}
