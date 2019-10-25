#include "globals.h"

extern analog_t analog[NUM_ADCS];
extern int16_t scale, offset, shift;

void output_envelope(uint32_t dacval)
{
	//Todo: optimize calcs using instruction set
	int32_t env;
	uint16_t env5V = dacval;

	env = (int32_t)dacval;
	env -= 2048;
	env += offset;
	env *= scale;
	env >>= 12;
	env += shift;

	if (env>4095) env=4095;
	else if (env<0) env=0;

	update_pwm(env, PWM_ENV);
	update_pwm(env5V, PWM_5VENV);

	if (env<2048) {
		set_led_brightness(0, PWM_ENVLED_B);
		set_led_brightness((2048-env)*2, PWM_ENVLED_R);
	} else {
		set_led_brightness((env-2048)*2, PWM_ENVLED_B);
		set_led_brightness(0, PWM_ENVLED_R);
	}

	//Todo: use red if 5V is locked
	set_led_brightness(env5V, PWM_5VENVLED_B);
}

//Todo: figure out if/how to integrate this into the UI
void output_offset(void)
{
	// int16_t offset = 1024 - (analog[POT_OFFSET].lpf_val>>2);

	// update_pwm(offset, PWM_ENV);
	// update_pwm(0, PWM_5VENV);

	// if (offset<512) {
	// 	set_led_brightness((512-offset)*2, PWM_ENVLED_B);
	// 	set_led_brightness(0, PWM_ENVLED_R);
	// } else {
	// 	set_led_brightness(0, PWM_ENVLED_B);
	// 	set_led_brightness((offset-512)*2, PWM_ENVLED_R);
	// }

	// set_led_brightness(0, PWM_5VENVLED_B);
}
