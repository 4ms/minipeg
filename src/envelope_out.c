#include "globals.h"

extern analog_t analog[NUM_ADCS];

void output_envelope(uint32_t dacval)
{
	int32_t env;
	uint16_t env5V = dacval >> 2;

	int16_t scale = analog[POT_SCALE].lpf_val - 2048;
	int16_t offset = 4095 - analog[POT_OFFSET].lpf_val; //4095 to 0

	env = (int16_t)dacval;
	env -= offset; //-2048 to +2048
	env *= scale;
	env >>= 14;		
	env = 512 - env ;

	if (env>1024) env=1025;
	else if (env<0) env=0;

	update_pwm(env, PWM_ENV);
	update_pwm(env5V, PWM_5VENV);

	if (env<512) {
		set_led_brightness((512-env)*2, PWM_ENVLED_B);
		set_led_brightness(0, PWM_ENVLED_R);
	} else {
		set_led_brightness(0, PWM_ENVLED_B);
		set_led_brightness((env-512)*2, PWM_ENVLED_R);
	}

	//Todo: use red if 5V is locked
	set_led_brightness(env5V, PWM_5VENVLED_B);
}

void output_offset(void)
{
	int16_t offset = 1024 - (analog[POT_OFFSET].lpf_val>>2);

	update_pwm(offset, PWM_ENV);
	update_pwm(0, PWM_5VENV);

	if (offset<512) {
		set_led_brightness((512-offset)*2, PWM_ENVLED_B);
		set_led_brightness(0, PWM_ENVLED_R);
	} else {
		set_led_brightness(0, PWM_ENVLED_B);
		set_led_brightness((offset-512)*2, PWM_ENVLED_R);
	}

	set_led_brightness(0, PWM_5VENVLED_B);
}

void test_envout(void)
{
	uint16_t i;

	for (i=0; i<1025; i++)
	{
		update_pwm(i, PWM_ENV);
		update_pwm(i, PWM_5VENV);
		update_pwm(i, PWM_ENVLED_B);
		update_pwm(i, PWM_5VENVLED_B);

		delay_ticks(1);
	}
	for (i=1025; i>0; i--)
	{
		update_pwm(i, PWM_ENV);
		update_pwm(i, PWM_5VENV);
		update_pwm(i, PWM_ENVLED_R);
		update_pwm(i, PWM_5VENVLED_R);

		delay_ticks(1);
	}

}