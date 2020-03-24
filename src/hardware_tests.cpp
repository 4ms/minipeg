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

extern "C" void test_hardware(void) {
    all_lights_off();

	test_leds();
	test_buttons();

	while (1) {
		;
	}
}

static void test_leds() {
	LEDTester test{NUM_PWMS};
	test.assign_led_onoff_func(set_led);
	test.reset();
	while (!test.is_done()) {
		while(PINGBUT) {;}
		test.next_led();
		while(!PINGBUT) {;}
	}

	all_lights_off();
    while(PINGBUT) {;}
}

static void test_buttons() {
	LEDButtonChecker but_test{3};
	but_test.assign_button_led_func(set_button_led);
	but_test.assign_button_read_func(read_button);
	but_test.reset();
	while (!but_test.check_done()) {
		but_test.run_check();
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
