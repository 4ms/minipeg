#include "globals.h"
#include "dac.h"
#include "pingable_env.h"
#include "env_transition.h"
#include "timers.h"

extern int16_t scale, offset, shift;
extern uint32_t clk_time;
extern uint8_t triga_down;
extern uint8_t trigq_down;
extern uint8_t cycle_but_on;
extern struct PingableEnvelope m, a;
extern struct SystemSettings settings;

static void do_reset_envelope(struct PingableEnvelope *e);
static uint16_t update_envelope(struct PingableEnvelope *e);
static void output_env_val(uint32_t rawA, uint32_t rawB);
static void handle_env_segment_end(struct PingableEnvelope *e, uint8_t end_segment_flag);
static void handle_env_end(struct PingableEnvelope *e, uint8_t end_env_flag);


void update_envelopes(void)
{
	m.divpingtmr++;
	a.divpingtmr++;
	inc_tmrs();

	uint16_t envA = update_envelope(&m);
	uint16_t envB = update_envelope(&a);
	output_env_val(envA, envB);
}


/*********UPDATE THE ENVELOPE************
 *
 * -restart if needed
 * -calculate new position
 * -change curve step (RISE/FALL)
 * -output EOR/EOF/HalfR
 *
 ****************************************/
const uint32_t k_accum_max = (0xFFF << 19);
const uint32_t k_accum_min = (0x001 << 19);

static uint16_t update_envelope(struct PingableEnvelope *e)
{
	uint8_t end_segment_flag = 0;
	uint8_t end_env_flag = 0;
	uint16_t cur_val = 0;

	if ((clk_time==0) || (e->div_clk_time==0))
	{
		e->envelope_running=0;
		e->outta_sync=0;
	}

	if (e->reset_now_flag)
		do_reset_envelope(e);

	if (!e->envelope_running)
	{
		eor_off();
		hr_off();
		eof_on();
		e->outta_sync = 0;
	}
	else
	{
		// if (e->env_state==TRANSITION) DEBUGON;
		// else DEBUGOFF;

		switch (e->env_state)
		{
			case (RISE):
				e->accum += e->rise_inc;
				e->segphase = e->accum>>19;
				if (e->accum > k_accum_max)
				{
					e->accum = k_accum_max;
					e->segphase = 0x0FFF;
					if (triga_down && settings.trigin_function==TRIGIN_IS_ASYNC_SUSTAIN)
						end_segment_flag = SUSTAIN;
					else
						end_segment_flag = FALL;
				}
				e->cur_curve = e->curve_rise;
				if (e->segphase >= 2048) hr_on();
				else hr_off();
				eor_off();
				eof_on();
			break;

			case (SUSTAIN):
				eor_off();
				eof_off();
				hr_on();
				e->segphase = 0x0FFF;
				if (triga_down && settings.trigin_function==TRIGIN_IS_ASYNC_SUSTAIN)
				{
					e->accum = k_accum_max;
					e->async_env_changed_shape = 1;
				} else {
					end_segment_flag = FALL;
				}
			break;

			case (FALL):
				e->accum -= e->fall_inc;
				e->segphase = e->accum>>19;

				if ((e->accum<k_accum_min) || (e->accum>k_accum_max))
				{
					e->accum = 0;
					e->segphase = 0;
					end_env_flag = 1;
				}
				eor_on();
				eof_off();
				if (e->segphase<2048)	hr_off();
				else hr_on();
				e->cur_curve = e->curve_fall;
			break;

			case (TRANSITION):
				e->accum += e->transition_inc;
				if (e->accum < k_accum_min || (e->transition_inc==0))
				{
				//trans_inc==0 would technically be an error, so this gives us an out
					e->accum = 0;
					e->segphase = 0;
					e->transition_ctr = 1;
				}
				else if (e->accum>k_accum_max)
				{
					e->accum = k_accum_max;
					e->segphase = 0x0FFF;
					e->transition_ctr = 1;
				}
				else
					e->segphase = e->accum>>19;

				if (e->transition_inc>0)
				{
					eor_off();
					eof_on();
				}
				else
				{
					eor_on();
					eof_off();
				}

				e->transition_ctr -= 1;
				if (e->transition_ctr <= 0)
				{
					end_segment_flag = e->next_env_state;
					e->accum = e->accum_endpoint;

					//SPEG Fixme: This logic looks wrong, should it be if (outta_sync==2) ? otherwise outta_sync always is set to 0
					if (e->outta_sync) //2 means we got to transistion from reset_now_flag
						e->outta_sync=0;
					else if (e->outta_sync==1)
						e->outta_sync=2;
					else
						e->outta_sync=0;
				}
			break;

			default:
				break;
		}
		cur_val = calc_curve(e->segphase, e->cur_curve);

		handle_env_segment_end(e, end_segment_flag);
		handle_env_end(e, end_env_flag);
	}

	return cur_val;
}

