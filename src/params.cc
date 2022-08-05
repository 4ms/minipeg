#include "params.h"
#include "analog_conditioning.h"
#include "calibration.hh"
#include "debounced_digins.h"
#include "env_transition.h"
#include "envelope_calcs.h"
#include "flash_user.hh"
#include "math_util.h"
#include "pingable_env.h"

extern struct PingableEnvelope m;
extern uint32_t clk_time;
extern struct SystemSettings settings;
extern analog_t analog[NUM_ADCS];
extern int16_t cycle_latched_offset;
extern uint8_t adjusting_shift_mode;

int32_t scale, offset, shift;

static uint16_t shape;
static int8_t read_divmult(void);
static uint8_t read_shape_scale_offset(void);
static void update_clock_divider_amount(struct PingableEnvelope *e, int16_t new_clock_divider_amount);
static void update_env_tracking(struct PingableEnvelope *e);
static int16_t plateau(int16_t val, const uint16_t low, const uint16_t high);

//Settings:
#define USER_INPUT_POLL_TIME 80

#define SCALE_PLATEAU_WIDTH 100
#define SCALE_PLATEAU_LOW (2048 - (SCALE_PLATEAU_WIDTH / 2))
#define SCALE_PLATEAU_HIGH (2048 + (SCALE_PLATEAU_WIDTH / 2))

#define OFFSET_PLATEAU_WIDTH 100
#define OFFSET_PLATEAU_LOW (2048 - (OFFSET_PLATEAU_WIDTH / 2))
#define OFFSET_PLATEAU_HIGH (2048 + (OFFSET_PLATEAU_WIDTH / 2))

#define ADC_DRIFT 16
#define DIV_ADC_HYSTERESIS 16

void init_params(void) {
	//Todo: store shift value in flash
	shift = 2048; //=settings.shift_value
	offset = 0;
	scale = 0;
}

void update_adc_params(uint8_t force_params_update) {
	static uint16_t oversample_wait_ctr = 0;
	static uint16_t poll_user_input = 0;

	if (force_params_update || ++poll_user_input > USER_INPUT_POLL_TIME) {
		poll_user_input = 0;

		if (read_shape_scale_offset()) {
			calc_skew_and_curves(shape, &m.skew, &m.next_curve_rise, &m.next_curve_fall);
			calc_rise_fall_incs(&m);
			update_env_tracking(&m);

			reset_transition_counter();
		}

		if (m.env_state != TRANSITION ||
			!m.envelope_running) //Todo: remove !m.envelope_running and test. if m.env_state==TRANSITION, then m.envelope_running should never be true
		{
			int8_t new_clock_divider_amount = read_divmult();
			if (new_clock_divider_amount != 0) {
				update_clock_divider_amount(&m, new_clock_divider_amount);
				calc_rise_fall_incs(&m);
				update_env_tracking(&m);

				reset_transition_counter();
			}
		}

		if (check_to_start_transition()) {
			do_start_transition(&m);
		}
	} else {
		if (++oversample_wait_ctr > 4) {
			oversample_wait_ctr = 0;
			condition_analog();
		}
	}
}

// Reads Scale, Offset, and Shape CV and pots
// Updates global vars: shift, shape, offset
// Returns 1 if shape changed, 0 if not
static uint8_t read_shape_scale_offset(void) {
	uint8_t update_risefallincs = 0;
	int16_t total_adc;
	int16_t cv;

	total_adc = analog[POT_SCALE].lpf_val + settings.center_detent_offset[DET_SCALE];
	scale = plateau(total_adc, SCALE_PLATEAU_LOW, SCALE_PLATEAU_HIGH);

	total_adc = analog[POT_OFFSET].lpf_val + settings.center_detent_offset[DET_OFFSET];
	int16_t tmp = plateau(total_adc, OFFSET_PLATEAU_LOW, OFFSET_PLATEAU_HIGH);

	if (is_pressed(CYCLE_BUTTON) && (diff(cycle_latched_offset, analog[POT_OFFSET].lpf_val) > 40))
		adjusting_shift_mode = 1;

	if (adjusting_shift_mode)
		shift = 2048 + tmp;
	else
		offset = tmp - 2048;

	//Todo: plateau the CV
	cv = 2048 - analog[CV_SHAPE].lpf_val;
	if (cv > -20 && cv < 20)
		cv = 0;

	total_adc = cv + analog[POT_SHAPE].lpf_val + settings.center_detent_offset[DET_SHAPE];
	if (total_adc > 4095)
		total_adc = 4095;
	else if (total_adc < 0)
		total_adc = 0;

	if (diff(total_adc, shape) > ADC_DRIFT) {
		shape = total_adc;
		update_risefallincs = 1;
	}

	return update_risefallincs;
}

// Reads Divmult pot and cv
// returns updated clock divider
// amount or 0 if no change
static int8_t read_divmult(void) {
	static uint16_t last_total_adc = 0;
	static int8_t last_clock_divider_amount = 0;

	int16_t cv;
	int16_t total_adc;
	int8_t t_clock_divider_amount = 1;
	int8_t hys_clock_divider_amount = 0;
	int8_t new_clock_divider_amount = 0;

	cv = 2048 - analog[CV_DIVMULT].lpf_val;
	if (cv > -20 && cv < 20)
		cv = 0;

	total_adc = cv + analog[POT_DIVMULT].lpf_val;
	if (total_adc > 4095)
		total_adc = 4095;
	else if (total_adc < 0)
		total_adc = 0;

	uint16_t d = diff(total_adc, last_total_adc);

	if (d > DIV_ADC_HYSTERESIS) {
		last_total_adc = total_adc;
		new_clock_divider_amount = get_clk_div_nominal(total_adc);
		last_clock_divider_amount = new_clock_divider_amount;
	} else if (d != 0) {
		t_clock_divider_amount = get_clk_div_nominal(total_adc);

		if (t_clock_divider_amount > last_clock_divider_amount) {
			int16_t tmp = total_adc + DIV_ADC_HYSTERESIS;
			hys_clock_divider_amount = get_clk_div_nominal(tmp);
		} else if (t_clock_divider_amount < last_clock_divider_amount) {
			int16_t tmp = total_adc - DIV_ADC_HYSTERESIS;
			hys_clock_divider_amount = get_clk_div_nominal(tmp);
		} else
			hys_clock_divider_amount = 99; //clock_divider_amount has not changed, do nothing

		if (hys_clock_divider_amount == t_clock_divider_amount) {
			new_clock_divider_amount = t_clock_divider_amount;
			last_clock_divider_amount = new_clock_divider_amount;
			last_total_adc = total_adc;
		}
	}

	return new_clock_divider_amount;
}

static int16_t plateau(int16_t val, const uint16_t low, const uint16_t high) {
	if (val > high)
		return val - high;
	else if (val < low)
		return val - low;
	else
		return 0;
}

static void update_env_tracking(struct PingableEnvelope *e) {
	if (e->envelope_running && e->sync_to_ping_mode)
		e->tracking_changedrisefalls = 1;

	e->async_env_changed_shape = 1;
}

static void update_clock_divider_amount(struct PingableEnvelope *e, int16_t new_clock_divider_amount) {
	e->clock_divider_amount = new_clock_divider_amount;

	if (clk_time) {
		if (e->ping_div_ctr < 0)
			e->ping_div_ctr = 0;
		if (e->ping_div_ctr > e->clock_divider_amount)
			e->ping_div_ctr = e->clock_divider_amount;

		e->div_clk_time = get_clk_div_time(new_clock_divider_amount, clk_time);
	}
}
