
#include "globals.h"
#include "log4096.h"

extern const uint16_t log4096[4096];

extern struct SystemSettings settings;

const int8_t P_array[NUM_DIVMULTS] = {32, 16, 8, 7, 6, 5, 4, 3, 2, 1, -2, -3, -4, -5, -6, -7, -8, -12, -16};

int8_t get_clk_div_nominal(uint16_t adc_val) {
	uint8_t i;

	for (i = 0; i < NUM_DIVMULTS; i++) {
		if (adc_val <= settings.midpt_array[i])
			return (P_array[i]);
	}
	return (P_array[NUM_DIVMULTS - 1]);
}

void calc_div_clk_time(struct PingableEnvelope *e, uint32_t new_clk_time) {
	e->div_clk_time = get_clk_div_time(e->clock_divider_amount, new_clk_time);
	calc_rise_fall_incs(e);
}

uint32_t get_clk_div_time(int8_t clock_divide_amount, uint32_t clk_time) {
	if (clock_divide_amount > 1)
		return clk_time * clock_divide_amount;
	else if (clock_divide_amount < -1)
		return clk_time / (-1 * clock_divide_amount);
	else
		return clk_time;
}

void calc_rise_fall_incs(struct PingableEnvelope *e) {
	e->fall_time = get_fall_time(e->skew, e->div_clk_time);
	e->rise_time = e->div_clk_time - e->fall_time;
	e->rise_inc = (1UL << 31) / e->rise_time;
	e->fall_inc = (1UL << 31) / e->fall_time;
}

//skew: 0..255, 0 means fall=min
uint32_t get_fall_time(uint8_t skew, uint32_t div_clk_time) {
	// return div_clk_time/2;

	uint32_t skew_portion, u;
	uint8_t rev_skew;

	if (div_clk_time > 0xFFFFFF)
		skew_portion = (div_clk_time >> 8) * skew;
	else
		skew_portion = (skew * div_clk_time) >> 8;

	if (!settings.limit_skew || (div_clk_time < (LIMIT_SKEW_TIME >> 1))) {
		if (skew == 0)
			return (30);

		else if (skew == 1)
			return (40);

		else if (skew == 2)
			return (50);

		else if (skew <= 25) {
			//return smaller of skew_portion and squared link funcion, but at least 50
			if (skew_portion < 50)
				skew_portion = 50;
			u = skew * skew * 2;
			return (skew_portion < u) ? skew_portion : u;

		} else if (skew >= 255)
			return (div_clk_time - 20);

		else if (skew > 200) {
			if (skew_portion > (div_clk_time - 20))
				skew_portion = div_clk_time - 20;

			rev_skew = 255 - skew;
			u = rev_skew * rev_skew * 2;

			if (u > (div_clk_time - 20)) {
				return skew_portion;
			} else {
				u = div_clk_time - u;
				return (skew_portion > u) ? skew_portion : u;
			}
		}

		else if ((skew > 125) && (skew <= 129))
			return (div_clk_time >> 1);

		else
			return (skew * (div_clk_time >> 8));
	}

	else
	{ //LIMIT_SKEW

		if ((skew > 125) && (skew <= 129))
			return (div_clk_time >> 1);
		else {
			if (skew_portion < LIMIT_SKEW_TIME)
				skew_portion = LIMIT_SKEW_TIME;
			if (skew_portion > (div_clk_time - LIMIT_SKEW_TIME))
				skew_portion = div_clk_time - LIMIT_SKEW_TIME;

			return skew_portion;
		}
	}
}

#define SHAPE_REGION_SIZE 820 //(4095/NUM_SHAPE_REGIONS) + 1;

// shape: 0..4095 (adc value)
// returns skew: 0..255
// returns next_curve_rise and _fall: 0..255: expo/linear/log
void calc_skew_and_curves(uint16_t shape, uint8_t *skew, uint8_t *next_curve_rise, uint8_t *next_curve_fall) {
	uint16_t shape_region = shape / SHAPE_REGION_SIZE;
	uint16_t shape_variation = shape - shape_region * SHAPE_REGION_SIZE;

	uint8_t var_127; //shape variation normalized to 0..127
	var_127 = shape_variation / 6;
	if (var_127 > 127)
		var_127 = 127;

	// uint16_t var_255 = shape_variation/3;
	// if (var_255>255)
	// 	var_255 = 255;

	switch (shape_region) {
		case RAMPUP_EXP2LIN:
			*skew = RAMPUP;
			*next_curve_rise = var_127;
			*next_curve_fall = EXPO;
			break;

		case RAMPUP2SYM_LIN2EXP:
			*skew = var_127;
			*next_curve_rise = 127 - var_127;
			*next_curve_fall = EXPO;
			break;

		case SYM_EXP2LOG:
			*skew = SYM;
			*next_curve_rise = var_127 * 2;
			*next_curve_fall = var_127 * 2;
			break;

		case SYM2RAMPDOWN_LOG2LIN:
			*skew = var_127 + 128;
			*next_curve_rise = 255 - var_127 * 2;
			*next_curve_fall = 255 - var_127;
			break;

		default:
		case RAMPDOWN_EXP2LIN:
			*skew = RAMPDOWN;
			*next_curve_rise = EXPO;
			*next_curve_fall = 127 - var_127;
			break;
	}
}

//phase: 0..4095
//cur_curve: 0..255, curve to use: 0=expo, 127/128=linear, 255=log (interpolates)
//returns: 0..4095 dac value
int16_t calc_curve(int16_t phase, char cur_curve) {
	if (phase > 4095)
		phase = 4095;

	if (cur_curve == 127 || cur_curve == 128)
		return phase;

	else if (cur_curve < 127) {
		uint16_t t_inv_loga = 4095 - log4096[4095 - phase];
		return (phase * cur_curve + t_inv_loga * (127 - cur_curve)) >> 7;
	}

	else
	{ //cur_curve>128
		uint16_t t_loga = log4096[phase];
		cur_curve -= 128;
		return (phase * (127 - cur_curve) + t_loga * cur_curve) >> 7;
	}
}
