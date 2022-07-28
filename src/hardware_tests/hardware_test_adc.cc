#include "adc.h"
#include "analog_conditioning.h"
#include "pwm.h"

#include "AdcRangeChecker.hh"

#include "hardware_test_util.h"

extern uint16_t adc_dma_buffer[NUM_ADCS];
extern uint16_t adc_cv_dma_buffer[NUM_CV_ADCS];
extern uint16_t adc_pot_dma_buffer[NUM_POT_ADCS];

void test_adc() {
	AdcRangeCheckerBounds init{
		.center_val = 2048,
		.center_width = 100,
		.center_check_counts = 400,
		.min_val = 10,
		.max_val = 4086,
	};

	AdcRangeChecker adc_checker{init};

	uint8_t adc_map[NUM_ADCS] = {
		POT_DIVMULT,
		POT_SHAPE,
		POT_SCALE,
		POT_OFFSET,
		CV_SHAPE,
		CV_DIVMULT,
	};

	pause_until_button_released();

	for (uint32_t adc_i = 0; adc_i < NUM_ADCS; adc_i++) {
		bool done = false;
		uint8_t cur_adc = adc_map[adc_i];
		adc_checker.reset();

		set_led(PWM_EOF_LED, true);
		set_led(PWM_ENVA_R, true);
		set_led(PWM_ENVB_R, true);

		while (!done) {
			// uint16_t adcval = (adc_i < NUM_POT_ADCS) ? adc_pot_dma_buffer[cur_adc] : adc_cv_dma_buffer[cur_adc];
			uint16_t adcval = adc_dma_buffer[cur_adc];
			adc_checker.set_adcval(adcval);
			auto status = adc_checker.check();

			if (adc_i >= NUM_POT_ADCS) {
				// zeroes_ok = check_max_one_cv_is_nonzero(300);
				// if (!zeroes_ok) {
				// 	show_multiple_nonzeros_error();
				// 	adc_checker.reset();
				// 	set_led(PWM_EOF_LED, true);
				// 	set_led(PWM_ENVA_R, true);
				// 	set_led(PWM_ENVB_R, true);
				// }
			}

			if (status == ADCCHECK_AT_MIN) {
				set_led(PWM_EOF_LED, false);
			} else if (status == ADCCHECK_AT_MAX) {
				set_led(PWM_ENVB_R, false);
			} else if (status == ADCCHECK_AT_CENTER) {
				set_led(PWM_ENVA_R, false);
			} else if (status == ADCCHECK_ELSEWHERE) {
				set_led(PWM_ENVA_R, true);
			} else if (status == ADCCHECK_FULLY_COVERED) {
				done = true;
			}

			if (hardwaretest_continue_button())
				done = true;
		}
		update_pwm(max_pwm, PWM_CYCLEBUT_G);
		//update_pwm(max_pwm, PWM_LOCKBUT_G);
		HAL_Delay(200);
		update_pwm(min_pwm, PWM_CYCLEBUT_G);
		//update_pwm(min_pwm, PWM_LOCKBUT_G);
	}
}
