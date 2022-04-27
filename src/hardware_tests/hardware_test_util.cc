#include "dig_inouts.h"
#include "leds.h"
#include "pwm.h"

#include "hardware_test_util.h"
#include <stm32g4xx.h>

bool hardwaretest_continue_button(void) {
	return PINGBUT ? true : false;
}

void pause_until_button_pressed(void) {
	HAL_Delay(80);
	while (!hardwaretest_continue_button()) {
		;
	}
}

void pause_until_button_released(void) {
	HAL_Delay(80);
	while (hardwaretest_continue_button()) {
		;
	}
}

void set_led(uint8_t led_num, bool newstate) {
	uint16_t brightness = newstate ? max_pwm : min_pwm;

	if (led_num == PWM_EOF_LED)
		set_inverted_led(PWM_EOF_LED, brightness);
	else if (led_num < NUM_PWMS)
		update_pwm(brightness, static_cast<PwmOutputs>(led_num));
}

bool read_button(uint8_t button_num) {
	if (button_num == 0)
		return PINGBUT ? true : false;
	else if (button_num == 1)
		return CYCLEBUT ? true : false;
	else
		return false;
}

void set_button_led(uint8_t button_num, bool turn_on) {
	uint16_t brightness = turn_on ? max_pwm : min_pwm;

	if (button_num == 0) {
		update_pwm(brightness, PWM_PINGBUT_R);
		update_pwm(brightness, PWM_PINGBUT_G);
		update_pwm(brightness, PWM_PINGBUT_B);
	} else if (button_num == 1) {
		update_pwm(brightness, PWM_CYCLEBUT_R);
		update_pwm(brightness, PWM_CYCLEBUT_G);
		update_pwm(brightness, PWM_CYCLEBUT_B);
	} else if (button_num == 2) {
		update_pwm(brightness, PWM_LOCKBUT_R);
		update_pwm(brightness, PWM_LOCKBUT_G);
		update_pwm(brightness, PWM_LOCKBUT_B);
	}
}

void flash_ping_until_pressed(void) {
	while (1) {
		set_button_led(0, true);
		HAL_Delay(200);
		if (hardwaretest_continue_button())
			break;
		set_button_led(0, false);
		HAL_Delay(200);
	}
	set_button_led(0, true);
	pause_until_button_released();
	set_button_led(0, false);
}
