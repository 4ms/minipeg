#include "globals.h"

extern struct SystemSettings settings;
extern analog_t analog[NUM_ADCS];
extern uint16_t adc_cv_dma_buffer[NUM_CV_ADCS];
extern uint16_t adc_pot_dma_buffer[NUM_POT_ADCS];

//Private:
#define FIRST_CD_POT POT_SCALE

enum CalRequests { CAL_REQUEST_NONE, CAL_REQUEST_ALL, CAL_REQUEST_CENTER_DET, CAL_REQUEST_LEDS };

uint8_t sanity_check_calibration(void);
void calibrate_divmult_pot(void);
enum CalRequests should_enter_calibration_mode(void);
void calibrate_center_detents(void);
void calibrate_led_colors(void);

void error_writing_settings(void) {
	int loops = 20;
	while (loops--) {
		set_rgb_led(LED_PING, c_WHITE);
		set_rgb_led(LED_CYCLE, c_WHITE);
		HAL_Delay(100);
		set_rgb_led(LED_PING, c_RED);
		set_rgb_led(LED_CYCLE, c_RED);
		HAL_Delay(100);
	}
}

void check_calibration(void) {
	if (!sanity_check_calibration()) {
		default_settings();
		if (write_settings() != HAL_OK)
			error_writing_settings();
	}

	enum CalRequests c = should_enter_calibration_mode();
	if (c != CAL_REQUEST_NONE) {
		if (c == CAL_REQUEST_ALL)
			calibrate_divmult_pot();

		if (c == CAL_REQUEST_ALL || c == CAL_REQUEST_CENTER_DET)
			calibrate_center_detents();

		if (c == CAL_REQUEST_ALL || c == CAL_REQUEST_LEDS)
			calibrate_led_colors();

		if (write_settings() != HAL_OK)
			error_writing_settings();
	}
}

void default_calibration(void) {
	settings.midpt_array[0] = 68;
	settings.midpt_array[1] = 262;
	settings.midpt_array[2] = 509;
	settings.midpt_array[3] = 743;
	settings.midpt_array[4] = 973;
	settings.midpt_array[5] = 1202;
	settings.midpt_array[6] = 1427;
	settings.midpt_array[7] = 1657;
	settings.midpt_array[8] = 1882;
	settings.midpt_array[9] = 2107;
	settings.midpt_array[10] = 2341;
	settings.midpt_array[11] = 2574;
	settings.midpt_array[12] = 2802;
	settings.midpt_array[13] = 3026;
	settings.midpt_array[14] = 3262;
	settings.midpt_array[15] = 3500;
	settings.midpt_array[16] = 3734;
	settings.midpt_array[17] = 3942;
	settings.midpt_array[18] = 4095;

	settings.center_detent_offset[DET_SCALE] = 0;
	settings.center_detent_offset[DET_OFFSET] = 0;
	settings.center_detent_offset[DET_SHAPE] = 0;

	settings.ping_cal_r = 2048;
	settings.ping_cal_g = 2048;
	settings.ping_cal_b = 2048;
	settings.cycle_cal_r = 2048;
	settings.cycle_cal_g = 2048;
	settings.cycle_cal_b = 2048;
	settings.lock_cal_r = 2048;
	settings.lock_cal_g = 2048;
	settings.lock_cal_b = 2048;
	settings.enva_cal_r = 2048;
	settings.enva_cal_g = 2048;
	settings.enva_cal_b = 2048;
	settings.envb_cal_r = 2048;
	settings.envb_cal_g = 2048;
	settings.envb_cal_b = 2048;
}

uint8_t sanity_check_calibration(void) {
	uint8_t j;

	for (j = 0; j < (NUM_DIVMULTS - 1); j++) {
		if (settings.midpt_array[j + 1] <= settings.midpt_array[j])
			return 0; //fail
	}
	for (j = 0; j < NUM_CENTER_DETENT_POTS; j++) {
		if ((settings.center_detent_offset[j] < -1000) || (settings.center_detent_offset[j] > 1000))
			return 0;
	}

	if (settings.limit_skew > 1)
		return 0;
	if (settings.free_running_ping > 1)
		return 0;
	if (settings.trigout_is_trig > 1)
		return 0;
	if (settings.trigin_function >= NUM_TRIGIN_FUNCTIONS)
		return 0;
	if (settings.trigout_function >= NUM_TRIGOUT_FUNCTIONS)
		return 0;
	if (settings.cycle_jack_behavior >= NUM_CYCLEJACK_BEHAVIORS)
		return 0;
	if ((settings.start_clk_time > 0x8000000) || (settings.start_clk_time < 100))
		return 0;
	if (settings.start_cycle_on > 1)
		return 0;

	if (settings.ping_cal_r < 1 || settings.ping_cal_r > 4096)
		return 0;
	if (settings.ping_cal_b < 1 || settings.ping_cal_b > 4096)
		return 0;
	if (settings.cycle_cal_r < 1 || settings.cycle_cal_r > 4096)
		return 0;
	if (settings.cycle_cal_g < 1 || settings.cycle_cal_g > 4096)
		return 0;

	return 1; //pass
}

