/*
 * system_mode.h
 */

#pragma once

#include <stdint.h>

enum SystemModeParams {
	SET_TRIGOUT_FUNC,
	SET_TRIGOUT_IS_TRIG,
	SET_LIMIT_SKEW,
	SET_FREE_RUNNING_PING,
	SET_TRIGIN_FUNCTION,
	SET_CYCLEJACK_FUNCTION,

	NUM_SYSMODE_PARAMS
};

void handle_system_mode(void);
