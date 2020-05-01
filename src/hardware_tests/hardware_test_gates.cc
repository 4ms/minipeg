#include "dig_inouts.h"
#include "pwm.h"
#include "leds.h"
#include "dac.h"

#include "hardware_test_util.h"
#include "GateInChecker.h"
#include "GateOutput.h"

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
	SEGGateInChecker() : IGateInChecker(3)
	{
		init_dac(0);
		pause_dac_timer();
		set_num_toggles(kNumRepeats);
	}

protected:
	virtual void _set_test_signal(bool newstate) {
		dac_out(DAC_ENVA, newstate ? 2870 : 2048);

		//set_led(PWM_ENVA_B, newstate);
		HAL_Delay(1); //allow for latency of DAC output
	}

	virtual bool _read_gate(uint8_t gate_num) {
		if (gate_num==0)
			return TRIG_JACK_READ ? true : false;

		else if (gate_num==1)
			return AUXTRIG_JACK_READ ? true : false;

		else if (gate_num==2)
			return PING_JACK_READ ? true : false;
		else
			return false;
	}

	virtual void _set_indicator(uint8_t indicator_num, bool newstate) {
		if (indicator_num==0)
			set_led(PWM_ENVA_G, !newstate);

		else if (indicator_num==1)
			set_led(PWM_ENVB_G, !newstate);

		else if (indicator_num==2)
			set_led(PWM_PINGBUT_G, !newstate);
	}

	virtual void _set_error_indicator(uint8_t channel, ErrorType err) {
		if (err == ErrorType::None) {
			set_led(PWM_CYCLEBUT_G, false);
			set_led(PWM_ENVA_R, true);
			set_led(PWM_ENVA_B, true);
			set_led(PWM_ENVB_R, true);
			set_led(PWM_ENVB_B, true);
			set_led(PWM_PINGBUT_R, true);
			set_led(PWM_PINGBUT_B, true);
		}

		if (err == ErrorType::MultipleHighs)
			set_led(PWM_CYCLEBUT_G, true);

		if (channel == 0) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_ENVA_R, false);

			if (err == ErrorType::StuckLow)
				set_led(PWM_ENVA_B, false);
		}

		if (channel == 1) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_ENVB_R, false);

			if (err == ErrorType::StuckLow)
				set_led(PWM_ENVB_B, false);
		}

		if (channel == 2) {
			if (err == ErrorType::StuckHigh)
				set_led(PWM_PINGBUT_R, false);

			if (err == ErrorType::StuckLow)
				set_led(PWM_PINGBUT_B, false);
		}
	}
};

void test_gate_ins() {
	all_lights_off();

	SEGGateInChecker checker;

	checker.reset();

	while (!checker.check()) {
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
	pause_until_button_released();

	set_trigout(false);
	while (!hardwaretest_continue_button()) {
		HAL_Delay(500);
		set_trigout(true);
		HAL_Delay(200);
		set_trigout(false);
	}
}
