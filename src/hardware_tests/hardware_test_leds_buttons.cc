#include "globals.h"
#include "leds.h"
#include "pwm.h"

#include "hardware_test_leds_buttons.h"
#include "hardware_test_util.h"

#include "ButtonChecker.h"
#include "LEDTester.h"

void test_leds() {
	while (!hardwaretest_continue_button()) {
		set_led(PWM_ENVA_B, true);
		set_led(PWM_ENVA_G, false);
		HAL_Delay(200);
		set_led(PWM_ENVA_B, false);
		set_led(PWM_ENVA_G, true);
		HAL_Delay(200);
	}

	LEDTester led_checker{NUM_PWMS};
	led_checker.assign_led_onoff_func(set_led);
	led_checker.reset();
	while (!led_checker.is_done()) {
		while (hardwaretest_continue_button()) {
			;
		}
		HAL_Delay(100);
		led_checker.next_led();
		while (!hardwaretest_continue_button()) {
			;
		}
		HAL_Delay(100);
	}

	all_lights_off();
	while (hardwaretest_continue_button()) {
		;
	}
}

class ButtonChecker : public IButtonChecker {
public:
	ButtonChecker(uint8_t num_channels)
		: IButtonChecker(num_channels) {
	}

	bool _read_button(uint8_t channel) override {
		return read_button(channel);
	}
	void _set_error_indicator(uint8_t channel, ErrorType err) override {
		if (err != ErrorType::None) {
			set_rgb_led(LED_PING, c_RED);
			set_rgb_led(LED_CYCLE, c_RED);
		} else {
			set_rgb_led(LED_PING, c_OFF);
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}
	void _set_indicator(uint8_t indicator_num, bool newstate) override {
		set_button_led(indicator_num, newstate);
	}
	void _check_max_one_pin_changed() override {
	}
};

void test_buttons() {
	pause_until_button_released();

#ifdef LOCK_PCB
	ButtonChecker button_checker{3};
#else
	ButtonChecker button_checker{2};
#endif
	// button_checker.assign_button_led_func(set_button_led);
	// button_checker.assign_button_read_func(read_button);
	button_checker.reset();
	while (button_checker.check()) {
		;
	}
	// while (!button_checker.check_done()) {
	// 	button_checker.run_check();
	// }
}
