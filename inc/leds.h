#pragma once
#include <stm32f0xx.h>

enum RgbLedList {
	LED_PING,
	LED_CYCLE,

	NUM_RGB_LEDS
};

typedef struct rgbLed {
	uint8_t r_pwm_id;
	uint8_t g_pwm_id;
	uint8_t b_pwm_id;
} rgbLed;

	// PWM_ENVLED,
	// PWM_EOFLED,

	// PWM_ENV,
	// PWM_ENVLED,
	// PWM_5VENV,
	// PWM_EOFLED,
	// PWM_CYCLEBUT_R,
	// PWM_CYCLEBUT_G,
	// PWM_CYCLEBUT_B,
	// PWM_PINGBUT_R,
	// PWM_PINGBUT_G,
	// PWM_PINGBUT_B,

void init_rgb_leds(void);
void init_palette(void);