/*
 * leds.h
 */

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

struct RgbColor{
	uint16_t r;
	uint16_t g;
	uint16_t b;
};

enum Palette {
	c_OFF,
	c_WHITE,
	c_ORANGE,
	
	NUM_COLORS
};



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
void set_rgb_led(enum RgbLedList rgb_led_id, enum Palette color_id);
void set_mono_led(uint8_t led_id, uint16_t brightness);
