#include "globals.h"

extern const uint16_t loga[4096];


void test_leds(void)
{
	update_pwm(1025, PWM_PINGBUT_RG);
	update_pwm(1025, PWM_PINGBUT_B);
	update_pwm(1025, PWM_CYCLEBUT_R);
	update_pwm(1025, PWM_CYCLEBUT_BG);
	update_pwm(1025, PWM_ENV);
	update_pwm(1025, PWM_ENVLED_B);
	update_pwm(1025, PWM_ENVLED_R);
	update_pwm(1025, PWM_5VENV);
	update_pwm(1025, PWM_5VENVLED_B);
	update_pwm(1025, PWM_5VENVLED_R);

	for (uint8_t j = 0; j < NUM_PWMS; j++)
	{
		update_pwm(0, j);
		update_pwm(1, j);
		update_pwm(1024, j);
		update_pwm(1025, j);
	}
}

void test_rb_color(uint16_t r, uint16_t b)
{
	update_pwm(r, PWM_PINGBUT_RG);
	update_pwm(b, PWM_PINGBUT_B);

	update_pwm(r, PWM_CYCLEBUT_R);
	update_pwm(b, PWM_CYCLEBUT_BG);

}

void set_led_brightness(uint16_t brightness, enum PwmOutputs pwm_led_num)
{
	brightness<<=2;
	if (brightness>4095) brightness=4095;
	brightness = loga[4095-brightness] >> 2;

	update_pwm(brightness, pwm_led_num);
}

void set_rgb_led(enum RgbLeds rgb_led_id, enum Palette color_id)
{
	if (rgb_led_id==LED_PING) 
	{
		switch (color_id) {
			case c_WHITE:
				update_pwm(0, PWM_PINGBUT_RG);
				update_pwm(100, PWM_PINGBUT_B);
				break;
			case c_YELLOW:
				update_pwm(0, PWM_PINGBUT_RG);
				update_pwm(1024, PWM_PINGBUT_B);
				break;
			case c_BLUE:
				update_pwm(1024, PWM_PINGBUT_RG);
				update_pwm(0, PWM_PINGBUT_B);
				break;
			case c_DIMBLUE:
				update_pwm(1024, PWM_PINGBUT_RG);
				update_pwm(850, PWM_PINGBUT_B);
				break;
			case c_OFF:
			default:
				update_pwm(1025, PWM_PINGBUT_RG);
				update_pwm(1025, PWM_PINGBUT_B);
				break;
		}
	} 
	else if (rgb_led_id==LED_CYCLE)
	{
		switch (color_id) {
			case c_RED:
				update_pwm(0, PWM_CYCLEBUT_R);
				update_pwm(1025, PWM_CYCLEBUT_BG);
				break;
			case c_ORANGE:
				update_pwm(30, PWM_CYCLEBUT_R);
				update_pwm(830, PWM_CYCLEBUT_BG);
				break;
			case c_YELLOW:
				update_pwm(300, PWM_CYCLEBUT_R);
				update_pwm(530, PWM_CYCLEBUT_BG);
				break;
			case c_GREEN:
				update_pwm(1025, PWM_CYCLEBUT_R);
				update_pwm(0, PWM_CYCLEBUT_BG);
				break;
			case c_WHITE:
				update_pwm(0, PWM_CYCLEBUT_R);
				update_pwm(0, PWM_CYCLEBUT_BG);
				break;
			case c_OFF:
			default:
				update_pwm(1025, PWM_CYCLEBUT_R);
				update_pwm(1025, PWM_CYCLEBUT_BG);
				break;
		}
	}
	else if (rgb_led_id==LED_ENV)
	{
		switch (color_id) {
			case c_PURPLE:
				update_pwm(0, PWM_ENVLED_B);
				update_pwm(0, PWM_ENVLED_R);
				break;
			case c_RED:
				update_pwm(1024, PWM_ENVLED_B);
				update_pwm(0, PWM_ENVLED_R);
				break;
			case c_BLUE:
				update_pwm(0, PWM_ENVLED_B);
				update_pwm(1024, PWM_ENVLED_R);
				break;
			case c_OFF:
			default:
				update_pwm(1025, PWM_ENVLED_B);
				update_pwm(1025, PWM_ENVLED_R);
				break;
		}
	}
	else if (rgb_led_id==LED_5VENV)
	{
		switch (color_id) {
			case c_PURPLE:
				update_pwm(0, PWM_5VENVLED_B);
				update_pwm(0, PWM_5VENVLED_R);
				break;
			case c_RED:
				update_pwm(1024, PWM_5VENVLED_B);
				update_pwm(0, PWM_5VENVLED_R);
				break;
			case c_BLUE:
				update_pwm(0, PWM_5VENVLED_B);
				update_pwm(1024, PWM_5VENVLED_R);
				break;
			case c_OFF:
			default:
				update_pwm(1025, PWM_5VENVLED_B);
				update_pwm(1025, PWM_5VENVLED_R);
				break;
		}
	}

}