enum CalRequests should_enter_calibration_mode(void) {
	if (CYCLEBUT && (adc_pot_dma_buffer[ADC_POT_SCALE] > 1800) && (adc_pot_dma_buffer[ADC_POT_SCALE] < 2200) &&
		(adc_pot_dma_buffer[ADC_POT_OFFSET] > 1800) && (adc_pot_dma_buffer[ADC_POT_OFFSET] < 2200) &&
		(adc_pot_dma_buffer[ADC_POT_SHAPE] > 1800) && (adc_pot_dma_buffer[ADC_POT_SHAPE] < 2200))
	{
		if (adc_pot_dma_buffer[ADC_POT_DIVMULT] < 70)
			return CAL_REQUEST_ALL;

		else if ((adc_pot_dma_buffer[ADC_POT_DIVMULT] > 1800) && (adc_pot_dma_buffer[ADC_POT_DIVMULT] < 2200))
			return CAL_REQUEST_LEDS;

		else if (adc_pot_dma_buffer[ADC_POT_DIVMULT] > 4000)
			return CAL_REQUEST_CENTER_DET;
	}

	return CAL_REQUEST_NONE;
}

void calibrate_center_detents(void) {
	const uint16_t stab_delay = 15;
	enum CenterDetentPots cur = DET_SCALE;
	uint16_t read_tot;
	uint16_t read_avg;
	uint16_t read1, read2, read3, read4;
	int16_t t;
	enum Palette color;

	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);

	while (PINGBUT) {
		;
	}
	while (CYCLEBUT) {
		;
	}

	HAL_Delay(100);
	set_rgb_led(LED_PING, c_WHITE);
	set_rgb_led(LED_CYCLE, c_WHITE);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_WHITE);
	set_rgb_led(LED_CYCLE, c_WHITE);
	HAL_Delay(100);
	set_rgb_led(LED_PING, c_OFF);
	set_rgb_led(LED_CYCLE, c_OFF);

	while (PINGBUT) {
		;
	}
	while (CYCLEBUT) {
		;
	}

	while (cur < NUM_CENTER_DETENT_POTS) {
		HAL_Delay(stab_delay);
		read1 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read2 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read3 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		HAL_Delay(stab_delay);
		read4 = adc_pot_dma_buffer[FIRST_CD_POT + cur];
		read_tot = read1 + read2 + read3 + read4;
		read_avg = read_tot >> 2;

		t = read_avg + settings.center_detent_offset[cur];
		if (t > 2152 || t < 1957)
			color = c_RED; //red: out of range
		else if (t > 2102 || t < 2007)
			color = c_ORANGE; //orange: warning, close to edge
		else if (t > 2068 || t < 2028)
			color = c_YELLOW; //yellow: ok: more than 20 from center
		else
			color = c_GREEN; //green: within 20 of center

		set_rgb_led(LED_CYCLE, color);

		if (CYCLEBUT) {
			settings.center_detent_offset[cur] = 2048 - read_avg;
			t = 0;
			while (t < 100) {
				if (CYCLEBUT)
					t++;
				else
					t = 0;
			}
			t = 0;
			while (t < 100) {
				if (!CYCLEBUT)
					t++;
				else
					t = 0;
			}
		}

		if (PINGBUT) {
			t = 0;
			while (t < 100) {
				if (PINGBUT)
					t++;
				else
					t = 0;
			}
			t = 0;
			while (t < 100) {
				if (!PINGBUT)
					t++;
				else
					t = 0;
			}

			cur++;
		}
	}
}

