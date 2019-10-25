#include "globals.h"

extern const uint16_t log4096[4096];
extern struct SystemSettings settings;

AdjustedColor palette[NUM_COLORS];

uint16_t adjust_hue(uint16_t base, uint16_t adj)
{
	uint32_t a = (base * adj) >> (kMaxBrightnessBits-1);
	if (a > kMaxBrightness)
		return kMaxBrightness;
	else
		return a;
}

void create_color(AdjustedColor *col, uint16_t red, uint16_t green, uint16_t blue)
{
	col->ping.r = adjust_hue(red, settings.ping_cal_r);
	col->ping.g = green;
	col->ping.b = adjust_hue(blue, settings.ping_cal_b);

	col->cycle.r = adjust_hue(red, settings.cycle_cal_r);
	col->cycle.g = adjust_hue(green, settings.cycle_cal_g);
	col->cycle.b = blue;
}

void adjust_palette(void)
{
	create_color(&palette[c_OFF], 0, 0, 0);
	create_color(&palette[c_GREY50], 2048, 2048, 2048);
	create_color(&palette[c_WHITE], 2048, 2048, 2048);
	create_color(&palette[c_FULLWHITE], 4095, 4095, 4095);
	create_color(&palette[c_RED], 4095, 0, 0);
	create_color(&palette[c_ORANGE], 4095, 600, 0);
	create_color(&palette[c_YELLOW], 3600, 2400, 0);
	create_color(&palette[c_GREEN], 0, 4095, 0);
	create_color(&palette[c_CYAN], 0, 3600, 3600);
	create_color(&palette[c_BLUE], 0, 0, 4095);
	create_color(&palette[c_DIMBLUE], 0, 0, 1024);
	create_color(&palette[c_PURPLE], 3600, 0, 4095);
}


void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id)
{
	if (rgb_led_id==LED_PING) 
	{
		update_pwm(palette[color_id].ping.r, PWM_PINGBUT_R);
		update_pwm(palette[color_id].ping.b, PWM_PINGBUT_B);
		if (palette[color_id].ping.g > 64) LED_PING_BUT_G_ON;
		else LED_PING_BUT_G_OFF;
	} 
	else if (rgb_led_id==LED_CYCLE)
	{
		update_pwm(palette[color_id].cycle.r, PWM_CYCLEBUT_R);
		update_pwm(palette[color_id].cycle.g, PWM_CYCLEBUT_G);
		if (palette[color_id].cycle.b > 64) LED_CYCLE_BUT_B_ON;
		else LED_CYCLE_BUT_B_OFF;
	}
}

void set_dual_led(enum DualLeds led_id, enum Palette color_id)
{
	enum PwmOutputs red, blue;

	if (led_id==LED_ENV)
	{
		red = PWM_ENVLED_R;
		blue = PWM_ENVLED_B;
	} 
	else if (led_id==LED_5VENV)
	{
		red = PWM_5VENVLED_R;
		blue = PWM_5VENVLED_B;
	}
	else
		return;

	switch (color_id) {
		case c_PURPLE:
			update_pwm(4095, blue);
			update_pwm(4095, red);
			break;
		case c_RED:
			update_pwm(0, blue);
			update_pwm(4095, red);
			break;
		case c_BLUE:
			update_pwm(4095, blue);
			update_pwm(0, red);
			break;
		case c_OFF:
		default:
			update_pwm(0, blue);
			update_pwm(0, red);
			break;
	}
}

void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num) {
	update_pwm(brightness, pwm_led_num);
}

void set_inverted_led(uint16_t brightness, enum PwmOutputs pwm_led_num) {
	if (brightness >= kMaxBrightness)
		update_pwm(0, pwm_led_num);
	else if (kMaxBrightness == 4095)
		update_pwm(log4096[4095-brightness], pwm_led_num);
	else {
		if (kMaxBrightnessBits > 12)
			brightness>>=(kMaxBrightnessBits-12);
		else
			brightness<<=(12-kMaxBrightnessBits);

		if (brightness>=4095)
			update_pwm(0, pwm_led_num);
		else 
			update_pwm(log4096[4095-brightness], pwm_led_num);
	}
}

void all_lights_off(void) {
    update_pwm(0, PWM_CYCLEBUT_R);
    update_pwm(0, PWM_CYCLEBUT_G);
    LED_CYCLE_BUT_B_OFF;

    update_pwm(0, PWM_PINGBUT_R);
    update_pwm(0, PWM_PINGBUT_B);
    LED_PING_BUT_G_OFF;

    update_pwm(0, PWM_ENVLED_B);
    update_pwm(0, PWM_ENVLED_R);

    update_pwm(0, PWM_5VENVLED_B);
    update_pwm(0, PWM_5VENVLED_R);

    set_inverted_led(0, PWM_EOF_LED);
}
