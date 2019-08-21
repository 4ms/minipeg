/*
 * flash_user.h
 */

#pragma once

#include <stm32f0xx.h>

enum CycleJackBehavior {
	CYCLE_JACK_RISING_EDGE_TOGGLES,
	CYCLE_JACK_BOTH_EDGES_TOGGLE,
	NUM_CYCLEJACK_FUNCTIONS
};
enum TrigOutFunctions {
	TRIGOUT_IS_ENDOFRISE,
	TRIGOUT_IS_ENDOFFALL,
	TRIGOUT_IS_HALFRISE,
	TRIGOUT_IS_TAPCLKOUT,

	NUM_TRIGOUT_FUNCTIONS
};
enum TrigInFunctions {
	TRIGIN_IS_ASYNC,
	TRIGIN_IS_ASYNC_SUSTAIN,
	TRIGIN_IS_QNT,

	NUM_TRIGIN_FUNCTIONS
};
#define VALID_SETTINGS 0xC001

struct SystemSettings {
	uint16_t				is_valid;
	uint16_t 				midpt_array[NUM_DIVMULTS];
	uint8_t 				limit_skew;
	uint8_t 				free_running_ping;
	uint8_t 				trigout_is_trig;
	enum TrigInFunctions 	trigin_function;
	enum TrigOutFunctions 	trigout_function;
	enum CycleJackBehavior 	cycle_jack_behavior;

	uint32_t				start_clk_time;
	uint8_t					start_cycle_on;
};

FLASH_Status write_settings(void);
uint8_t read_settings(void);
uint8_t check_settings_valid(void);
void default_settings(void);
