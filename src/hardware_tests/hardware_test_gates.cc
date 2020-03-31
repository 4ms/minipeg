#include "dig_inouts.h"
#include "pwm.h"
#include "leds.h"

#include "hardware_test_util.h"
#include "../libhwtests/inc/GateInChecker.h"
#include "../libhwtests/inc/GateOutput.h"

static bool read_jack(uint8_t gate_num) {
	if (gate_num==0)
		return PING_JACK_READ ? true : false;
	else if (gate_num==1)
		return CYCLE_JACK_READ ? true : false;
	else if (gate_num==2)
		return TRIG_JACK_READ ? true : false;
	else
		return false;
}

void test_gate_ins() {
	all_lights_off();

	GateInChecker checker{3};
	checker.assign_read_gate_func(read_jack);
	checker.assign_indicator_func(set_led);

	checker.reset();
	while (checker.check()) {
		if (checker.num_gates_high() > 1)
			set_led(PWM_PINGBUT_R, true);
	}

	all_lights_off();
	set_button_led(0, true);
	pause_until_button_pressed();
	pause_until_button_released();
	all_lights_off();
}

static void set_trigout(bool state) {
	if (state) {
		TRIGOUT_ON;
		DEBUGON;
		set_led(PWM_EOF_LED, true);
	}
	else {
		TRIGOUT_OFF;
		DEBUGOFF;
		set_led(PWM_EOF_LED, false);
	}
}

void test_gate_out() {
	GateOutput trigout{2, 0.25f, 0.0f, 2140000};

	trigout.assign_gate_onoff_func(set_trigout);
	trigout.reset();

	pause_until_button_released();

	while (!hardwaretest_continue_button()) {
		trigout.update();
		HAL_Delay(20);
	}

}
