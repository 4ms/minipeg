/*
 * debounced_digins.h
 */

#pragma once

#include <stdint.h>

enum DebouncedDiginList {
	PING_BUTTON,
	CYCLE_BUTTON,
	TRIGGER_JACK,
	CYCLE_JACK,
	PING_JACK,

	NUM_DEBOUNCED_DIGINS
};

typedef struct DebouncedDigin {
	uint16_t 	history;
	uint8_t 	state;
	int8_t 		edge;
} debounced_digin_t;

void init_debouncer(void);
