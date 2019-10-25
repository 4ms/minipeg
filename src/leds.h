/*
 * leds.h
 */

#pragma once

#include <stm32g0xx.h>
#include "pwm.h"

enum RgbLeds {
	LED_PING,
	LED_CYCLE,
	NUM_RGB_LEDS
};

enum DualLeds {
	LED_ENV,
	LED_5VENV,
	NUM_DUAL_LEDS
};

enum MonoLeds {
	LED_TRIGOUT,

	NUM_MONO_LEDS
};


enum Palette {
	c_OFF,
	c_WHITE,
	c_ORANGE,
	c_GREEN,
	c_RED,
	c_BLUE,
	c_CYAN,
	c_PURPLE,
	c_YELLOW,
	c_DIMBLUE,
	
	NUM_COLORS
};

void set_inverted_led(uint16_t brightness, enum PwmOutputs pwm_led_num);
void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num);
void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id);
void all_lights_off(void);