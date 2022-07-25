#include "stm32xx.h"
extern "C" {
#include "adc.h"
#include "analog_conditioning.h"
#include "calibration.h"
#include "debounced_digins.h"
#include "dig_inouts.h"
#include "env_update.h"
#include "envelope_calcs.h"
#include "flash.h"
#include "flash_user.hh"
#include "leds.h"
#include "pwm.h"
#include "system.h"
#include "system_mode.h"
#include "trigout.h"
}
#include "dac.h"

#include "hardware_test_adc.h"
#include "hardware_test_dac.h"
#include "hardware_test_gates.h"
#include "hardware_test_leds_buttons.h"
#include "hardware_test_util.h"

static void animate_success();

extern "C" void test_hardware(void) {
	all_lights_off();

	test_leds();
	test_buttons();
	test_dac();
	test_adc();
	test_gate_ins();
	test_gate_out();
	uint32_t repeats = 10;
	while (repeats--) {
		animate_success();
		HAL_Delay(100);
	}
}

static void animate_success() {
	static uint8_t led = 0;

	set_led(led, false);
	led++;
	if (led > PWM_EOF_LED)
		led = 0;
	set_led(led, true);
}
