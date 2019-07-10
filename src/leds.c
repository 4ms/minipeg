#include "globals.h"

extern const uint16_t loga[4096];


rgbLed rgbleds[NUM_RGB_LEDS];

struct RgbColor colors[NUM_COLORS];

void test_leds(void)
{
	update_pwm(0, PWM_PINGBUT_R);
	update_pwm(0, PWM_PINGBUT_G);
	update_pwm(0, PWM_PINGBUT_B);
	update_pwm(0, PWM_CYCLEBUT_R);
	update_pwm(0, PWM_CYCLEBUT_G);
	update_pwm(0, PWM_CYCLEBUT_B);
	update_pwm(0, PWM_ENV);
	update_pwm(0, PWM_ENVLED);
	update_pwm(0, PWM_5VENV);
	update_pwm(0, PWM_TRIGOUTLED);

	for (uint8_t j = 0; j < NUM_PWMS; j++)
	{
		update_pwm(0, j);
		update_pwm(1, j);
		update_pwm(1024, j);
		update_pwm(1025, j);
	}
}

void test_rgb_color(uint16_t r, uint16_t g, uint16_t b)
{
	while (1) 
	{
		update_pwm(r, PWM_PINGBUT_R);
		update_pwm(g, PWM_PINGBUT_G);
		update_pwm(b, PWM_PINGBUT_B);

		update_pwm(r, PWM_CYCLEBUT_R);
		update_pwm(g, PWM_CYCLEBUT_G);
		update_pwm(b, PWM_CYCLEBUT_B);
	}

}

void set_rgb_led(enum RgbLedList rgb_led_id, enum Palette color_id)
{
	update_pwm(colors[color_id].r, rgbleds[rgb_led_id].r_pwm_id);
	update_pwm(colors[color_id].g, rgbleds[rgb_led_id].g_pwm_id);
	update_pwm(colors[color_id].b, rgbleds[rgb_led_id].b_pwm_id);
}

void set_mono_led(uint8_t led_id, uint16_t brightness)
{
	brightness = (brightness*4);
	if (brightness > 4095) brightness = 4095;
	brightness = 4095 - loga[4095-brightness];
	update_pwm(brightness, led_id);
}

void init_rgb_leds(void)
{
	rgbleds[LED_PING].r_pwm_id = PWM_PINGBUT_R;
	rgbleds[LED_PING].g_pwm_id = PWM_PINGBUT_G;
	rgbleds[LED_PING].b_pwm_id = PWM_PINGBUT_B;

	rgbleds[LED_CYCLE].r_pwm_id = PWM_CYCLEBUT_R;
	rgbleds[LED_CYCLE].g_pwm_id = PWM_CYCLEBUT_G;
	rgbleds[LED_CYCLE].b_pwm_id = PWM_CYCLEBUT_B;
}

void init_palette(void)
{
	colors[c_OFF].r = 1025;
	colors[c_OFF].g = 1025;
	colors[c_OFF].b = 1025;

	colors[c_WHITE].r = 0;
	colors[c_WHITE].g = 0;
	colors[c_WHITE].b = 0;

	colors[c_ORANGE].r = 30;
	colors[c_ORANGE].g = 830;
	colors[c_ORANGE].b = 1025;

	colors[c_RED].r = 0;
	colors[c_RED].g = 1025;
	colors[c_RED].b = 1025;

	colors[c_GREEN].r = 1025;
	colors[c_GREEN].g = 0;
	colors[c_GREEN].b = 1025;

	colors[c_BLUE].r = 1024;
	colors[c_BLUE].g = 1025;
	colors[c_BLUE].b = 0;
}
