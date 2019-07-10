#include "globals.h"

extern analog_t analog[NUM_ADCS];


void output_envelope(uint32_t dacval)
{
	int32_t env;
	
	uint16_t env5V = dacval >> 2;

	// if (analog[POT_SCALE].lpf_val > 2048)
	// 	env = (env5V * (analog[POT_SCALE].lpf_val - 2048)) >> 11;
	// else 
	// 	env = ((1024-env5V) * (2048 - analog[POT_SCALE].lpf_val)) >> 11;

	// env += (analog[POT_OFFSET].lpf_val-2048);

// DEBUGON;
	int16_t scale = analog[POT_SCALE].lpf_val - 2048; //-2048..+2047
	int16_t offset = (analog[POT_OFFSET].lpf_val>>3);

	env = dacval - 2048;
	env *= scale;

	// env >>= 11; 	//for +/-10V
	// env >>= 12; 	//for +/-5V 
	// env += (analog[POT_OFFSET].lpf_val>>1) - 1023;
	// env = 2048 - env; //0..4096
	// env >>= 2; 		//0..1024

	env >>= 14;	
	env += offset;
	env = 768 - env;

	if (env>1024) env=1025;
	else if (env<0) env=0;
// DEBUGOFF;

	update_pwm(env, PWM_ENV);
	update_pwm(env5V, PWM_5VENV);

//	set_mono_led(PWM_ENVLED, env5V);
	set_mono_led(PWM_ENVLED, 1025-env);
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