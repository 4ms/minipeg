#include "globals.h"

extern analog_t analog[NUM_ADCS];
extern int16_t scale, offset, shift;

void output_envelope(uint32_t dacval)
{
	//Todo: optimize calcs using instruction set
	int32_t envA;
	uint16_t envB = dacval;

	envA = (int32_t)dacval;
	envA -= 2048;
	envA += offset;
	envA *= scale;
	envA >>= 12;
	envA += shift;

	if (envA>4095) envA=4095;
	else if (envA<0) envA=0;

	//dac_out(envA, ENVA);
	//dac_out(envB, ENVB);

	if (envA<2048) {
		set_led_brightness(0, PWM_ENVA_B);
		set_led_brightness((2048-envA)*2, PWM_ENVA_R);
	} else {
		set_led_brightness((envA-2048)*2, PWM_ENVA_B);
		set_led_brightness(0, PWM_ENVA_R);
	}

	//Todo: use red if 5V is locked
	set_led_brightness(envB, PWM_ENVB_B);
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
