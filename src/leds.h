/*
 * leds.h
 */

#pragma once

#include <stdint.h>
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

typedef struct Color {
	uint16_t r;
	uint16_t g;
	uint16_t b;
} Color;

typedef struct AdjustedColor {
	Color ping;
	Color cycle;
} AdjustedColor;


enum Palette {
	c_OFF,
	c_GREY50,
	c_WHITE,
	c_FULLWHITE,
	c_RED,
	c_ORANGE,
	c_YELLOW,
	c_GREEN,
	c_CYAN,
	c_BLUE,
	c_DIMBLUE,
	c_PURPLE,
	
	NUM_COLORS
};

// const uint16_t kMaxBrightness = 4095;
#define kMaxBrightness 4095
#define kMaxBrightnessBits 12

void set_inverted_led(uint16_t brightness, enum PwmOutputs pwm_led_num);
void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num);
void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id);
void set_dual_led(enum DualLeds led_id, enum Palette color_id);
void adjust_palette(void);
uint16_t adjust_hue(uint16_t base, uint16_t adj);
void all_lights_off(void);
