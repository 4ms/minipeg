#include "globals.h"

extern analog_t analog[NUM_ADCS];


void output_envelope(uint32_t dacval)
{
	int32_t env;
	uint16_t env5V = dacval >> 2;

	//This way, Scale attenuates/inverts around the center of the LFO, 
	//And offset places the center.
	//Offset at 100% places the LFO center at 5V
	//Scale at 100% is 10Vpp wave
	//But Scale at 0%, Offset at 100% is a steady 5V output (not useful)
	// int16_t scale = analog[POT_SCALE].lpf_val - 2048; //-2048..+2047
	// int16_t offset = (analog[POT_OFFSET].lpf_val>>3);
	// env = dacval - 2048;
	// env *= scale;
	// env >>= 14;	
	// env += offset;
	// env = 768 - env;
	// if (env>1024) env=1025;
	// else if (env<0) env=0;

	//This way, Scale attenuates/inverts around the base-line,
	//which is set by Offset.
	//Offset sets the base line from -10V (CCW) to 0V (center) to +10V (CW)
	//Scale makes the LFO go up from the base-line (CW) or down below the base-line (CCW)
	int16_t scale = analog[POT_SCALE].lpf_val - 2048;
	int16_t offset = analog[POT_OFFSET].lpf_val>>2;
	env = dacval * scale;
	env >>= 14;	
	env += offset;
	
	env = 1024 - env;
	if (env>1024) env=1025;
	else if (env<0) env=0;

	update_pwm(env, PWM_ENV);
	update_pwm(env5V, PWM_5VENV);

	//Todo: use red for negative portion
	if (env<284) {
		update_pwm(env*3+(1025-284*3), PWM_ENVLED_B);
		update_pwm(1025, PWM_ENVLED_R);
	} else {
		update_pwm(1025, PWM_ENVLED_B);
		update_pwm((284+1025)-env, PWM_ENVLED_R);
	}
	//Todo: use red if 5V is locked
	update_pwm(env5V, PWM_5VENVLED_B);
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