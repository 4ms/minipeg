
/*
todo: 
Lock +5V (or can adjust it?)
*/

#include <stm32g4xx.h>
#include "globals.h"
#include "hardware_tests.h"
#include "timekeeper.h"
#include "pingable_env.h"
#include "params.h"
#include "dac.h"
#include "env_transition.h"
#include "math_util.h"
#include "timers.h"

extern debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
extern analog_t analog[NUM_ADCS];
extern struct SystemSettings settings;
extern volatile uint32_t systmr;
extern volatile uint32_t tapouttmr;
extern volatile uint32_t tapintmr;
extern volatile uint32_t pingtmr;
extern volatile uint32_t trigouttmr;

volatile uint32_t ping_irq_timestamp=0;
volatile uint32_t trig_irq_timestamp=0;


uint32_t last_tapin_time=0;
uint32_t tapout_clk_time=0;

uint32_t clk_time=0;

uint8_t cycle_but_on = 0;

uint8_t trigq_down=0;
uint8_t triga_down=0;
volatile uint8_t using_tap_clock=0;
uint8_t do_toggle_cycle=0;

uint8_t force_params_update = 0;

char div_ping_led=0;
char got_tap_clock=0;

uint32_t entering_system_mode=0;
uint8_t system_mode_cur=0;
uint8_t initial_cycle_button_state=0;
char update_cycle_button_now=0;

uint8_t adjusting_shift_mode=0;
int16_t cycle_latched_offset;

struct PingableEnvelope m, a;

//main.h
void read_taptempo(void);
void read_trigjacks(void);
void read_cycle_button(void);
void check_reset_envelopes(void);
void update_tap_clock(void);
void read_ping_clock(void);
void update_adc_params(uint8_t force_params_update);

void SysTick_Handler(void)
{
	HAL_IncTick();
}

static const uint32_t kDacSampleRate = 22000;

int main(void)
{
	system_init();
	SysTick_Config(SystemCoreClock/(TICKS_PER_MS*1000));

	init_timekeeper();
	init_pingable_env(&m);
	init_pingable_env(&a);
	init_dig_inouts();
	eor_off();
	eof_off();
	hr_off();
	tapclkout_off();

	init_analog_conditioning();

	init_pwm();
	all_lights_off();

	init_debouncer();

	//Todo: figure out when to enter hardware test mode... check settings for passed_hw_test==1 ?

	if (!read_settings()) 
	{
		test_hardware();
		write_settings();
	}

	HAL_Delay(50);
	adjust_palette();
	check_calibration();
	adjust_palette();

	init_dac(kDacSampleRate);
	assign_dac_update_callback(&update_envelopes);

	init_params();

	//Todo: store each envelope running separately
	if (settings.start_cycle_on)
	{
		cycle_but_on = 1;
		set_rgb_led(LED_CYCLE, c_ORANGE);
		m.envelope_running = 1;
		a.envelope_running = 1;
		using_tap_clock = 1;
	}
	else
	{
		cycle_but_on = 0;
		set_rgb_led(LED_CYCLE, c_OFF);
		m.envelope_running = 0;
		a.envelope_running = 0;
	}

	if (settings.start_clk_time)
	{
		clk_time = settings.start_clk_time;
		m.div_clk_time = settings.start_clk_time;
		a.div_clk_time = settings.start_clk_time; //Todo: settings.start_clk_time_aux;
	}

	last_tapin_time = 0;

	while (1)
	{
		//G0: loops every ~11uS, maybe 13us if you include envelope updates every 4th loop
		//G4: loops every ~2uS, with ~10us gaps  

		//DEBUGON;
		read_taptempo();
		read_trigjacks();
		read_cycle_button();
		check_reset_envelopes();

		//DEBUGOFF;
		update_tap_clock();
		read_ping_clock();
		update_adc_params(force_params_update);
		force_params_update = 0;

		handle_trigout_trigfall();

		handle_system_mode();
	}
}


