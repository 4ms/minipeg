/*
 * leds.h
 */

#pragma once

#include <stm32f0xx.h>

enum RgbLeds {
	LED_PING,
	LED_CYCLE,
	LED_ENV,
	LED_5VENV,

	NUM_RGB_LEDS
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
	
	NUM_COLORS
};

void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id);
void test_leds(void);
void test_rb_color(uint16_t r, uint16_t b);
