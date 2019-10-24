#include "globals.h"

extern const uint16_t loga[4096];

void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num)
{
	// if (pwm_led_num!=PWM_EOF_LED)
	// 	brightness = 1024 - brightness;

	//Todo shift/sat
	// brightness<<=2;
	// if (brightness>4095) brightness=4095;
	// brightness = loga[brightness] >> 2;

	update_pwm(brightness, pwm_led_num);
}

void set_inverted_led(uint16_t brightness, enum PwmOutputs pwm_led_num)
{
	//Todo shift w/sat
	brightness<<=2;
	if (brightness>4095) brightness=4095;
	brightness = loga[4095-brightness] >> 2;

	update_pwm(brightness, pwm_led_num);
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
				update_pwm(1024, PWM_PINGBUT_R);
				update_pwm(924, PWM_PINGBUT_B);
				LED_PING_BUT_G_ON;
				break;
			case c_YELLOW:
				update_pwm(1024, PWM_PINGBUT_R);
				update_pwm(0, PWM_PINGBUT_B);
				LED_PING_BUT_G_ON;
				break;
			case c_RED:
				update_pwm(1024, PWM_PINGBUT_R);
				update_pwm(0, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
			case c_BLUE:
				update_pwm(0, PWM_PINGBUT_R);
				update_pwm(1024, PWM_PINGBUT_B);
				LED_PING_BUT_G_OFF;
				break;
			case c_DIMBLUE:
				update_pwm(0, PWM_PINGBUT_R);
				update_pwm(174, PWM_PINGBUT_B);
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
				update_pwm(1024, PWM_CYCLEBUT_R);
				update_pwm(0, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_ORANGE:
				update_pwm(1024, PWM_CYCLEBUT_R);
				update_pwm(150, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_YELLOW:
				update_pwm(900, PWM_CYCLEBUT_R);
				update_pwm(600, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_GREEN:
				update_pwm(0, PWM_CYCLEBUT_R);
				update_pwm(1024, PWM_CYCLEBUT_G);
				LED_CYCLE_BUT_B_OFF;
				break;
			case c_WHITE:
				update_pwm(1024, PWM_CYCLEBUT_R);
				update_pwm(1024, PWM_CYCLEBUT_G);
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
	else if (rgb_led_id==LED_ENV)
	{
		switch (color_id) {
			case c_PURPLE:
				update_pwm(1024, PWM_ENVLED_B);
				update_pwm(1024, PWM_ENVLED_R);
				break;
			case c_RED:
				update_pwm(0, PWM_ENVLED_B);
				update_pwm(1024, PWM_ENVLED_R);
				break;
			case c_BLUE:
				update_pwm(1024, PWM_ENVLED_B);
				update_pwm(0, PWM_ENVLED_R);
				break;
			case c_OFF:
			default:
				update_pwm(0, PWM_ENVLED_B);
				update_pwm(0, PWM_ENVLED_R);
				break;
		}
	}
	else if (rgb_led_id==LED_5VENV)
	{
		switch (color_id) {
			case c_PURPLE:
				update_pwm(1024, PWM_5VENVLED_B);
				update_pwm(1024, PWM_5VENVLED_R);
				break;
			case c_RED:
				update_pwm(0, PWM_5VENVLED_B);
				update_pwm(1024, PWM_5VENVLED_R);
				break;
			case c_BLUE:
				update_pwm(1024, PWM_5VENVLED_B);
				update_pwm(0, PWM_5VENVLED_R);
				break;
			case c_OFF:
			default:
				update_pwm(0, PWM_5VENVLED_B);
				update_pwm(0, PWM_5VENVLED_R);
				break;
		}
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