void read_taptempo(void)
{
	uint32_t now;

	if (digin[PING_BUTTON].state == 1)
	{
		now=tapintmr;

		if (digin[PING_BUTTON].edge == 1)
		{
			digin[PING_BUTTON].edge = 0;

			using_tap_clock=1;

			if (last_tapin_time && (diff32(last_tapin_time,now)<(last_tapin_time>>1)) ) {
				clk_time=(now>>1) + (last_tapin_time>>1);
			} else {
				clk_time=now;
				last_tapin_time=now;
			}

			tapout_clk_time=clk_time;

			tapintmr = 0;
			tapouttmr = 0;

			m.div_clk_time=get_clk_div_time(m.clock_divider_amount,clk_time);
			calc_rise_fall_incs(&m);

			force_transition();
			force_params_update = 1;
		}
		else {
			if (now > HOLDTIMECLEAR)
			{ //button has been down for more than 2 seconds
				if (using_tap_clock)
				{
					m.env_state=WAIT;
					m.envelope_running=0;

					m.divpingtmr=0;
					clk_time=0;
					m.div_clk_time=0;

					m.accum=0;
					using_tap_clock=0;
				}
				tapout_clk_time=0;
				last_tapin_time=0;
				tapouttmr=0;

				set_rgb_led(LED_PING, c_OFF);
				div_ping_led=0;//SEG: added
			} else {
				set_rgb_led(LED_PING, c_WHITE);
				div_ping_led=1;//SEG: added
			}
		}
	}

	if (digin[PING_BUTTON].edge == -1)
	{
		set_rgb_led(LED_PING, c_OFF);
		div_ping_led=0;//SEG: added
		digin[PING_BUTTON].edge = 0;
	}

}

void read_trigjacks(void)
{
	if (digin[TRIGGER_JACK].edge==1)
	{
		digin[TRIGGER_JACK].edge = 0;

		if (settings.trigin_function==TRIGIN_IS_ASYNC || settings.trigin_function==TRIGIN_IS_ASYNC_SUSTAIN)
		{
			triga_down=1;
			trigq_down=0;
			m.sync_to_ping_mode=0;

			m.reset_now_flag=1;
			m.ready_to_start_async=0;

			if (m.rise_time>0x1000)  //FixMe: should this be 0x10 ?? //do an immediate fall if rise_time is fast
				m.outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero

			//start each envelope the same time after the divided clock
			m.async_phase_diff=m.divpingtmr;
		}
		else
		{
			triga_down=0;
			trigq_down=1;
			//Todo: these came from the EXTI ISR, check if its ok
			m.sync_to_ping_mode=1;
			m.reset_nextping_flag=1;

			// When Cycle is on, a TRIGQ should always re-phase (re-start) a divided-ping env.
			// When Cycle is off, if and only if QNT_REPHASES_WHEN_CYCLE_OFF is set, then a TRIGQ should rephase
			// If the envelope is not running, a TRIGQ should always start the envelope.
			// ping_div_ctr=m.clock_divider_amount makes sure that a divided-ping envelope will start from its beginning on the next ping
			if (QNT_REPHASES_WHEN_CYCLE_OFF || cycle_but_on || !m.envelope_running)
			{
				m.ping_div_ctr=m.clock_divider_amount;
				if (m.rise_time>0x10)  //was 0x1000 do an immediate fall if rise_time is fast
					m.outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero
			}

			m.tracking_changedrisefalls=0;

			m.curve_rise=m.next_curve_rise;
			m.curve_fall=m.next_curve_fall;
		}
	}

	if (digin[TRIGGER_JACK].edge==-1)
	{
		digin[TRIGGER_JACK].edge=0;
		triga_down=0;
		trigq_down=0;
	}

	if (digin[CYCLE_JACK].edge==1)
	{
		digin[CYCLE_JACK].edge=0;
		do_toggle_cycle=1;
	}

	if (digin[CYCLE_JACK].edge==-1)
	{
		digin[CYCLE_JACK].edge=0;

		if (settings.cycle_jack_behavior==CYCLE_JACK_BOTH_EDGES_TOGGLE)
			do_toggle_cycle=1;
	}

}