static void handle_env_segment_end(struct PingableEnvelope *e, uint8_t end_segment_flag)
{
	if (end_segment_flag)
	{
		if (end_segment_flag==FALL)
			e->curve_fall = e->next_curve_fall;

		if (end_segment_flag==RISE)
			e->curve_rise = e->next_curve_rise;

		if (end_segment_flag==SUSTAIN)
			e->curve_fall = e->next_curve_fall;

		e->env_state = end_segment_flag;
	}
}

static void handle_env_end(struct PingableEnvelope *e, uint8_t end_env_flag)
{
	if (end_env_flag)
	{
		eof_on();
		eor_off();	//should already be OFF, but make sure
		hr_off();

		e->curve_rise = e->next_curve_rise;
		e->curve_fall = e->next_curve_fall;

		//Loop if needed
		if (cycle_but_on || trigq_down || e->reset_nextping_flag)
		{
			//Todo: SPEG code changed order here, check for race conditions
			if (e->sync_to_ping_mode)
				e->reset_nextping_flag = 0;
			else
			{
				e->ready_to_start_async = 1;
				if (e->async_env_changed_shape) //if we altered the waveshape, then re-calc the landing spot
					e->async_phase_diff = e->divpingtmr;
				e->async_env_changed_shape = 0;
			}
			e->envelope_running = 1;
			e->env_state = RISE;
		}
		else
		{
			e->envelope_running = 0;
			e->env_state = WAIT;
			e->outta_sync = 0;
		}
	}
}


static int32_t calc_dacval(uint32_t raw_env_val)
{
	int32_t env;
	env = (int32_t)raw_env_val;
	env -= 2048;
	env += offset;
	env *= scale;
	env >>= 12;
	env += shift;

	if (env>4095) env = 4095;
	else if (env<0) env = 0;
}

static void output_env_val(uint32_t rawA, uint32_t rawB)
{
	int32_t envA = calc_dacval(rawA);
	int32_t envB = calc_dacval(rawB);

	dac_out(DAC_ENVA, envA);
	dac_out(DAC_ENVB, envB);

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

static void do_reset_envelope(struct PingableEnvelope *e)
{
	uint32_t elapsed_time;

	e->reset_now_flag = 0;

	if (e->segphase<0x0010)
		e->outta_sync = 0; // if we're practically at bottom, then consider us in sync and do an immediate transition

	if ((!e->envelope_running || (e->outta_sync==0) || (e->div_clk_time<0x80))) //was div_clk_time<0x8000
	{
		e->envelope_running = 1;
		e->env_state = RISE;
		e->accum = 0;
		eof_on();
		eor_off();
	}
	else
	{
		if (e->outta_sync==1)
			e->outta_sync = 2;
		elapsed_time = 128; //was 0x8000. offset to account for transition period: 64/128 timer overflows (6/13ms)
		start_transition(e, elapsed_time);
	}

	e->curve_rise = e->next_curve_rise;
	e->curve_fall = e->next_curve_fall;

	e->reset_nextping_flag = 0;
}

