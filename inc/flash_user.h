/*
 * flash_user.h
 */

#pragma once

#include <stdint.h>
#include <stm32g4xx.h>

#include "envelope_calcs.h"
enum CycleJackBehaviors {
	CYCLE_JACK_RISING_EDGE_TOGGLES,
	CYCLE_JACK_BOTH_EDGES_TOGGLES,

	NUM_CYCLEJACK_BEHAVIORS
};

enum AuxTrigJackAssignment {
	AUX_ENV_TRIG,
	CYCLE_TOGGLE,

	NUM_AUX_TRIG_JACK_ASSIGNMENTS
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

enum CenterDetentPots {
	DET_SCALE,
	DET_OFFSET,
	DET_SHAPE,

	NUM_CENTER_DETENT_POTS
};

#define VALID_SETTINGS 0xC002

struct SystemSettings {
	uint16_t is_valid;
	uint16_t midpt_array[NUM_DIVMULTS];
	int16_t center_detent_offset[NUM_CENTER_DETENT_POTS];

	uint8_t limit_skew;
	uint8_t free_running_ping;
	uint8_t trigout_is_trig;
	enum TrigInFunctions trigin_function;
	enum TrigOutFunctions trigout_function;

	enum AuxTrigJackAssignment auxtrigin_assignment;
	enum TrigInFunctions auxtrigin_function;
	enum CycleJackBehaviors cycle_jack_behavior;

	uint32_t start_clk_time;
	uint8_t start_cycle_on;

	uint16_t ping_cal_r, ping_cal_g, ping_cal_b;
	uint16_t cycle_cal_r, cycle_cal_g, cycle_cal_b;
	uint16_t lock_cal_r, lock_cal_g, lock_cal_b;
	uint16_t enva_cal_r, enva_cal_g, enva_cal_b;
	uint16_t envb_cal_r, envb_cal_g, envb_cal_b;

	int32_t shift_value;
};

//The following are not user modifiable, change at your own risk!
#define QNT_REPHASES_WHEN_CYCLE_OFF 0
#define CYCLE_REPHASES_DIV_PING 1

HAL_StatusTypeDef write_settings(void);
uint8_t read_settings(void);
uint8_t check_settings_valid(void);
void default_settings(void);
