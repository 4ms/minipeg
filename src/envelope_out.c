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

	set_mono_led(PWM_ENVLED, env5V);
	// set_mono_led(PWM_ENVLED, 1025-env);
}

void test_envout(void)
{
	uint16_t i;

	for (i=0; i<1025; i++)
	{
		update_pwm(i, PWM_ENV);
		update_pwm(i, PWM_5VENV);
		update_pwm(i, PWM_ENVLED);

		delay_ticks(1);
	}

}