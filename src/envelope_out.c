#include "globals.h"
#include "dac.h"

extern analog_t analog[NUM_ADCS];
extern int16_t scale, offset, shift;

/*********UPDATE THE ENVELOPE************
 *					
 *  Update only when timer has overflowed
 *  Timer overflows about once every 3-4 main loop cycles
 * -restart if needed			
 * -calculate new position		
 * -change curve step (RISE/FALL)	
 * -output EOR/EOF/HalfR 		
 *					
 ****************************************/

uint32_t clk_time;
uint32_t div_clk_time;

void update_envelope(void)
{
	uint8_t end_segment_flag=0;
	uint8_t end_env_flag=0;

	if ((clk_time==0) || (div_clk_time==0))
	{
		envelope_running=0;
		outta_sync=0;
	}

	if (ticks_since_envout)
	{
		use_ticks_since_envout = ticks_since_envout;
		ticks_since_envout = 0;

		if (reset_now_flag)
		{ 
			do_reset_envelopes();
		}

		if (envelope_running)
		{
			//PEGv2: this block takes about 15-18us and runs every 100us (10kHz sampling rate)
			//SEG-G0: runs every 50us (20kHz)
			segphase=0;
			// if (env_state==TRANSITION)
			// 	DEBUGON;
			// else
			// 	DEBUGOFF;

			switch (env_state)
			{
				case (RISE):
					ticks_since_envout_running_total += use_ticks_since_envout;
					accum += rise_inc*use_ticks_since_envout;
					segphase = accum>>19;

					if (accum > 0x7FF80000)
					{
						accum = 0x7FF80000;
						segphase = 0x0FFF;
						if (triga_down && settings.trigin_function==TRIGIN_IS_ASYNC_SUSTAIN)
							end_segment_flag = SUSTAIN;
						else
							end_segment_flag = FALL;
					}
					
					cur_curve=curve_rise;

					if (segphase>=2048) hr_on();
					else hr_off();
					eor_off();
					eof_on();
				break;

				case (SUSTAIN):
					eor_off();
					eof_off();
					hr_on();

					segphase=0x0FFF;

					if (triga_down && settings.trigin_function==TRIGIN_IS_ASYNC_SUSTAIN)
					{
						accum=0x7FF80000;
						async_env_changed_shape=1;
					} else {
						end_segment_flag=FALL;
					}
				break;

				case (FALL):
					ticks_since_envout_running_total += use_ticks_since_envout;
					accum -= fall_inc*use_ticks_since_envout;
					segphase=accum>>19;

					if ((accum<0x00080000) || (accum>0x7FF80000))
					{
						accum = 0;
						segphase = 0;
						end_env_flag = 1;
					}

					eor_on();
					eof_off();
					if (segphase<2048)	hr_off();
					else hr_on();

					cur_curve=curve_fall;
				break;

				case (TRANSITION): 
					ticks_since_envout_running_total += use_ticks_since_envout;
					accum += transition_inc*use_ticks_since_envout;
					if (accum < 0x00080000 || (transition_inc==0)) //trans_inc==0 would technically be an error, so this gives us an out
					{
						accum = 0;
						segphase = 0;
						transition_ctr = use_ticks_since_envout;
					}
					else if (accum>0x7FF80000)
					{
						accum = 0x7FF80000;
						segphase = 0x0FFF;
						transition_ctr = use_ticks_since_envout;
					}
					else
						segphase = accum>>19;

					if (transition_inc>0)
					{
						eor_off();
						eof_on();
					}
					else
					{
						eor_on();
						eof_off();
					}

					transition_ctr -= use_ticks_since_envout;
					if (transition_ctr <= 0)
					{
						end_segment_flag = next_env_state;
						accum = accum_endpoint;

						//SPEG Fixme: This logic looks wrong, should it be if (outta_sync==2) ? otherwise outta_sync always is set to 0
						if (outta_sync) //2 means we got to transistion from reset_now_flag
							outta_sync=0;
						else if (outta_sync==1)
							outta_sync=2;
						else
							outta_sync=0;
					}
				break;

				default:
					break;
			}
			segphase = calc_curve(segphase, cur_curve);
			output_envelope(segphase);

			if (end_segment_flag)
			{
				if (end_segment_flag==FALL)
					curve_fall = next_curve_fall;

				if (end_segment_flag==RISE)
					curve_rise = next_curve_rise;

				if (end_segment_flag==SUSTAIN)
					curve_fall = next_curve_fall;
				
				env_state = end_segment_flag;
				end_segment_flag = 0;
			}

			if (end_env_flag)
			{
				eof_on();
				eor_off();	//should already be OFF, but make sure
				hr_off();

				curve_rise = next_curve_rise;
				curve_fall = next_curve_fall;

				//Loop if needed
				if (cycle_but_on || trigq_down || reset_nextping_flag)
				{
					//Todo: SPEG code changed order here, check for race conditions
					if (sync_to_ping_mode)
						reset_nextping_flag = 0; 
					else
					{
						ready_to_start_async = 1;
						if (async_env_changed_shape) //if we altered the waveshape, then re-calc the landing spot
							async_phase_diff = divpingtmr;
						async_env_changed_shape = 0;
					}
					envelope_running = 1;
					env_state = RISE;
				}
				else
				{
					envelope_running = 0;
					env_state = WAIT;
					outta_sync = 0;
				}
			}

		} 
		else
		{ //envelope_running!=1 
			eor_off();
			hr_off();
			eof_on();
			outta_sync = 0;
			
			output_envelope(0);
			//Todo: offset if holding down CYCLE while turning Offset:
			//if (cycle_offset_combo)
			//	output_offset();
			//else
			//	output_envelope(0);
			ticks_since_envout = 0;
		}
	}
}


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
