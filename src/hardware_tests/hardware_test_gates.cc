#include "dac.h"
#include "dig_inouts.h"
#include "leds.h"
#include "pwm.h"

#include "GateInChecker.h"
#include "GateOutput.h"
#include "hardware_test_util.h"

//Test:
//Patch ENVA to TRIGA, TRIGB, and PING jacks in that order
//Respective lights will flash green as test is happening (flashes alternating magenta/white)
//
//Error Codes:
//CYCLE green: multiple jacks found high at the same time
//CYCLE flashing red after test is done: Some type of error detected
//
class SEGGateInChecker : public IGateInChecker {
	const unsigned kNumRepeats = 100;

public:
	SEGGateInChecker()
		: IGateInChecker(3) {
		pause_dac_timer();
		init_dac(0, [] {});
		set_num_toggles(kNumRepeats);
	}

protected:
	void set_test_signal(bool newstate) override {
		dac_out(DAC_ENVA, newstate ? 2870 : 2048);

		//set_led(PWM_ENVA_B, newstate);
		HAL_Delay(1); //allow for latency of DAC output
	}

	bool read_gate(uint8_t gate_num) override {
		if (gate_num == 0)
			return TRIG_JACK_READ ? true : false;

		else if (gate_num == 1)
			return AUXTRIG_JACK_READ ? true : false;

		else if (gate_num == 2)
			return PING_JACK_READ ? true : false;
		else
			return false;
	}

	void set_indicator(uint8_t indicator_num, bool newstate) override {
		if (indicator_num == 0)
			set_led(PWM_ENVA_G, newstate);

		else if (indicator_num == 1)
			set_led(PWM_ENVB_G, newstate);

		else if (indicator_num == 2)
			set_led(PWM_PINGBUT_G, newstate);
	}

	void set_error_indicator(uint8_t channel, ErrorType err) override {
		if (err == ErrorType::None) {
			set_led(PWM_CYCLEBUT_G, false);
			set_led(PWM_ENVA_R, false);
			set_led(PWM_ENVA_B, false);
			set_led(PWM_ENVB_R, false);
			set_led(PWM_ENVB_B, false);
			set_led(PWM_PINGBUT_R, false);
			set_led(PWM_PINGBUT_B, false);
		}

		if (err == ErrorType::MultipleHighs)
			set_led(PWM_CYCLEBUT_G, true);

		if (channel == 0) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_ENVA_R, true);

			if (err == ErrorType::StuckLow)
				set_led(PWM_ENVA_B, true);
		}

		if (channel == 1) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_ENVB_R, true);

			if (err == ErrorType::StuckLow)
				set_led(PWM_ENVB_B, true);
		}

		if (channel == 2) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_PINGBUT_R, true);

			if (err == ErrorType::StuckLow)
				set_led(PWM_PINGBUT_B, true);
		}
	}

	void signal_jack_done(uint8_t chan) override {
		if (chan == 0)
			set_led(PWM_ENVA_B, true);

		else if (chan == 1)
			set_led(PWM_ENVB_B, true);

		else if (chan == 2)
			set_led(PWM_PINGBUT_B, true);
	}

	bool is_ready_to_read_jack(uint8_t chan) override {
		return true;
	}
};

void test_gate_ins() {
	all_lights_off();

	SEGGateInChecker checker;

	checker.reset();

	while (checker.check()) {
		if (hardwaretest_continue_button()) {
			set_led(PWM_CYCLEBUT_G, true);
			pause_until_button_released();
			checker.skip();
			set_led(PWM_CYCLEBUT_G, false);
			HAL_Delay(50);
		}
	}

	if (checker.get_error() != SEGGateInChecker::ErrorType::None) {
		while (!hardwaretest_continue_button()) {
			set_led(PWM_CYCLEBUT_R, true);
			HAL_Delay(100);
			set_led(PWM_CYCLEBUT_R, false);
			HAL_Delay(100);
		}
	}

	all_lights_off();
	pause_until_button_released();
}

static void set_trigout(bool state) {
	if (state) {
		TRIGOUT_ON;
		DEBUGON;
		set_led(PWM_EOF_LED, true);
	} else {
		TRIGOUT_OFF;
		DEBUGOFF;
		set_led(PWM_EOF_LED, false);
	}
}

void test_gate_out() {
	pause_until_button_released();

	set_trigout(false);
	while (!hardwaretest_continue_button()) {
		HAL_Delay(300);
		set_trigout(true);
		if (hardwaretest_continue_button())
			continue;
		HAL_Delay(150);
		set_trigout(false);
	}
}
