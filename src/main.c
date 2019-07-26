
/*
todo: 
Rename envout to segment_phase
*/

#include "stm32f0xx_conf.h"
#include "globals.h"


extern debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
extern analog_t analog[NUM_ADCS];
extern struct SystemSettings settings;


volatile uint32_t tapouttmr;
volatile uint32_t tapintmr;
volatile uint32_t pingtmr;
volatile uint32_t divpingtmr;
volatile uint32_t trigouttmr;
volatile uint8_t ticks_since_envout=0;
volatile uint32_t ping_irq_timestamp=0;
volatile uint32_t trig_irq_timestamp=0;

__IO uint32_t systmr=0;

int8_t clock_divider_amount=1;
uint8_t didnt_change_divmult=0;


uint32_t last_tapin_time=0;
uint16_t ticks_since_envout_running_total=0; //debug watch variable
char use_ticks_since_envout=0;
uint32_t tapout_clk_time=0;

uint32_t clk_time=0;
uint32_t div_clk_time=0;
uint32_t rise_time=0;
uint32_t fall_time=0;
uint32_t rise_inc=0;
uint32_t fall_inc=0;

uint8_t curve_rise=127;
uint8_t curve_fall=127;
uint8_t cur_curve=127;

uint8_t next_curve_rise = 127;
uint8_t next_curve_fall = 127;

uint32_t async_phase_diff=0;
uint8_t async_env_changed_shape=1;


char envelope_running=0;
enum envelopeStates env_state=WAIT;
uint8_t next_env_state=WAIT;

uint32_t accum=0;
uint16_t envout=0;
uint32_t new_accum=0;
uint16_t new_dacout=0;

uint16_t adc_dma_buffer[NUM_ADCS];


uint8_t cycle_but_on = 0;

uint8_t trigq_down=0;
uint8_t triga_down=0;
volatile uint8_t using_tap_clock=0;
uint8_t do_toggle_cycle=0;

uint8_t force_params_update = 0;

volatile uint8_t reset_nextping_flag=0;
volatile uint8_t sync_to_ping_mode=1;
char reset_now_flag=0;
char tracking_changedrisefalls=0;
int32_t transition_inc=0;
int32_t transition_ctr=0;
char outta_sync=0;

int8_t ping_div_ctr=0;
char div_ping_led=0;
char ready_to_start_async=0;
char got_tap_clock=0;

uint32_t entering_system_mode=0;
uint8_t system_mode_cur=0;
uint8_t initial_cycle_button_state=0;
char update_cycle_button_now=0;

uint16_t shape;
uint8_t skew, curve;
uint16_t poll_user_input = 0;
char divmult_changed=0;

//main.h
void read_taptempo(void);
void read_trigjacks(void);
void read_cycle_button(void);
void check_reset_envelopes(void);
void update_tap_clock(void);
void read_ping_clock(void);
void do_reset_envelopes(void);
void update_envelope(void);
void update_adc_params(uint8_t force_params_update);


void SysTick_Handler(void)
{
	systmr++;
	tapouttmr++;
	tapintmr++;
	pingtmr++;
	divpingtmr++;
	trigouttmr++;
	ticks_since_envout++;
}

void init_tmrs(void)
{
	tapouttmr=0;
	tapintmr=0;
	pingtmr=0;
	divpingtmr=0;
	trigouttmr=0;
}

uint32_t udiv32(uint32_t n27)
{
	return (uint32_t)(1<<31) / n27;
}