void calibrate_divmult_pot(void) {
	const uint16_t stab_delay = 15;
	uint16_t calib_array[NUM_DIVMULTS];
	uint16_t read_tot;
	uint16_t read_avg;
	uint16_t read1, read2, read3, read4;
	uint8_t j;
	uint16_t t;
	uint16_t diff;

	set_rgb_led(LED_PING, c_OFF);

	set_rgb_led(LED_CYCLE, c_RED);
	t = 0;
	while (t < 100) {
		if (CYCLEBUT)
			t++;
		else
			t = 0;
	}
	t = 0;
	while (t < 100) {
		if (!CYCLEBUT)
			t++;
		else
			t = 0;
	}
	set_rgb_led(LED_CYCLE, c_OFF);

	for (j = 0; j < NUM_DIVMULTS; j++) {
		set_rgb_led(LED_CYCLE, c_OFF); //off = reading pot

		HAL_Delay(stab_delay);
		read1 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read2 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read3 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
		HAL_Delay(stab_delay);
		read4 = adc_pot_dma_buffer[ADC_POT_DIVMULT];

		read_tot = read1 + read2 + read3 + read4;
		read_avg = read_tot >> 2;

		calib_array[j] = read_avg;

		if (j < (NUM_DIVMULTS - 1)) {
			set_rgb_led(LED_CYCLE, c_GREEN); //blue = ready for user to change knob

			if (j == 0 || j == (NUM_DIVMULTS - 2))
				diff = 80;
			else
				diff = 160;

			HAL_Delay(20);

			//wait until knob is detected as being moved
			do {
				HAL_Delay(stab_delay);
				read1 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read2 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read3 = adc_pot_dma_buffer[ADC_POT_DIVMULT];
				HAL_Delay(stab_delay);
				read4 = adc_pot_dma_buffer[ADC_POT_DIVMULT];

				read_tot = read1 + read2 + read3 + read4;
				read_avg = read_tot >> 2;
			} while ((read_avg - calib_array[j]) < diff);

			set_rgb_led(LED_CYCLE, c_WHITE); //green = press cycle button

			t = 0;
			while (t < 100) {
				if (CYCLEBUT)
					t++;
				else
					t = 0;
			}
			t = 0;
			while (t < 100) {
				if (!CYCLEBUT)
					t++;
				else
					t = 0;
			}
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}

	//convert the calib_array values to mid-points
	for (j = 0; j < (NUM_DIVMULTS - 1); j++) {
		settings.midpt_array[j] = (calib_array[j] + calib_array[j + 1]) >> 1;
	}
	settings.midpt_array[NUM_DIVMULTS - 1] = 4095;
}

void calibrate_led_colors(void) {

	set_rgb_led(LED_CYCLE, c_OFF);

	while (!PINGBUT) {
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_SCALE]), PWM_PINGBUT_R);
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_SHAPE]), PWM_PINGBUT_G);
		update_pwm(adjust_hue(2048, adc_pot_dma_buffer[ADC_POT_OFFSET]), PWM_PINGBUT_B);
	}
	settings.ping_cal_r = adc_pot_dma_buffer[ADC_POT_SCALE];
	settings.ping_cal_g = adc_pot_dma_buffer[ADC_POT_SHAPE];
	settings.ping_cal_b = adc_pot_dma_buffer[ADC_POT_OFFSET];

	HAL_Delay(100);
	while (PINGBUT) {
		;
	}
	HAL_Delay(100);

	uint16_t r;
	uint16_t g;
	uint16_t b;

	while (!PINGBUT) {
		if (!CYCLEBUT) {
			r = 4095;
			g = 600;
			b = 0;
		} else {
			r = 2048;
			g = 2048;
			b = 2048;
		}
		update_pwm(adjust_hue(r, adc_pot_dma_buffer[ADC_POT_SCALE]), PWM_CYCLEBUT_R);
		update_pwm(adjust_hue(g, adc_pot_dma_buffer[ADC_POT_SHAPE]), PWM_CYCLEBUT_G);
		update_pwm(adjust_hue(b, adc_pot_dma_buffer[ADC_POT_OFFSET]), PWM_CYCLEBUT_B);
	}
	settings.cycle_cal_r = adc_pot_dma_buffer[ADC_POT_SCALE];
	settings.cycle_cal_g = adc_pot_dma_buffer[ADC_POT_SHAPE];
	settings.cycle_cal_b = adc_pot_dma_buffer[ADC_POT_OFFSET];

	all_lights_off();
}
