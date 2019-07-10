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
	c_GREEN,
	c_RED,
	c_BLUE,
	
	NUM_COLORS
};

void init_rgb_leds(void);
void init_palette(void);
void set_rgb_led(enum RgbLedList rgb_led_id, enum Palette color_id);
void set_mono_led(uint8_t led_id, uint16_t brightness);
void test_leds(void);
