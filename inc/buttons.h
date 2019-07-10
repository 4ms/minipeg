/*
 * buttons.h
 */

#pragma once

#include <stm32f0xx.h>

enum ButtonList {
	PING_BUTTON,
	CYCLE_BUTTON,

	NUM_BUTTONS
};

typedef struct ButtonDebounced {
	uint16_t 	debounce_history;
	uint8_t 	state;
	int8_t 		edge;
} button_t;

#define DEBOUNCE_TIMx 		TIM14
#define DEBOUNCE_IRQn	 	TIM14_IRQn
#define DEBOUNCE_TIM_RCC 	RCC_APB1Periph_TIM14
#define BUTTON_DEBOUNCE_IRQ	TIM14_IRQHandler
void init_buttons(void);
