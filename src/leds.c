#include "globals.h"

extern const uint16_t loga[4096];

void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num)
{
	update_pwm(brightness, pwm_led_num);
}

void set_inverted_led(uint16_t brightness, enum PwmOutputs pwm_led_num)
{
	//Todo shift w/sat
	brightness<<=2;
	if (brightness>4095) brightness=4095;
	brightness = loga[4095-brightness];

	update_pwm(brightness, pwm_led_num);
}

typedef struct Color {
	uint16_t r;
	uint16_t g;
	uint16_t b;
} Color;

typedef struct AdjustedColor {
	Color ping;
	Color cycle;
} AdjustedColor;


// const Color c_off = {0, 0, 0};
// const Color c_grey50 = {2048, 2048, 2048};
// const Color c_white = {4095, 3700, 4095};
// const Color c_red = {4095, 0, 0};
// const Color c_orange = {4095, 600, 0};
// const Color c_yellow = {3600, 2400, 0};
// const Color c_green = {0, 4095, 0};
// const Color c_cyan = {0, 3600, 3600};
// const Color c_blue = {0, 0, 4095};
// const Color c_dimblue = {0, 0, 1024};
// const Color c_purple = {3600, 0, 4095};

AdjustedColor palette[NUM_COLORS];

AdjustedColor create_color(uint16_t red, uint16_t blue, uint16_t green) {
	Color color;
	uint32_t adj;

	if (rgb_led_id==LED_PING) {
		adj = (red * settings.ping_cal_r) >> 15;
		if (adj > 0xFFFF) color.r = 0xFFFF;
		else color.r = adj;

		adj = (blue * settings.ping_cal_b) >> 15;
		if (adj > 0xFFFF) color.b = 0xFFFF;
		else color.b = adj;

		color.g = (green >= 2048) ? 4095 : 0;
	}

	if (rgb_led_id==LED_CYCLE) {
		adj = (red * settings.cycle_cal_r) >> 15;
		if (adj > 0xFFFF) color.r = 0xFFFF;
		else color.r = adj;

		color.b = (blue >= 2048) ? 4095 : 0;

		adj = (green * settings.cycle_cal_g) >> 15;
		if (adj > 0xFFFF) color.g = 0xFFFF;
		else color.g = adj;
	}
}


void create_adjusted_palette(void) {
	palette[c_OFF] = create_color(0, 0, 0,);
}


//Todo: make color arrays, initialized to some default values
//Then adjust them on boot (After checking for calibration)
//
void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id)
{
	if (rgb_led_id==LED_PING) 
	{
		switch (color_id) {
			case c_WHITE:
				update_pwm(4095, PWM_PINGBUT_R);
				update_pwm(3700, PWM_PINGBUT_B);
				LED_PING_BUT_G_ON;
				break;
			case c_YELLOW:
				update_pwm(4095, PWM_PINGBUT_R);
				update_pwm(0, PWM_PINGBUT_B);
				LED_PING_BUT_G_ON;
				break;
			case c_RED:
				update_pwm(4095, PWM_PINGBUT_R);
				update_pwm(0, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
			case c_BLUE:
				update_pwm(0, PWM_PINGBUT_R);
				update_pwm(4095, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
			case c_DIMBLUE:
				update_pwm(0, PWM_PINGBUT_R);
				update_pwm(700, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
			case c_OFF:
			default:
				update_pwm(0, PWM_PINGBUT_R);
				update_pwm(0, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
		}
	} 
	else if (rgb_led_id==LED_CYCLE)
	{
		switch (color_id) {
			case c_RED:
				update_pwm(4095, PWM_CYCLEBUT_R);
				update_pwm(0, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_ORANGE:
				update_pwm(4095, PWM_CYCLEBUT_R);
				update_pwm(600, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_YELLOW:
				update_pwm(3600, PWM_CYCLEBUT_R);
				update_pwm(2400, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_GREEN:
				update_pwm(0, PWM_CYCLEBUT_R);
				update_pwm(4095, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_WHITE:
				update_pwm(4095, PWM_CYCLEBUT_R);
				update_pwm(4095, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_ON;
				break;
			case c_OFF:
			default:
				update_pwm(0, PWM_CYCLEBUT_R);
				update_pwm(0, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
		}
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
