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

#define DEBOUNCE_TIMx TIM7
#define DEBOUNCE_IRQn TIM7_IRQn
#define DEBOUNCE_TIM_RCC_ENABLE __HAL_RCC_TIM7_CLK_ENABLE
#define DEBOUNCE_IRQHandler TIM7_IRQHandler
void init_debouncer(void);
