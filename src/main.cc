#include "adc.h"
#include "analog_conditioning.h"
#include "calibration.hh"
#include "dac.h"
#include "debounced_digins.h"
#include "dig_inouts.hh"
#include "drivers/system.hh"
#include "env_transition.h"
#include "env_update.h"
#include "flash_layout.hh"
#include "flash_user.hh"
#include "hardware_tests.h"
#include "leds.h"
#include "math_util.h"
#include "params.h"
#include "pingable_env.h"
#include "pwm.h"
#include "settings.h"
#include "stm32xx.h"
#include "system.hh"
#include "system_mode.h"
#include "timekeeper.h"
#include "timers.h"
#include "trigout.h"
#include "version.hh"

extern debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
extern analog_t analog[NUM_ADCS];
extern struct SystemSettings settings;
extern volatile uint32_t systmr;
extern volatile uint32_t tapouttmr;
extern volatile uint32_t tapintmr;
extern volatile uint32_t pingtmr;
extern volatile uint32_t trigouttmr;

volatile uint32_t ping_irq_timestamp = 0;
volatile uint32_t trig_irq_timestamp = 0;

uint32_t last_tapin_time = 0;
uint32_t tapout_clk_time = 0;

uint32_t clk_time = 0;

uint8_t cycle_but_on = 0;

volatile uint8_t using_tap_clock = 0;
uint8_t do_toggle_cycle = 0;

uint8_t force_params_update = 0;

char div_ping_led = 0;
char got_tap_clock = 0;

uint32_t entering_system_mode = 0;
uint8_t system_mode_cur = 0;
uint8_t initial_cycle_button_state = 0;
char update_cycle_button_now = 0;

uint8_t adjusting_shift_mode = 0;
int16_t cycle_latched_offset;

struct PingableEnvelope m;

// main.h
static void read_ping_button(void);
static void read_trigjacks(void);
static void read_cycle_button(void);
static void check_reset_envelopes(void);
static void update_tap_clock(void);
static void read_ping_clock(void);
static void ping_led_off();
static void ping_led_on();

static const uint32_t kDacSampleRate = 40000;

void main() {

	mdrivlib::System::SetVectorTable(AppFlashAddr);
	system_init();
	SysTick_Config(SystemCoreClock / (TICKS_PER_MS * 1000));

	init_timekeeper();
	init_pingable_env(&m);
	init_dig_inouts();
	eor_off();
	eof_off();
	hr_off();
	tapclkout_off();

	init_analog_conditioning();

	init_pwm();
	all_lights_off();

	if (!read_settings()) {
		test_hardware();
		write_settings();
	}
	HAL_Delay(10);
	if (PINGBUT && CYCLEBUT) {
		test_hardware();
	}

	init_debouncer();

	HAL_Delay(50);
	read_settings();
	adjust_palette();
	check_calibration();
	adjust_palette();

	Version::show_version();

	init_dac(kDacSampleRate, &update_all_envelopes);

	init_params();

	// Todo: store each envelope running separately
	if (settings.start_cycle_on) {
		cycle_but_on = 1;
		set_rgb_led(LED_CYCLE, c_ORANGE);
		m.envelope_running = 1;
		using_tap_clock = 1;
	} else {
		cycle_but_on = 0;
		set_rgb_led(LED_CYCLE, c_OFF);
		m.envelope_running = 0;
	}

	if (settings.start_clk_time) {
		clk_time = settings.start_clk_time;
		m.div_clk_time = settings.start_clk_time;
	}

	last_tapin_time = 0;

	while (1) {
		// G0: loops every ~11uS, maybe 13us if you include envelope updates every
		// 4th loop G4: loops every ~2uS, with ~10us gaps
		// p4 unit no-lock: 1.2uS fastest loop. Max 10us loop... average 530kHz

		// DEBUGON;
		read_ping_button();
		read_trigjacks();
		read_cycle_button();
		check_reset_envelopes();

		// DEBUGOFF;
		update_tap_clock();
		read_ping_clock();
		update_adc_params(force_params_update);
		force_params_update = 0;

		handle_trigout_trigfall();

		handle_system_mode();
	}
}

