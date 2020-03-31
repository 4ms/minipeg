/*
 * params.h
 */

#pragma once

#include <stm32g0xx.h>


enum AnalogParams {
	DIVMULT,
	SHAPE,
	SCALE,
	OFFSET,

	NUM_ANALOG_PARAMS
};

void update_analog(void);

