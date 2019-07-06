#include "leds.h"
#include "pwm.h"

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

	update_pwm(100, PWM_PINGBUT_R);
	update_pwm(200, PWM_PINGBUT_R);

	update_pwm(300, PWM_PINGBUT_G);
	update_pwm(400, PWM_PINGBUT_G);

	update_pwm(500, PWM_PINGBUT_B);
	update_pwm(600, PWM_PINGBUT_B);

	update_pwm(700, PWM_CYCLEBUT_R);
	update_pwm(800, PWM_CYCLEBUT_R);

	update_pwm(900, PWM_CYCLEBUT_G);	//controls ENVLED
	update_pwm(1000, PWM_CYCLEBUT_G);	

	update_pwm(900, PWM_CYCLEBUT_B);	//nothing?
	update_pwm(1000, PWM_CYCLEBUT_B);

	update_pwm(100, PWM_ENV);			//controls TRIGOUTLED
	update_pwm(200, PWM_ENVLED);		//nothing?
	update_pwm(300, PWM_5VENV);			//nothing
	update_pwm(400, PWM_TRIGOUTLED);	//nothing

	
}


void set_rgb_led(enum RgbLedList rgb_led_id, enum Palette color_id)
{
	update_pwm(colors[color_id].r, rgbleds[rgb_led_id].r_pwm_id);
	update_pwm(colors[color_id].g, rgbleds[rgb_led_id].g_pwm_id);
	update_pwm(colors[color_id].b, rgbleds[rgb_led_id].b_pwm_id);
}

void set_mono_led(uint8_t led_id, uint16_t brightness)
{
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
	colors[c_OFF].r = 0;
	colors[c_OFF].g = 0;
	colors[c_OFF].b = 0;

	colors[c_WHITE].r = 1024;
	colors[c_WHITE].g = 1024;
	colors[c_WHITE].b = 1024;

	colors[c_ORANGE].r = 1024;
	colors[c_ORANGE].g = 100;
	colors[c_ORANGE].b = 100;
}