static void read_ping_button(void) {
	if (digin[PING_BUTTON].state == 1) {
		uint32_t now = tapintmr;

		if (digin[PING_BUTTON].edge == 1) {
			digin[PING_BUTTON].edge = 0;

			using_tap_clock = 1;

			if (last_tapin_time && (diff32(last_tapin_time, now) < (last_tapin_time >> 1))) {
				clk_time = (now >> 1) + (last_tapin_time >> 1);
			} else {
				clk_time = now;
				last_tapin_time = now;
			}

			tapout_clk_time = clk_time;
			tapintmr = 0;
			tapouttmr = 0;

			calc_div_clk_time(&m, clk_time);

			force_transition();
			force_params_update = 1;
		} else {
			if (now > HOLDTIMECLEAR) { // button has been down for more than 2 seconds
				if (using_tap_clock) {
					stop_envelope(&m);
					clk_time = 0;
					using_tap_clock = 0;
				}
				tapout_clk_time = 0;
				last_tapin_time = 0;
				tapouttmr = 0;
				ping_led_off();
			} else
				ping_led_on();
		}
	}

	if (digin[PING_BUTTON].edge == -1) {
		ping_led_off();
		digin[PING_BUTTON].edge = 0;
	}
}

void handle_qnt_trig(struct PingableEnvelope *e) {
	e->triga_down = 0;
	e->trigq_down = 1;
	e->sync_to_ping_mode = 1;
	e->reset_nextping_flag = 1;

	if (QNT_REPHASES_WHEN_CYCLE_OFF || cycle_but_on || !e->envelope_running) {
		e->ping_div_ctr = e->clock_divider_amount; // make sure divided clock envelopes start from
												   // beginning on next ping
		if (e->rise_time > 0x10)				   // was 0x1000
			e->outta_sync = 1;
	}
	e->tracking_changedrisefalls = 0;
	e->curve_rise = e->next_curve_rise;
	e->curve_fall = e->next_curve_fall;
}

void handle_async_trig(struct PingableEnvelope *e) {
	e->triga_down = 1;
	e->trigq_down = 0;
	e->sync_to_ping_mode = 0;
	e->reset_now_flag = 1;
	e->ready_to_start_async = 0;

	if (e->rise_time > 0x1000) // FixMe: should this be 0x10 ?? //do an immediate
							   // fall if rise_time is fast
		e->outta_sync = 1;	   // otherwise set the outta_sync flag which works to force
							   // a slew limited transition to zero

	e->async_phase_diff = e->divpingtmr;
}

void read_trigjacks(void) {
	if (digin[TRIGGER_JACK].edge == 1) {
		digin[TRIGGER_JACK].edge = 0;
		if (settings.trigin_function == TRIGIN_IS_QNT)
			handle_qnt_trig(&m);
		else
			handle_async_trig(&m);
	}

	if (digin[TRIGGER_JACK].edge == -1) {
		digin[TRIGGER_JACK].edge = 0;
		m.triga_down = 0;
		m.trigq_down = 0;
	}

	if (digin[CYCLE_JACK].edge == 1) {
		digin[CYCLE_JACK].edge = 0;
		do_toggle_cycle = 1;
	}

	if (digin[CYCLE_JACK].edge == -1) {
		digin[CYCLE_JACK].edge = 0;

		if (settings.cycle_jack_behavior == CYCLE_JACK_BOTH_EDGES_TOGGLES)
			do_toggle_cycle = 1;
	}
}

void update_trigout(void) {
	if (m.env_state == RISE) {
		if ((m.accum >> 19) >= 2048)
			hr_on();
		else
			hr_off();
		eor_off();
		eof_on();
	} else if (m.env_state == FALL) {
		if ((m.accum >> 19) < 2048)
			hr_off();
		else
			hr_on();
		eor_on();
		eof_off();
	}
}

