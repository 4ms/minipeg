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
	col->ping.g = adjust_hue(green, settings.ping_cal_g);
	col->ping.b = adjust_hue(blue, settings.ping_cal_b);

	col->cycle.r = adjust_hue(red, settings.cycle_cal_r);
	col->cycle.g = adjust_hue(green, settings.cycle_cal_g);
	col->cycle.b = adjust_hue(blue, settings.cycle_cal_b);

	col->lock.r = adjust_hue(red, settings.lock_cal_r);
	col->lock.g = adjust_hue(green, settings.lock_cal_g);
	col->lock.b = adjust_hue(blue, settings.lock_cal_b);

	col->envA.r = adjust_hue(red, settings.envA_cal_r);
	col->envA.g = adjust_hue(green, settings.envA_cal_g);
	col->envA.b = adjust_hue(blue, settings.envA_cal_b);

	col->envB.r = adjust_hue(red, settings.envB_cal_r);
	col->envB.g = adjust_hue(green, settings.envB_cal_g);
	col->envB.b = adjust_hue(blue, settings.envB_cal_b);
}

void adjust_palette(void)
{
	create_color(&palette[c_OFF], 0, 0, 0);
	create_color(&palette[c_GREY50], 1024, 1024, 1024);
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
		update_pwm(palette[color_id].ping.g, PWM_PINGBUT_G);
	} 
	else if (rgb_led_id==LED_CYCLE)
	{
		update_pwm(palette[color_id].cycle.r, PWM_CYCLEBUT_R);
		update_pwm(palette[color_id].cycle.b, PWM_CYCLEBUT_B);
		update_pwm(palette[color_id].cycle.g, PWM_CYCLEBUT_G);
	}
	else if (led_id==LED_LOCK)
	{
		update_pwm(palette[color_id].lock.r, PWM_LOCKBUT_R);
		update_pwm(palette[color_id].lock.b, PWM_LOCKBUT_B);
		update_pwm(palette[color_id].lock.g, PWM_LOCKBUT_G);
	} 
	else if (led_id==LED_ENVA)
	{
		update_pwm(palette[color_id].envA.r, PWM_ENVA_R);
		update_pwm(palette[color_id].envA.b, PWM_ENVA_B);
		update_pwm(palette[color_id].envA.g, PWM_ENVA_G);
	} 
	else if (led_id==LED_ENVB)
	{
		update_pwm(palette[color_id].envB.r, PWM_ENVB_R);
		update_pwm(palette[color_id].envB.b, PWM_ENVB_B);
		update_pwm(palette[color_id].envB.g, PWM_ENVB_G);
	}
	else
		return;
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
    update_pwm(0, PWM_CYCLEBUT_B);

    update_pwm(0, PWM_PINGBUT_R);
    update_pwm(0, PWM_PINGBUT_G);
    update_pwm(0, PWM_PINGBUT_B);

    update_pwm(0, PWM_LOCKBUT_R);
    update_pwm(0, PWM_LOCKBUT_G);
    update_pwm(0, PWM_LOCKBUT_B);

    update_pwm(0, PWM_ENVA_R);
    update_pwm(0, PWM_ENVA_G);
    update_pwm(0, PWM_ENVA_B);

    update_pwm(0, PWM_ENVB_R);
    update_pwm(0, PWM_ENVB_G);
    update_pwm(0, PWM_ENVB_B);

    set_inverted_led(0, PWM_EOF_LED);
}