int main(void)
{

	init_tmrs();
	init_dig_inouts();
	init_adc(adc_dma_buffer, NUM_ADCS);
	init_analog_conditioning();

	init_pwm();
	SysTick_Config(SystemCoreClock/10000); //1000 is 1ms ticks, 10000 is 100us ticks, 100000 is 10us ticks

	init_EXTI_inputs();
	init_debouncer();

	init_rgb_leds();
	init_palette();

	//test_leds();
	//test_rgb_color(adc_buffer[5], adc_buffer[5], adc_buffer[5]);

	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);

	set_mono_led(PWM_TRIGOUTLED, 0);
	set_mono_led(PWM_ENVLED, 0);

	eor_off();
	eof_off();
	hr_off();
	tapclkout_off();

	read_settings();

	if (settings.start_cycle_on)
	{
		cycle_but_on = 1;
		set_rgb_led(LED_CYCLE, c_ORANGE);
		envelope_running = 1;
	}
	else
	{
		cycle_but_on = 0;
		set_rgb_led(LED_CYCLE, c_OFF);
		envelope_running = 0;
	}

	if (settings.start_clk_time)
	{
		clk_time = settings.start_clk_time;
		div_clk_time = settings.start_clk_time;
	}

	last_tapin_time=0;
	sync_to_ping_mode=1;
	accum=0;

	check_calibration();

	while (1)
	{
		// DEBUGON;
		read_taptempo();
		read_trigjacks();
		read_cycle_button();
		check_reset_envelopes();
		// DEBUGOFF;

		update_tap_clock();
		read_ping_clock();
		update_adc_params(force_params_update);
		force_params_update = 0;

		handle_trigout_trigfall();
		update_envelope();

		// handle_system_mode();
	}

}

