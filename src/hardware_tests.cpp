#include <stm32g4xx.h>
extern "C" {
#include "adc.h"
#include "calibration.h"
#include "delay.h"
#include "dig_inouts.h"
#include "envelope_calcs.h"
#include "envelope_out.h"
#include "flash_user.h"
#include "leds.h"
#include "pwm.h"
#include "trigout.h"
#include "debounced_digins.h"
#include "analog_conditioning.h"
#include "system_mode.h"
#include "flash.h"
#include "system.h"
}
#include "dac.h"
#include "libhwtests/inc/LEDTester.h"
#include "libhwtests/inc/LEDButtonChecker.h"
#include "libhwtests/inc/AdcRangeChecker.hh"

extern uint16_t adc_cv_dma_buffer[NUM_CV_ADCS];
extern uint16_t adc_pot_dma_buffer[NUM_POT_ADCS];

const uint16_t max_pwm = 4095;
const uint16_t min_pwm = 0;

static void set_led(uint8_t led_num, bool newstate) {
	uint16_t brightness = newstate ? max_pwm : min_pwm;

	if (led_num<PWM_EOF_LED) {
		update_pwm(brightness, static_cast<PwmOutputs>(led_num));
	} else if (led_num==PWM_EOF_LED) {
		set_inverted_led(PWM_EOF_LED, brightness);
	}
}

static bool read_button(uint8_t button_num) {
	if (button_num == 0)
		return PINGBUT ? true : false;
	else if (button_num == 1)
		return CYCLEBUT ? true : false;
	else if (button_num == 2)
		return LOCKBUT ? true : false;
	else
		return false;
}


static void set_button_led(uint8_t button_num, bool turn_on) {
	uint16_t brightness = turn_on ? max_pwm : min_pwm;

	if (button_num == 0) {
		update_pwm(brightness, PWM_PINGBUT_R);
		update_pwm(brightness, PWM_PINGBUT_G);
		update_pwm(brightness, PWM_PINGBUT_B);
	}
	else if (button_num == 1) {
		update_pwm(brightness, PWM_CYCLEBUT_R);
		update_pwm(brightness, PWM_CYCLEBUT_G);
		update_pwm(brightness, PWM_CYCLEBUT_B);
	}
	else if (button_num == 2) {
		update_pwm(brightness, PWM_LOCKBUT_R);
		update_pwm(brightness, PWM_LOCKBUT_G);
		update_pwm(brightness, PWM_LOCKBUT_B);
	}
}

static void test_leds();
static void test_buttons();
static void test_dac();
static void test_adc();

extern "C" void test_hardware(void) {
    all_lights_off();

	//test_leds();
	test_buttons();
	test_dac();
	test_adc();
	while (1) {
		;
	}
}

static void test_leds() {
	LEDTester led_checker{NUM_PWMS};
	led_checker.assign_led_onoff_func(set_led);
	led_checker.reset();
	while (!led_checker.is_done()) {
		while(PINGBUT) {;}
		led_checker.next_led();
		while(!PINGBUT) {;}
	}

	all_lights_off();
    while(PINGBUT) {;}
}

static void test_buttons() {
	LEDButtonChecker button_checker{3};
	button_checker.assign_button_led_func(set_button_led);
	button_checker.assign_button_read_func(read_button);
	button_checker.reset();
	while (!button_checker.check_done()) {
		button_checker.run_check();
	}
}

const uint16_t sinewave[60] = {
0x07ff,0x08cb,0x0994,0x0a5a,0x0b18,0x0bce,0x0c79,0x0d18,0x0da8,0x0e29,0x0e98,0x0ef4,0x0f3e,0x0f72,0x0f92,0x0f9d,
0x0f92,0x0f72,0x0f3e,0x0ef4,0x0e98,0x0e29,0x0da8,0x0d18,0x0c79,0x0bce,0x0b18,0x0a5a,0x0994,0x08cb,0x07ff,0x0733,
0x066a,0x05a4,0x04e6,0x0430,0x0385,0x02e6,0x0256,0x01d5,0x0166,0x010a,0x00c0,0x008c,0x006c,0x0061,0x006c,0x008c,
0x00c0,0x010a,0x0166,0x01d5,0x0256,0x02e6,0x0385,0x0430,0x04e6,0x05a4,0x066a,0x0733};

static void update_test_waves() {
	static uint32_t ctr=0;
	static uint32_t sinectr=0;

	if (++ctr >= 0xFFF) 
		ctr=0;
	if ((ctr & 0x2) == 0) {
		if (++sinectr >= 59) sinectr=0;
		dac_out(DAC_ENVB, sinewave[sinectr]);
	}
	dac_out(DAC_ENVA, sinewave[ctr % 60]/2 + (0xFFF/4));
}

static void test_dac() {
	init_dac();
	assign_dac_update_callback(&update_test_waves);
}

