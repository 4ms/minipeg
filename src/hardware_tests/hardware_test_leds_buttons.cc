#include "pwm.h"
#include "leds.h"

#include "hardware_test_leds_buttons.h"
#include "hardware_test_util.h"

#include "libhwtests/inc/LEDTester.h"
#include "libhwtests/inc/LEDButtonChecker.h"

void test_leds() {
	LEDTester led_checker{NUM_PWMS};
	led_checker.assign_led_onoff_func(set_led);
	led_checker.reset();
	while (!led_checker.is_done()) {
		while(hardwaretest_continue_button()) {;}
		led_checker.next_led();
		while(!hardwaretest_continue_button()) {;}
	}

	all_lights_off();
    while(hardwaretest_continue_button()) {;}
}

void test_buttons() {
	LEDButtonChecker button_checker{3};
	button_checker.assign_button_led_func(set_button_led);
	button_checker.assign_button_read_func(read_button);
	button_checker.reset();
	while (!button_checker.check_done()) {
		button_checker.run_check();
	}
}
