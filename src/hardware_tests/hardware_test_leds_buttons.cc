#include "pwm.h"
#include "leds.h"

#include "hardware_test_leds_buttons.h"
#include "hardware_test_util.h"

#include "libhwtests/inc/LEDTester.h"
#include "libhwtests/inc/LEDButtonChecker.h"

void test_leds() {
	while(!hardwaretest_continue_button()) {
		set_led(0, true);
		HAL_Delay(500);
		set_led(0, false);
		HAL_Delay(500);
	}

	LEDTester led_checker{NUM_PWMS};
	led_checker.assign_led_onoff_func(set_led);
	led_checker.reset();
	while (!led_checker.is_done()) {
		while(hardwaretest_continue_button()) {;}
		HAL_Delay(200);
		led_checker.next_led();
		while(!hardwaretest_continue_button()) {;}
	}

	all_lights_off();
    while(hardwaretest_continue_button()) {;}
}

void test_buttons() {
	pause_until_button_released();

#ifdef LOCK_PCB
	LEDButtonChecker button_checker{3};
#else
	LEDButtonChecker button_checker{2};
#endif
	button_checker.assign_button_led_func(set_button_led);
	button_checker.assign_button_read_func(read_button);
	button_checker.reset();
	while (!button_checker.check_done()) {
		button_checker.run_check();
	}
}
