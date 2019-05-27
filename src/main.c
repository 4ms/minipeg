#include "stm32f0xx_conf.h"
#include "globals.h"
#include "dig_inouts.h"
#include "pwm.h"
#include "adc.h"
#include "exti.h"
#include "envelope.h"
#include "flash_user.h"
#include "log4096.h"

volatile uint32_t tapouttmr;
volatile uint32_t tapintmr;
volatile uint32_t pingtmr;
volatile uint32_t divpingtmr;
volatile uint32_t trigouttmr;
volatile uint8_t timer_overflowed=0;
volatile uint32_t ping_irq_timestamp=0;
volatile uint32_t trig_irq_timestamp=0;

uint32_t last_tapin_time=0;
uint16_t timer_overflowed_running_total=0;
char use_timer_overflowed=0;
uint32_t elapsed_time=0;
uint32_t tapout_clk_time=0;

uint32_t clk_time=0;
uint32_t div_clk_time=0;
uint32_t rise_time=0;
uint32_t fall_time=0;
uint32_t rise_inc=0;
uint32_t fall_inc=0;

char curve_rise=LIN;
char curve_fall=LIN;
char cur_curve=LIN;

uint32_t async_phase_diff=0;
uint8_t async_env_changed_shape=1;

int8_t clock_divider_amount=1;
int8_t new_clock_divider_amount=1;

char envelope_running=0;
enum envelopeStates env_state=WAIT;
char next_env_state=WAIT;

int32_t accum=0;
int16_t t_dacout=0;
int32_t new_accum=0;
int16_t new_dacout=0;


uint8_t trigout_high=0;
volatile uint8_t trig_jack_down=0;
volatile uint8_t using_tap_clock=0;

volatile uint8_t reset_nextping_flag=0;
volatile uint8_t sync_to_ping_mode=1;
char reset_now_flag=0;
char end_env_flag=0;
char end_segment_flag=0;
uint8_t update_risefallincs=0;
char divmult_changed=0;
uint8_t didnt_change_divmult=0;
char tracking_changedrisefalls=0;
int32_t transition_inc=0;
int8_t transition_ctr=0;
char outta_sync=0;
uint32_t rise_per_clk=0;

int8_t ping_div_ctr=0;
char div_ping_led=0;
char ready_to_start_async=0;
char got_tap_clock=0;

uint32_t entering_system_mode=0;
uint8_t system_mode_cur=0;
uint8_t initial_cycle_button_state=0;
char update_cycle_button_now=0;


//Settings
char NO_FREERUNNING_PING=0;
char LIMIT_SKEW=0;
char ROLLOFF_PING=1;
char TRIG_IS_ASYNC=1;
char TRIG_CAN_SUSTAIN=1;

char TRIGOUT_IS_ENDOFRISE = 1;
char TRIGOUT_IS_ENDOFFALL = 0;
char TRIGOUT_IS_HALFRISE = 0;
char TRIGOUT_IS_TAPCLKOUT = 0;
char TRIGOUT_IS_TRIG = 0;

volatile uint16_t adc_buffer[NUM_ADCS];
int16_t t_dacout[6]={0};

void SysTick_Handler(void)
{
	tapoutmr++;
	tapintmr++;
	pingtmr++;
	divpingtmr++;
	trigouttmr++;
	timer_overflowed++;
}

void init_tmrs(void)
{
	tapoutmr=0;
	tapintmr=0;
	pingtmr=0;
	divpingtmr=0;
	trigouttmr=0;
}

int main(void)
{
	uint64_t time_tmp=0;
	int8_t t_clock_divider_amount=1;
	int8_t hys_clock_divider_amount=0;
	int8_t c_d_a=1;

	init_tmrs();
	init_dig_inouts();
	init_adc(adc_buffer, NUM_ADCS);

	init_pwm();
	SysTick_Config(SystemCoreClock/10000); //1000 is 1ms ticks, 10000 is 100us ticks, 100000 is 10us ticks

	init_EXTI_inputs();
	init_rgb_leds();
	init_palette();
	
	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);
	set_mono_led(PWM_TRIGOUTLED, 0);
	set_mono_led(PWM_ENVLED, 0);

	eor_off();
	eof_off();
	hr_off();
	tapclkout_off();

	//read settings from flash

	clk_time=0;
	div_clk_time=0;
	last_tapin_time=0;
	sync_to_ping_mode=1;
	accum=0;

	while (1) {

		read_taptempo();
		read_trigjacks();


	}

}

void read_taptempo(void)
{
	uint32_t now;
	static uint8_t tapin_down=0;
	static uint8_t tapin_up=0;


	if (PINGBUT){
		tapin_down=0;
		now=tapintmr;

		if (!(tapin_up)){
			tapin_up=1;
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
			fall_time=get_fall_time(skew_adc, div_clk_time);
			rise_time=div_clk_time-fall_time;
			rise_inc=udiv32(rise_time>>5);
			fall_inc=udiv32(fall_time>>5);

			didnt_change_divmult=NUM_ADC_CYCLES_BEFORE_TRANSITION; //force us to do a transition mode as if divmult or skew was changed
			poll_user_input=USER_INPUT_POLL_TIME; //force us to enter the ADC block

		} else {
			if (now > HOLDTIMECLEAR){ //button has been down for more than 2 seconds

				if (using_tap_clock){
					env_state=WAIT;
					envelope_running=0;

					divpingtmr=0;
					clk_time=0;
					div_clk_time=0;

					accum=0;
					using_tap_clock=0;
					timer_overflowed=1;
				}
				tapout_clk_time=0;
				last_tapin_time=0;
				tapouttmr=0;

				set_rgb_led(PINGLED, c_OFF);

			} else {
				set_rgb_led(PINGLED, c_WHITE);
			}

		}
	}
	else {
		tapin_up=0;
		if (!(tapin_down)){
			PINGLEDLOW;
			tapin_down=1;
		}
	}
}

void read_trigjacks(void)
{
	static uint8_t triga_jack_down=0;
	
	if (trig_irq_timestamp){ //this is set when the pinchange interrupt detects a TRIGQ jack going high
		trig_irq_timestamp=0; //clear it so we only run this once per TRIGQ

		if (TRIG_IS_ASYNC){
			if (TRIGJACK){
				if (!triga_jack_down){
				
					sync_to_ping_mode=0;
					triga_jack_down=1;

					reset_now_flag=1;
					ready_to_start_async=0;
					
					if (rise_time>0x1000)  //do an immediate fall if rise_time is fast
						outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero

					//start each envelope the same time after the divided clock
					async_phase_diff=get_divpingtmr();
					
				}


			}
			else
				triga_jack_down=0;

		}
		else {
			/*
			When Cycle is on, a TRIGQ should always re-phase (re-start) a divided-ping env.
			When Cycle is off, if and only if QNT_REPHASES_WHEN_CYCLE_OFF is set, then a TRIGQ should rephase 
			
			If the envelope is not running, a TRIGQ should always start the envelope.
			
			ping_div_ctr=clock_divider_amount makes sure that a divided-ping envelope will start from its beginning on the next ping
			*/
			if (QNT_REPHASES_WHEN_CYCLE_OFF || CYCLE_BUT || !envelope_running){
				ping_div_ctr=clock_divider_amount;
				if (rise_time>0x1000)  //do an immediate fall if rise_time is fast
					outta_sync=1;		//otherwise set the outta_sync flag which works to force a slew limited transition to zero
			}

			tracking_changedrisefalls=0;

			curve_rise=next_curve_rise;
			curve_fall=next_curve_fall;
		}

}

