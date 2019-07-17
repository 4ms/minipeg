/*
 * debounced_digins.h
 */

#pragma once

#include <stm32f0xx.h>

enum DebouncedDiginList {
	PING_BUTTON,
	CYCLE_BUTTON,
	TRIGGER_JACK,
	CYCLE_JACK,

	NUM_DEBOUNCED_DIGINS
};

typedef struct DebouncedDigin {
	uint16_t 	debounce_history;
	uint8_t 	state;
	int8_t 		edge;
} debounced_digin_t;

#define DEBOUNCE_TIMx 		TIM14
#define DEBOUNCE_IRQn	 	TIM14_IRQn
#define DEBOUNCE_TIM_RCC 	RCC_APB1Periph_TIM14
#define DEBOUNCE_IRQ		TIM14_IRQHandler
void init_debouncer(void);