static void read_cycle_button(void) {
	if (digin[CYCLE_BUTTON].edge == 1) {
		cycle_latched_offset = analog[POT_OFFSET].lpf_val;
		digin[CYCLE_BUTTON].edge = 0;
	}

	if ((digin[CYCLE_BUTTON].edge == -1) && adjusting_shift_mode) {
		adjusting_shift_mode = 0;
		digin[CYCLE_BUTTON].edge = 0;
	}

	if (digin[CYCLE_BUTTON].edge == -1 || do_toggle_cycle) {
		digin[CYCLE_BUTTON].edge = 0;
		do_toggle_cycle = 0;

		if (cycle_but_on == 0) {
			cycle_but_on = 1;
			set_rgb_led(LED_CYCLE, c_ORANGE);

			if (clk_time > 0) {
				calc_div_clk_time(&m, clk_time);

				start_envelope(&m);

				update_trigout();
			}
		} else {
			cycle_but_on = 0;
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}
}

static void ping_led_off(void) {
	set_rgb_led(LED_PING, c_OFF);
	div_ping_led = 0;
}

static void ping_led_on(void) {
	set_rgb_led(LED_PING, c_WHITE);
	div_ping_led = 1;
}

// Todo: this should be done when divpingtmr is updated, or when div_clk_time is
// updated void resync_on_divpingtmr(void)
void check_reset_envelopes(void) {
	check_restart_async_env(&m);

	if (div_ping_led && (m.divpingtmr >= (m.div_clk_time >> 1)))
		ping_led_off();

	if (m.div_clk_time) {
		if ((!div_ping_led) && (m.divpingtmr > m.div_clk_time)) { // Todo: why do we have !div_ping_led
																  // && ( > ). Do we need that?
			m.divpingtmr = m.divpingtmr - m.div_clk_time;
			sync_env_to_clk(&m);
			ping_led_on();
		}

	} else
		ping_led_off();
}

// Todo: this only needs to be done when tapouttmr updates
void update_tap_clock(void) {
	if (tapout_clk_time) {
		if (tapouttmr >= tapout_clk_time) {
			tapouttmr = 0;
			if (using_tap_clock) {
				// Todo: Yet another sync to the clock,is this needed?
				if (m.clock_divider_amount <= 1)
					m.divpingtmr = 0;

				got_tap_clock = 1;
			}
			tapclkout_on();
		} else if (tapouttmr >= (tapout_clk_time >> 1))
			tapclkout_off();
	}
}

void read_ping_clock(void) {
	if (got_tap_clock || ping_irq_timestamp) {
		if (ping_irq_timestamp) {
			uint32_t prev_clk_time = clk_time;
			clk_time = ping_irq_timestamp;

			if (prev_clk_time) {
				uint32_t delta = diff32(clk_time, prev_clk_time);
				if (delta > (prev_clk_time >> 3)) //>12.5%
				{
					force_transition();
					force_params_update = 1;
				}
			}
		}
		ping_irq_timestamp = 0;
		got_tap_clock = 0;

		if (!using_tap_clock)
			last_tapin_time = 0;

		if (m.clock_divider_amount <= 1) {
			ping_led_on();
			// if (!using_tap_clock)
			m.divpingtmr = 0;
		}

		calc_div_clk_time(&m, clk_time);

		if (resync_on_ping(&m))
			ping_led_on();

	} else {
		/*	If we haven't received a ping within 2x expected clock time (that is,
       clock stopped or slowed to less than half speed) we should stop the ping
       clock. Or switch to the Tap clock if it's running and we have Tap Clock
       Output on EOF
    */
		if (clk_time && !settings.free_running_ping && !using_tap_clock) {
			uint32_t ext_ping_timeout = clk_time * 2;
			if (pingtmr >= ext_ping_timeout) {
				pingtmr = 0;

				if (tapout_clk_time && (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)) {
					// switch to tap clock
					using_tap_clock = 1;
					clk_time = tapout_clk_time;

					calc_div_clk_time(&m, clk_time);

					m.reset_now_flag = 1;
				} else {
					stop_envelope(&m);
					clk_time = 0;
				}
				ping_led_off();
			}
		}
	}
}

extern "C" void SysTick_Handler(void) {
	HAL_IncTick();
}