void read_cycle_button(void)
{
	uint64_t time_tmp=0;
	uint32_t elapsed_time;

	if (digin[CYCLE_BUTTON].edge == 1) {
		cycle_latched_offset = analog[POT_OFFSET].lpf_val;
		digin[CYCLE_BUTTON].edge = 0;
	}

	// Releasing cycle button in adjusting shift mode does not toggle cycle
	if ((digin[CYCLE_BUTTON].edge == -1) && adjusting_shift_mode) {
		adjusting_shift_mode = 0;
		digin[CYCLE_BUTTON].edge = 0;
	}

	if (digin[CYCLE_BUTTON].edge == -1 || do_toggle_cycle)
	{
		digin[CYCLE_BUTTON].edge = 0;
		do_toggle_cycle = 0;

		if (cycle_but_on == 0)
		{
			cycle_but_on = 1;
			set_rgb_led(LED_CYCLE, c_ORANGE);

			if (clk_time>0)
			{
				// Cycle button/jack starts the envelope mid-way in its curve (must be calculated)

				if (using_tap_clock)
					elapsed_time=tapouttmr;
				else
					elapsed_time=pingtmr;

				m.div_clk_time=get_clk_div_time(m.clock_divider_amount, clk_time);
				calc_rise_fall_incs(&m);

				//Start the envelope at whatever point it would be in if it were already running

				if (!m.envelope_running && m.sync_to_ping_mode) {
					m.envelope_running=1;

					if (m.clock_divider_amount <= 1) {		//if we're multiplying, calculate the elapsed time
						while (elapsed_time > m.div_clk_time)		//since the last multiplied clock
							elapsed_time -= m.div_clk_time;
					} else {									//otherwise, we're dividing the clock
						while (elapsed_time <= m.div_clk_time) 	//so we want to get as close to the end of the divided cycle
							elapsed_time += clk_time;
						elapsed_time -= clk_time;
					}


					if (elapsed_time <= m.rise_time) {  //Are we on the rise?
						time_tmp = ((uint64_t)elapsed_time) << 12;
						m.accum = time_tmp/m.rise_time; // elapsed_time/rise_time is the % of the rise phase already elapsed, and <<12 gives us the 0..4095 dac value
						m.accum <<= 19;
						//FixMe: Why aren't we setting segphase here?
						m.env_state = RISE;
						m.curve_rise = m.next_curve_rise;
					
						if (m.segphase >= 2048) hr_on();
						else hr_off();
						eor_off();
						eof_on();
					}
					else {
						elapsed_time = elapsed_time-m.rise_time;
						time_tmp = ((uint64_t)elapsed_time) << 12;
						m.accum = 4096 - (time_tmp/m.fall_time);
						m.accum <<= 19;
						//FixMe: Why aren't we setting segphase here?
						m.env_state = FALL;
						m.curve_fall = m.next_curve_fall;

						if (m.segphase < 2048) hr_off();
						else hr_on();
						eor_on();
						eof_off();
					}

					m.ping_div_ctr=m.clock_divider_amount;	

				} else if (!m.envelope_running && !m.sync_to_ping_mode) {
					m.envelope_running=1;
					m.reset_now_flag=1;
					m.ready_to_start_async=0;
					m.async_phase_diff=m.divpingtmr;

				} else if (m.envelope_running && CYCLE_REPHASES_DIV_PING) {
					m.ping_div_ctr=m.clock_divider_amount;
				}

			} //if (clk_time>0)

		} else {
			cycle_but_on = 0;
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}
}

void check_reset_envelopes(void)
{
	uint32_t now;

	now=m.divpingtmr;

	/*
	 Phase-lock the async'ed envelope
	if we're cycling in async mode
	...and have passed the phase offset point 
	...and the envelope hasn't changed shape
	...and are "ready", meaning we have completed an envelope or received the divided ping (this flag ensures we only reset once per divclk period)
	...and we're not holding a sustain
	 */

	if (m.async_phase_diff>m.div_clk_time) m.async_phase_diff=0; //fail-safe measure

	if (!m.sync_to_ping_mode && cycle_but_on && (now >= m.async_phase_diff) && !m.async_env_changed_shape && m.ready_to_start_async && !triga_down) { 
		m.reset_now_flag=1;
		m.ready_to_start_async=0;
		m.async_env_changed_shape=0;
	}

	if (div_ping_led && (now>=(m.div_clk_time>>1))) {
		set_rgb_led(LED_PING, c_OFF);
		div_ping_led=0;
	}

	if (m.div_clk_time) {

		if ((!div_ping_led) && (now>m.div_clk_time)) {

			// now=(now-m.div_clk_time)>>8;
			// m.divpingtmr=now;
			m.divpingtmr = now-m.div_clk_time;

			set_rgb_led(LED_PING, c_WHITE);
			div_ping_led=1;
		
			if (!m.sync_to_ping_mode)
				m.ready_to_start_async=1;

			/* Reset a multiplied envelope when the div_clk occurs
			   if we are in cycle mode or high-gate QNT jack, or have a request to reset/start
			*/
			if (m.sync_to_ping_mode) { 
				if (m.reset_nextping_flag || cycle_but_on || trigq_down ) {		
					if (!m.tracking_changedrisefalls) {
						m.reset_now_flag=1;
						m.reset_nextping_flag=0;
					}
				}  
				m.ping_div_ctr=m.clock_divider_amount; //make sure it restarts the next ping
			}
		}

	} else {
		set_rgb_led(LED_PING, c_OFF);
		div_ping_led=0;//SEG: added
	}
}

void update_tap_clock(void)
{
	uint32_t now;

	if (tapout_clk_time){
		now=tapouttmr;

		if (now>=(tapout_clk_time>>1)){
			tapclkout_off();
		}
		if (now>=tapout_clk_time){
			tapouttmr=0;
			if (using_tap_clock) {
				if (m.clock_divider_amount<=1)
					m.divpingtmr=0;
				got_tap_clock=1;
			}
			tapclkout_on();
		}
	}
}



void read_ping_clock(void)
{
	uint32_t now;
	uint32_t elapsed_time;

	// If the ping interrupt was executed
	// calculate the new clock time, divided clock time,
	// and rise/fall times
	// Also address starting and sync'ing the envelope to pings

	if (got_tap_clock || ping_irq_timestamp) {
		if (ping_irq_timestamp) { // && PINGJACK
			clk_time=ping_irq_timestamp;
		}
		if (!using_tap_clock)
			last_tapin_time=0;

		ping_irq_timestamp=0;

		if (m.clock_divider_amount<=1) {//multiplying, reset div_clk on every ping
			if (!using_tap_clock) m.divpingtmr=0;
			set_rgb_led(LED_PING, c_WHITE);
			div_ping_led=1;
		}

		got_tap_clock=0;
		
		//see if it changed more than 12.5%, if so force a slew limited transition... otherwise just jump-cut

		elapsed_time = m.div_clk_time;
		m.div_clk_time = get_clk_div_time(m.clock_divider_amount,clk_time);
		if (elapsed_time)
		{
			if (m.div_clk_time > elapsed_time)
				now = m.div_clk_time - elapsed_time;
			else
				now = elapsed_time - m.div_clk_time;

			//if (now>(elapsed_time>>6)) //more than 1.5% change
			if (now > (elapsed_time>>3)) //smooth out more than 12.5% change (allow to chop a <12.5%)
			{
				force_transition();
				force_params_update = 1;
			}
		}

		calc_rise_fall_incs(&m);

		if (cycle_but_on || trigq_down || m.reset_nextping_flag || m.envelope_running) {

			if (m.clock_divider_amount > 1) //we're dividing the clock, so resync on every N pings
			{
				if (m.envelope_running && !m.tracking_changedrisefalls)
					m.ping_div_ctr++;

				if (m.ping_div_ctr>=m.clock_divider_amount)
				{
					if (m.sync_to_ping_mode  && !m.tracking_changedrisefalls && (cycle_but_on || m.reset_nextping_flag || trigq_down))
						m.reset_now_flag = 1;

					m.reset_nextping_flag = 0;
					m.ping_div_ctr = 0;
					m.divpingtmr = 0;

					set_rgb_led(LED_PING, c_WHITE);
					div_ping_led=1;
				}

			} else {
				//re-sync on every ping, since we're mult or = to the clock
				if (m.sync_to_ping_mode  && !m.tracking_changedrisefalls && (m.reset_nextping_flag || cycle_but_on || trigq_down))
					m.reset_now_flag = 1;

				m.reset_nextping_flag = 0; //FYI: this goes low only right after an envelope starts (on the ping, of course)
				m.divpingtmr = 0;
			} 
		}
	}
	else 
	{
		/*	If we haven't received a ping within 2x expected clock time (that is, clock stopped or slowed to less than half speed)
			we should stop the ping clock. Or switch to the Tap clock if it's running and we have Tap Clock Output on EOF
		*/	
		if (clk_time && !settings.free_running_ping && !using_tap_clock) {
			now = pingtmr;
			if (now >= (clk_time<<1)) {

				pingtmr=0;

				if (tapout_clk_time && (settings.trigout_function==TRIGOUT_IS_TAPCLKOUT))
				{
					using_tap_clock = 1;
					clk_time = tapout_clk_time;

					m.div_clk_time = get_clk_div_time(m.clock_divider_amount,clk_time);
					calc_rise_fall_incs(&m);

					m.reset_now_flag = 1;
				}
				else
				{
					m.rise_time = 0;
					m.fall_time = 0;
					m.rise_inc = 0;
					m.fall_inc = 0;
					clk_time = 0;
					m.div_clk_time = 0;
					m.envelope_running = 0;
					m.env_state = WAIT;
				}
				set_rgb_led(LED_PING, c_OFF);
				div_ping_led=0;//SEG: added
			}
		}
	}
}