void calc_rise_fall_incs(void)
{
	fall_time=get_fall_time(skew, div_clk_time);
	rise_time=div_clk_time-fall_time;
	rise_inc=udiv32(rise_time); //udiv32(rise_time>>5); 
	fall_inc=udiv32(fall_time);
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

			div_clk_time=get_clk_div_time(clock_divider_amount,clk_time);
			calc_rise_fall_incs();

			didnt_change_divmult=NUM_ADC_CYCLES_BEFORE_TRANSITION; //force us to do a transition mode as if divmult or skew was changed
			force_params_update = 1;
		}
		else {
			if (now > HOLDTIMECLEAR)
			{ //button has been down for more than 2 seconds
				if (using_tap_clock)
				{
					env_state=WAIT;
					envelope_running=0;

					divpingtmr=0;
					clk_time=0;
					div_clk_time=0;

					accum=0;
					using_tap_clock=0;
					ticks_since_envout=1;
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

		if (settings.trigin_function==TRIGIN_IS_ASYNC)
		{
			triga_down=1;
			trigq_down=0;
			sync_to_ping_mode=0;

			reset_now_flag=1;
			ready_to_start_async=0;
			
			if (rise_time>0x1000)  //do an immediate fall if rise_time is fast
				outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero

			//start each envelope the same time after the divided clock
			async_phase_diff=divpingtmr;	
		}
		else
		{
			triga_down=0;
			trigq_down=1;
			//Todo: these came from the EXTI ISR, check if its ok
			sync_to_ping_mode=1;
			reset_nextping_flag=1;

			// When Cycle is on, a TRIGQ should always re-phase (re-start) a divided-ping env.
			// When Cycle is off, if and only if QNT_REPHASES_WHEN_CYCLE_OFF is set, then a TRIGQ should rephase 
			// If the envelope is not running, a TRIGQ should always start the envelope.
			// ping_div_ctr=clock_divider_amount makes sure that a divided-ping envelope will start from its beginning on the next ping
			if (QNT_REPHASES_WHEN_CYCLE_OFF || cycle_but_on || !envelope_running)
			{
				ping_div_ctr=clock_divider_amount;
				if (rise_time>0x10)  //was 0x1000 do an immediate fall if rise_time is fast
					outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero
			}

			tracking_changedrisefalls=0;

			curve_rise=next_curve_rise;
			curve_fall=next_curve_fall;
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

	// Cycle button/jack starts the envelope mid-way in its curve (must be calculated)

	if (digin[CYCLE_BUTTON].edge==1 || do_toggle_cycle)
	{
		digin[CYCLE_BUTTON].edge=0;
		do_toggle_cycle=0;

		if (cycle_but_on==0)
		{
			cycle_but_on=1;
			set_rgb_led(LED_CYCLE, c_ORANGE);

			if (clk_time>0)
			{
				if (using_tap_clock)
					elapsed_time=tapouttmr;
				else
					elapsed_time=pingtmr;

				div_clk_time=get_clk_div_time(clock_divider_amount, clk_time);
				calc_rise_fall_incs();

				//Start the envelope at whatever point it would be in if it were already running

				if (!envelope_running && sync_to_ping_mode) {
					envelope_running=1;

					if (clock_divider_amount <= 1) {		//if we're multiplying, calculate the elapsed time
						while (elapsed_time > div_clk_time)		//since the last multiplied clock
							elapsed_time -= div_clk_time;		
					} else {									//otherwise, we're dividing the clock
						while (elapsed_time <= div_clk_time) 	//so we want to get as close to the end of the divided cycle
							elapsed_time += clk_time;
						elapsed_time -= clk_time;
					}


					if (elapsed_time <= rise_time) {  //Are we on the rise?
						time_tmp = ((uint64_t)elapsed_time) << 12;
						accum = time_tmp/rise_time; // elapsed_time/rise_time is the % of the rise phase already elapsed, and <<12 gives us the 0..4095 dac value
						accum <<= 19;
						//FixMe: Why aren't we setting envout here?
						env_state = RISE;
						curve_rise = next_curve_rise;
					
						if (envout >= 2048) hr_on();
						else hr_off();
						eor_off();
						eof_on();
					}
					else {
						elapsed_time = elapsed_time-rise_time;
						time_tmp = ((uint64_t)elapsed_time) << 12;
						accum = 4096 - (time_tmp/fall_time);
						accum <<= 19;
						//FixMe: Why aren't we setting envout here?
						env_state = FALL;
						curve_fall = next_curve_fall;

						if (envout < 2048) hr_off();
						else hr_on();
						eor_on();
						eof_off();
					}

					ping_div_ctr=clock_divider_amount;	

				} else if (!envelope_running && !sync_to_ping_mode) {
					envelope_running=1;
					reset_now_flag=1;
					ready_to_start_async=0;
					async_phase_diff=divpingtmr;

				} else if (envelope_running && CYCLE_REPHASES_DIV_PING) {
					ping_div_ctr=clock_divider_amount;
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

	now=divpingtmr;

	/*
	 Phase-lock the async'ed envelope 
	   if we're cycling in async mode
	...and have passed the phase offset point 
	...and the envelope hasn't changed shape
	...and are "ready", meaning we have completed an envelope or received the divided ping (this flag ensures we only reset once per divclk period)
	...and we're not holding a sustain
	 */

	if (async_phase_diff>div_clk_time) async_phase_diff=0; //fail-safe measure

	if (!sync_to_ping_mode && cycle_but_on && (now >= async_phase_diff) && !async_env_changed_shape && ready_to_start_async && !triga_down) { 	
		reset_now_flag=1;
		ready_to_start_async=0;
		async_env_changed_shape=0;
	}

	if (div_ping_led && (now>=(div_clk_time>>1))) {
		set_rgb_led(LED_PING, c_OFF);
		div_ping_led=0;
	}

	if (div_clk_time) {

		if ((!div_ping_led) && (now>div_clk_time)) {

			// now=(now-div_clk_time)>>8;
			// divpingtmr=now;
			divpingtmr = now-div_clk_time;

			set_rgb_led(LED_PING, c_WHITE);
			div_ping_led=1;
		
			if (!sync_to_ping_mode)
				ready_to_start_async=1;

			/* Reset a multiplied envelope when the div_clk occurs
			   if we are in cycle mode or high-gate QNT jack, or have a request to reset/start
			*/
			if (sync_to_ping_mode) { 
				if (reset_nextping_flag || cycle_but_on || trigq_down ) {		
					if (!tracking_changedrisefalls) {
						reset_now_flag=1;
						reset_nextping_flag=0;
					}
				}  
				ping_div_ctr=clock_divider_amount; //make sure it restarts the next ping
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
				if (clock_divider_amount<=1)
					divpingtmr=0;
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
		if (ping_irq_timestamp && PINGJACK) { //added && PING because it was sometimes false triggering on falling edge of PING 
			clk_time=ping_irq_timestamp;
		}
		if (!using_tap_clock)
			last_tapin_time=0;

		ping_irq_timestamp=0;

		if (clock_divider_amount<=1) {//multiplying, reset div_clk on every ping
			if (!using_tap_clock) divpingtmr=0;
			set_rgb_led(LED_PING, c_WHITE);
			div_ping_led=1;
		}

		got_tap_clock=0;
		
		//see if it changed more than 12.5%, if so force a slew limited transition... otherwise just jump-cut

		elapsed_time = div_clk_time;
		div_clk_time = get_clk_div_time(clock_divider_amount,clk_time);
		if (elapsed_time)
		{
			if (div_clk_time > elapsed_time)
				now = div_clk_time - elapsed_time;
			else
				now = elapsed_time - div_clk_time;

			//if (now>(elapsed_time>>6)) //more than 1.5% change
			if (now > (elapsed_time>>3)) //smooth out more than 12.5% change (allow to chop a <12.5%)
			{
				didnt_change_divmult = NUM_ADC_CYCLES_BEFORE_TRANSITION; //force us to do a transition mode as if divmult or skew was changed
				force_params_update = 1;
			}
		}

		calc_rise_fall_incs();

		if (cycle_but_on || trigq_down || reset_nextping_flag || envelope_running) {

			if (clock_divider_amount > 1) //we're dividing the clock, so resync on every N pings
			{
				if (envelope_running && !tracking_changedrisefalls)
					ping_div_ctr++;

				if (ping_div_ctr>=clock_divider_amount)
				{
					if (sync_to_ping_mode  && !tracking_changedrisefalls && (cycle_but_on || reset_nextping_flag || trigq_down))
						reset_now_flag = 1;

					reset_nextping_flag = 0;
					ping_div_ctr = 0;
					divpingtmr = 0;

					set_rgb_led(LED_PING, c_WHITE);
					div_ping_led=1;
				}

			} else {
				//re-sync on every ping, since we're mult or = to the clock
				if (sync_to_ping_mode  && !tracking_changedrisefalls && (reset_nextping_flag || cycle_but_on || trigq_down))
					reset_now_flag = 1;

				reset_nextping_flag = 0; //FYI: this goes low only right after an envelope starts (on the ping, of course)
				divpingtmr = 0;
			} 
		}
	}
	else 
	{
		/*	If we haven't received a ping within 2x expected clock time (that is, clock stopped or slowed to less than half speed)
			we should stop the ping clock. Or switch to the Tap clock if it's running and we have Tap Clock Output on EOF
		*/	
		if (clk_time && settings.no_free_running_ping && !using_tap_clock) {
			now = pingtmr;
			if (now >= (clk_time<<1)) {
			
				pingtmr=0;

				if (tapout_clk_time && (settings.trigout_function==TRIGOUT_IS_TAPCLKOUT))
				{
					using_tap_clock = 1;				
					clk_time = tapout_clk_time;

					div_clk_time = get_clk_div_time(clock_divider_amount,clk_time);
					calc_rise_fall_incs();

					reset_now_flag = 1;
					ticks_since_envout = 1;
				}
				else
				{
					rise_time = 0;
					fall_time = 0;
					rise_inc = 0;
					fall_inc = 0;
					clk_time = 0;
					div_clk_time = 0;
					envelope_running = 0;
					env_state = WAIT;
				}
				set_rgb_led(LED_PING, c_OFF);
				div_ping_led=0;//SEG: added
			}
		}
	}
}

void do_reset_envelopes(void)
{
	uint32_t elapsed_time;

	reset_now_flag=0;

	if (envout<0x0010)
		outta_sync=0; // if we're practically at bottom, then consider us in sync and do an immediate transition

	if ((!envelope_running || (outta_sync==0) || (div_clk_time<0x80))) //was div_clk_time<0x8000
	{
		envelope_running=1;
		env_state=RISE;
		accum=0;
		eof_on();
		eor_off();
	}
	else
	{
		if (outta_sync==1) outta_sync=2;
		env_state=TRANSITION;
		//envelope_running=1; ///shouldn't we have this ??
		
		elapsed_time=128; //was 0x8000. offset to account for transition period: 64/128 timer overflows (6/13ms)
		if (elapsed_time>div_clk_time)
			elapsed_time-=div_clk_time;

		if (elapsed_time <= rise_time)  //Does our transition length exceed the rise time?
		{
			//time_tmp=((uint64_t)elapsed_time) << 12; //0x80000 = 524288
			//new_dacout = time_tmp/rise_time;
			new_dacout = (elapsed_time<<12)/rise_time;
			new_accum = new_dacout << 19;
			new_dacout = calc_curve(new_dacout, next_curve_rise);
			next_env_state=RISE;
		}
		else
		{
			elapsed_time = elapsed_time-rise_time;
			// time_tmp = ((uint64_t)elapsed_time) << 12;
			// new_dacout = time_tmp/fall_time;
			new_dacout = (elapsed_time<<12)/fall_time;
			if (new_dacout < 4096)
				new_dacout = 4096 - new_dacout;
			else
				new_dacout = 0;
			new_accum = new_dacout << 19;
			new_dacout = calc_curve(new_dacout,next_curve_fall);
			next_env_state=FALL;
		}

		//split (new_dacout - envout) into 128 pieces (>>7), but in units of accum (<<19) ===> <<12 
		if (new_dacout > envout)
			transition_inc = (new_dacout - envout) << 12; //was 9
		else {
			transition_inc = (envout - new_dacout) << 12; //was 9
			transition_inc = -1 * transition_inc;
		}

		cur_curve=LIN;
		accum=envout<<19;

		transition_ctr=128;
	}

	curve_rise=next_curve_rise;
	curve_fall=next_curve_fall;

	reset_nextping_flag=0;
	ticks_since_envout_running_total=0;
}

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
			envout=0;
			if (env_state==TRANSITION)
				DEBUGON;
			else
				DEBUGOFF;

			switch (env_state)
			{
				case (RISE):
					ticks_since_envout_running_total += use_ticks_since_envout;
					accum += rise_inc*use_ticks_since_envout;
					envout = accum>>19;

					if (accum > 0x7FF80000)
					{
						accum = 0x7FF80000;
						envout = 0x0FFF;
						if (triga_down && settings.async_can_sustain)
							end_segment_flag = SUSTAIN;
						else
							end_segment_flag = FALL;
					}
					
					cur_curve=curve_rise;

					if (envout>=2048) hr_on();
					else hr_off();
					eor_off();
					eof_on();
				break;

				case (SUSTAIN):
					eor_off();
					eof_off();
					hr_on();

					envout=0x0FFF;

					if (triga_down && settings.async_can_sustain)
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
					envout=accum>>19;

					if ((accum<0x00080000) || (accum>0x7FF80000))
					{
						accum = 0;
						envout = 0;
						end_env_flag = 1;
					}

					eor_on();
					eof_off();
					if (envout<2048)	hr_off();
					else hr_on();

					cur_curve=curve_fall;
				break;

				case (TRANSITION): 
					ticks_since_envout_running_total += use_ticks_since_envout;
					accum += transition_inc*use_ticks_since_envout;
					if (accum < 0x00080000 || (transition_inc==0)) //trans_inc==0 would technically be an error, so this gives us an out
					{
						accum = 0;
						envout = 0;
						transition_ctr = use_ticks_since_envout;
					}
					else if (accum>0x7FF80000)
					{
						accum = 0x7FF80000;
						envout = 0x0FFF;
						transition_ctr = use_ticks_since_envout;
					}
					else
						envout = accum>>19;

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
						accum = new_accum;

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
		
			envout = calc_curve(envout, cur_curve);
			output_envelope(envout);

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
			ticks_since_envout = 0;
		}
	}
}

int8_t calc_divided_ping_div_ctr(uint8_t envstate)
{
	int8_t pdc;
	uint32_t rise_per_clk;
	uint32_t temp_u32;
	uint8_t temp_u8;

	if (envstate==RISE)
	{
		pdc=0;
		if (rise_time>clk_time)
		{ 
			rise_per_clk=(rise_inc>>8) * (clk_time>>8);
			if (rise_per_clk<32) rise_per_clk=32;
			temp_u32=0;
			temp_u8=0;
			while ((temp_u32<=0x00007FF8) && (temp_u8<=clock_divider_amount)){ //was temp_u32<0x10000000
				temp_u32+=rise_per_clk;
				temp_u8++;
				if ((accum>>16)>=temp_u32) pdc++;
			}
		}
	}
	else if (envstate==FALL)
	{
		pdc=clock_divider_amount-1;
		if (fall_time>clk_time) //otherwise leave pdc at the last step
		{
			rise_per_clk=(fall_inc>>8) * (clk_time>>8);//was fall_inc * (clk_time>>8);
			if (rise_per_clk<32) rise_per_clk=32;
			temp_u32=0;
			temp_u8=0;
			while ((temp_u32<=0x00007FF8) && (temp_u8<=clock_divider_amount)){
				temp_u32+=rise_per_clk;
				temp_u8++;
				if ((accum>>16)>=temp_u32) pdc--;
			}
		}
	}
	else
		pdc = ping_div_ctr;

	return pdc;
}

void update_adc_params(uint8_t force_params_update)
{
	uint16_t d;
	int16_t total_adc;
	uint64_t time_tmp = 0;
	uint8_t temp_u8;
	// uint32_t temp_u32;
	uint32_t elapsed_time;
	uint8_t update_risefallincs;
	int8_t t_clock_divider_amount=1;
	int8_t hys_clock_divider_amount=0;
	int16_t cv;
	// uint32_t rise_per_clk=0;

	static uint16_t oversample_wait_ctr=0;
	static int8_t new_clock_divider_amount=1;
	static uint16_t prev_clock_div = 0;

	if (force_params_update || ++poll_user_input>USER_INPUT_POLL_TIME)
	{
		// DEBUGON;
		poll_user_input=0;

		update_risefallincs=0;

		cv = 2048 - analog[CV_SHAPE].lpf_val;
		if (cv>-20 && cv<20) cv = 0;
		
		total_adc = cv + analog[POT_SHAPE].lpf_val;
		if (total_adc>4095) total_adc=4095;
		else if (total_adc<0) total_adc=0;
		
		if (diff(total_adc, shape)>ADC_DRIFT)
		{
			shape = total_adc;
			update_risefallincs = 1;
			new_clock_divider_amount=clock_divider_amount;

			calc_skew_and_curves(shape, &skew, &next_curve_rise, &next_curve_fall);
		}

		cv = 2048 - analog[CV_DIVMULT].lpf_val;
		if (cv>-20 && cv<20) cv = 0;

		total_adc = cv + analog[POT_DIVMULT].lpf_val;
		if (total_adc>4095) total_adc=4095;
		else if (total_adc<0) total_adc=0;

		d = diff(total_adc, prev_clock_div);

		if ((env_state==TRANSITION) && envelope_running) {
			;  //do nothing
		}
		else if (d > DIV_ADC_HYSTERESIS)
		{ 
			prev_clock_div = total_adc;
			new_clock_divider_amount = get_clk_div_nominal(prev_clock_div);

			if (clk_time)
			{
				update_risefallincs = 1;
				divmult_changed = 1;
			} else {
				clock_divider_amount = new_clock_divider_amount;
			}

		}
		else if (d>0)
		{ 
			prev_clock_div = total_adc;
			t_clock_divider_amount = get_clk_div_nominal(prev_clock_div); 

			if (t_clock_divider_amount > clock_divider_amount)
			{
				if (prev_clock_div <= (0xFF - DIV_ADC_HYSTERESIS)) //Make sure we don't overflow past 0xFF
					temp_u8 = prev_clock_div + DIV_ADC_HYSTERESIS;
				else
					temp_u8 = 0xFF;
			
				hys_clock_divider_amount = get_clk_div_nominal(temp_u8);
			}
			else if (t_clock_divider_amount < clock_divider_amount)
			{
				if (prev_clock_div > DIV_ADC_HYSTERESIS)
					temp_u8 = prev_clock_div - DIV_ADC_HYSTERESIS;
				else
					temp_u8 = 0;

				hys_clock_divider_amount=get_clk_div_nominal(temp_u8);
			}
			else
				hys_clock_divider_amount = 99; //clock_divider_amount has not changed, do nothing
		
			if (hys_clock_divider_amount == t_clock_divider_amount)
			{
				new_clock_divider_amount = t_clock_divider_amount;

				if (clk_time)
				{
					update_risefallincs = 1;
					divmult_changed = 1;
				} else
					clock_divider_amount = new_clock_divider_amount; 
			}
		}


		// Update to the new envelope shape 

		//if it's 0 then that means we already did a TRANSITION for the update_risefallincs
		//increment it so that we know how many ADC read cycles we've done since it was reset to 1
		//(which happens when update_risefallincs is true)
		if (didnt_change_divmult)
		{
			didnt_change_divmult++; 
			// DEBUGON;
		} 
		// else 
		// 	DEBUGOFF;			

		if (update_risefallincs)
		{
			didnt_change_divmult = 1; //reset this to 1 which indicates we've made a change to risefallincs
			if (envelope_running && sync_to_ping_mode)
				tracking_changedrisefalls = 1;
			
			async_env_changed_shape = 1;

			if (divmult_changed)
			{
				clock_divider_amount = new_clock_divider_amount;	
				if (ping_div_ctr < 0) ping_div_ctr=0;
				if (ping_div_ctr > clock_divider_amount) ping_div_ctr = clock_divider_amount;

				div_clk_time = get_clk_div_time(new_clock_divider_amount, clk_time);
			}

			calc_rise_fall_incs();

			update_risefallincs=0;
		}

			/************* SLEW LIMITED TRANSITION *************
				Only do a slew-limited transition if we're running locking to ping (which is always, in some versions)
				
				Calculate the new accum value if we were to switch instantly
				  This will be used to calculate the slope of the transistion segment
				  which is necessary so we don't chop instantly (but instead set the transistion time to 10ms)

				 Reset didnt_change_divmult to 0, so that it won't increment (above) and we will then be locked out of this 
				 TRANSITION block until update_risefallincs goes 1 again
			****************************************************/

		if (div_clk_time && sync_to_ping_mode && (didnt_change_divmult>=NUM_ADC_CYCLES_BEFORE_TRANSITION))
		{
			tracking_changedrisefalls = 0;
			didnt_change_divmult = 0;	
			divmult_changed = 0;

			if (using_tap_clock)
				elapsed_time = tapouttmr;
			else
				elapsed_time = pingtmr;

			if (clock_divider_amount<=1) //Multiplying
			{	
				while (elapsed_time > div_clk_time)
					elapsed_time -= div_clk_time;
			}
			else //Dividing
			{
				if (envelope_running)
				{
					ping_div_ctr = calc_divided_ping_div_ctr(env_state);

					while (elapsed_time >= clk_time)
						elapsed_time -= clk_time;
					elapsed_time += (ping_div_ctr*clk_time);
				}
				else //envelope not running
				{ 
					ping_div_ctr=clock_divider_amount; //the next ping will be the one that the envelope starts on
					while (elapsed_time <= div_clk_time)
						elapsed_time += clk_time;
					elapsed_time -= clk_time;
				}
			}

			divpingtmr = elapsed_time;// was >>8;
			
			if (envelope_running)
			{
				elapsed_time += (uint32_t)0x80; //was 0x8000. offset to account for transition period: 128 timer overflows
				if (elapsed_time > div_clk_time)
					elapsed_time -= div_clk_time;

				if (elapsed_time <= rise_time) {  //Are we on the rise?
					time_tmp = ((uint64_t)elapsed_time) << 12; //12 bits
					new_dacout = time_tmp/rise_time;
					if (new_dacout > 4095)
						new_dacout = 4095;
					new_accum = new_dacout << 19;
					new_dacout = calc_curve(new_dacout,next_curve_rise);
					next_env_state = RISE;
				} else {
					elapsed_time = elapsed_time - rise_time;
					time_tmp = ((uint64_t)elapsed_time) << 12; 
					new_dacout = time_tmp / fall_time;
					if (new_dacout < 4095)
						new_dacout = 4095 - new_dacout;
					else
						new_dacout = 0;
					new_accum = new_dacout << 19;
					new_dacout = calc_curve(new_dacout, next_curve_fall);
					next_env_state = FALL;
				}

				accum = envout << 19;
	
				//split (new_dacout - envout) into 128 pieces (>>7), but in units of accum (<<19) ===> <<12 
				if (new_dacout > envout)
					transition_inc = (new_dacout - envout) << 12;
				else {
					transition_inc = (envout - new_dacout) << 12;
					transition_inc = -1 * transition_inc;
				}
				cur_curve = LIN;

				env_state = TRANSITION;
				transition_ctr = 128;

				outta_sync = 1;
				ready_to_start_async = 1;	
			}

			if ((clock_divider_amount>1) && envelope_running)
			{
				ping_div_ctr = calc_divided_ping_div_ctr(next_env_state);
			}

		}
		// DEBUGOFF;
	}
	else
	{
		if (++oversample_wait_ctr>25)
		{
			oversample_wait_ctr=0;
			// DEBUGON;
			condition_analog();
			// DEBUGOFF;
		}
	}

}