static void test_adc() {
	AdcRangeCheckerBounds init {
		.center_val = 2048,
		.center_width = 100,
		.center_check_counts = 4,
		.min_val = 10,
		.max_val = 4086
	};

	AdcRangeChecker adc_checker{init};

	uint8_t adc_map[NUM_ADCS] = {
		ADC_POT_DIVMULT,
		ADC_POT_SHAPE,
		ADC_POT_SCALE,
		ADC_POT_OFFSET,
		ADC_CV_SHAPE,
		ADC_CV_DIVMULT,
	};

	for (uint32_t adc_i=0; adc_i<NUM_ADCS; adc_i++) {
		bool done = false;
		uint8_t cur_adc = adc_map[adc_i];
		adc_checker.reset();

		set_led(PWM_EOF_LED, true);
		set_led(PWM_ENVA_R, true);
		set_led(PWM_ENVB_R, true);

		while (!done) {
			uint16_t adcval = (adc_i<NUM_POT_ADCS) ?
								adc_pot_dma_buffer[cur_adc]
								: adc_cv_dma_buffer[cur_adc];
			adc_checker.set_adcval(adcval);
			auto status = adc_checker.check();


			if (adc_i>=NUM_POT_ADCS) {
				// zeroes_ok = check_max_one_cv_is_nonzero(300);
				// if (!zeroes_ok) {
				// 	show_multiple_nonzeros_error();
				// 	adc_checker.reset();
				// 	set_led(PWM_EOF_LED, true);
				// 	set_led(PWM_ENVA_R, true);
				// 	set_led(PWM_ENVB_R, true);
				// }
			}

			if (status==ADCCHECK_AT_MIN){
				set_led(PWM_EOF_LED, false);
			}
			else if (status==ADCCHECK_AT_MAX){
				set_led(PWM_ENVB_R, false);
			}
			else if (status==ADCCHECK_AT_CENTER){
				set_led(PWM_ENVA_R, false);
			}
			else if (status==ADCCHECK_ELSEWHERE){
				set_led(PWM_ENVA_R, true);
			}
			else if (status==ADCCHECK_FULLY_COVERED){
				done = true;
			}

			if (PINGBUT)
				done = true;
		}
		update_pwm(max_pwm, PWM_CYCLEBUT_G);
		update_pwm(max_pwm, PWM_LOCKBUT_G);
		HAL_Delay(350);
		update_pwm(min_pwm, PWM_CYCLEBUT_G);
		update_pwm(min_pwm, PWM_LOCKBUT_G);
	}
}


void other_tests() {
    uint16_t adcval;
    while (!PINGBUT) {
        update_pwm(PING_JACK_READ ? max_pwm : min_pwm, PWM_PINGBUT_R);
        update_pwm(CYCLE_JACK_READ ? max_pwm : min_pwm, PWM_CYCLEBUT_R);
        set_inverted_led(PWM_EOF_LED, TRIG_JACK_READ ? max_pwm : min_pwm);

        adcval = adc_cv_dma_buffer[CV_SHAPE] / (4095/max_pwm);
        update_pwm(adcval, PWM_ENVA_R);
        update_pwm(max_pwm - adcval, PWM_ENVA_B);

        adcval = adc_cv_dma_buffer[CV_DIVMULT] / (4095/max_pwm);
        update_pwm(adcval, PWM_ENVB_R);
        update_pwm(max_pwm - adcval, PWM_ENVB_B);
    }

    while(PINGBUT) {;}
    all_lights_off();

    while (!PINGBUT) {
        adcval = adc_pot_dma_buffer[POT_SHAPE];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_ENVA_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_ENVA_B);

        adcval = adc_pot_dma_buffer[POT_DIVMULT];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_PINGBUT_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_PINGBUT_B);

        adcval = adc_pot_dma_buffer[POT_OFFSET];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_ENVB_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_ENVB_B);

        adcval = adc_pot_dma_buffer[POT_SCALE];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_CYCLEBUT_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_CYCLEBUT_G);
    }

    while(PINGBUT) {;}

    all_lights_off();

    int16_t i=-max_pwm, i5=-max_pwm;
    uint16_t tri, tri5;
    while (!PINGBUT) {
        if (i++>=max_pwm) i = -max_pwm;
        tri = (i<0) ? -i : i;
 //       dac_out(tri, DAC_ENVA);
        update_pwm(tri, PWM_ENVA_B); 

        if (i&1 && i5++>=max_pwm) i5= -max_pwm;
        tri5 = (i5<0) ? -i5 : i5;
//        dac_out(tri5, DAC_ENVB);
        update_pwm(tri5, PWM_ENVB_B);

        delay_ticks(2);
    }

    while(PINGBUT) {;}

    all_lights_off();
}

uint8_t hardwaretest_continue_button(void) {
	return PINGBUT;
}

void pause_until_button_pressed(void) {
	HAL_Delay(80);
	while (!hardwaretest_continue_button()) {;}
}

void pause_until_button_released(void) {
	HAL_Delay(80);
	while (hardwaretest_continue_button()) {;}
}

// void flash_ping_until_pressed(void) {
// 	while (1) {
// 		LED_PINGBUT_OFF;
// 		delay_ms(200);
// 		if (hardwaretest_continue_button()) break;
// 		LED_PINGBUT_ON;
// 		delay_ms(200);
// 	}
// 	LED_PINGBUT_ON;
// 	pause_until_button_released();
// 	LED_PINGBUT_OFF;
// }
