#include "params.h"
#include "analog_conditioning.h"
#include "calibration.hh"
#include "debounced_digins.h"
#include "dig_inout_pins.hh"
#include "env_transition.h"
#include "envelope_calcs.h"
#include "flash_user.hh"
#include "pingable_env.h"
#include "util/math.hh"

extern struct PingableEnvelope m;
extern uint32_t clk_time;
extern struct SystemSettings settings;
extern analog_t analog[NUM_ADCS];
extern int16_t cycle_latched_offset;
extern bool adjusting_shift_mode;

int32_t scale, offset, shift;

static uint16_t shape;
static int8_t read_divmult();
static uint8_t read_shape_scale_offset();
static void update_clock_divider_amount(struct PingableEnvelope *e, int16_t new_clock_divider_amount);
static void update_env_tracking(struct PingableEnvelope *e);

//Settings:
#define USER_INPUT_POLL_TIME 80

constexpr inline int32_t SCALE_PLATEAU_WIDTH = 100;
constexpr inline int32_t OFFSET_PLATEAU_WIDTH = 100;
constexpr inline int32_t SHAPECV_PLATEAU_WIDTH = 100;
constexpr inline int32_t DIVMULTCV_PLATEAU_WIDTH = 100;

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
			DigIO::DebugOut::high();
			calc_skew_and_curves(shape, &m.skew, &m.next_curve_rise, &m.next_curve_fall);
			calc_rise_fall_incs(&m);
			update_env_tracking(&m);

			reset_transition_counter();
			DigIO::DebugOut::low();
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

	{
		int16_t adj_scale = analog[POT_SCALE].lpf_val + settings.center_detent_offset[DET_SCALE];
		scale = MathTools::plateau(adj_scale, SCALE_PLATEAU_WIDTH, 2048);
	}

	{
		int16_t adj_offset = analog[POT_OFFSET].lpf_val + settings.center_detent_offset[DET_OFFSET];
		int16_t tmp = MathTools::plateau(adj_offset, OFFSET_PLATEAU_WIDTH, 2048);
		if (is_pressed(CYCLE_BUTTON) && (MathTools::diff(cycle_latched_offset, analog[POT_OFFSET].lpf_val) > 40))
			adjusting_shift_mode = true;

		if (adjusting_shift_mode)
			shift = 2048 + tmp;
		else
			offset = tmp - 2048;
	}

	{
		int16_t shape_cv = MathTools::plateau(2048 - analog[CV_SHAPE].lpf_val, SHAPECV_PLATEAU_WIDTH, 0);
		int16_t shape_total = shape_cv + analog[POT_SHAPE].lpf_val + settings.center_detent_offset[DET_SHAPE];
		shape_total = MathTools::constrain(shape_total, (int16_t)0, (int16_t)4095);
		if (MathTools::diff((uint16_t)shape_total, shape) > ADC_DRIFT) {
			shape = shape_total;
			update_risefallincs = 1;
		}
	}

	return update_risefallincs;
}

// Reads Divmult pot and cv
// returns updated clock divider
// amount or 0 if no change
static int8_t read_divmult(void) {
	static int16_t last_total_adc = 0;
	static int8_t last_clock_divider_amount = 0;

	int16_t total_adc;
	int8_t t_clock_divider_amount = 1;
	int8_t hys_clock_divider_amount = 0;
	int8_t new_clock_divider_amount = 0;

	int16_t cv = MathTools::plateau(2048 - analog[CV_DIVMULT].lpf_val, DIVMULTCV_PLATEAU_WIDTH, 0);
	cv = 2048 - analog[CV_DIVMULT].lpf_val;
	if (cv > -20 && cv < 20)
		cv = 0;

	total_adc = cv + analog[POT_DIVMULT].lpf_val;
	if (total_adc > 4095)
		total_adc = 4095;
	else if (total_adc < 0)
		total_adc = 0;

	uint16_t d = MathTools::diff(total_adc, last_total_adc);

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
