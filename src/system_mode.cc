#include "globals.h"
#include "stm32xx.h"

extern struct SystemSettings settings;
extern debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
extern uint8_t adjusting_shift_mode;

void handle_system_mode(void) {
	uint8_t d;
	enum SystemModeParams system_mode_cur;
	static uint32_t ping_held_time = 0;
	static uint32_t cycle_held_time = 0;
	uint32_t now = HAL_GetTick();

	if (digin[CYCLE_BUTTON].state == 0)
		cycle_held_time = now;

	if (adjusting_shift_mode)
		return;

	if ((now - cycle_held_time) <= (3000 * TICKS_PER_MS))
		return;

	for (d = 0; d < 5; d++) {
		set_rgb_led(LED_PING, c_WHITE);
		set_rgb_led(LED_CYCLE, c_WHITE);
		set_rgb_led(LED_ENVA, c_PURPLE);
		set_rgb_led(LED_ENVB, c_PURPLE);
		set_led_brightness(1024, PWM_EOF_LED);

		HAL_Delay(100);
		if (d == 4)
			HAL_Delay(500);

		set_rgb_led(LED_PING, c_OFF);
		set_rgb_led(LED_CYCLE, c_OFF);
		set_rgb_led(LED_ENVA, c_OFF);
		set_rgb_led(LED_ENVB, c_OFF);
		set_led_brightness(0, PWM_EOF_LED);

		HAL_Delay(100);
	}

	while (digin[CYCLE_BUTTON].state == 1) {
		;
	}
	HAL_Delay(50);

	ping_held_time = now;
	system_mode_cur = SET_TRIGOUT_FUNC;

	while (1) {
		now = HAL_GetTick();

		if (digin[PING_BUTTON].state == 0)
			ping_held_time = now;
		else if ((now - ping_held_time) > (3000 * TICKS_PER_MS))
			break;

		if (digin[CYCLE_BUTTON].state == 0)
			cycle_held_time = now;
		else if ((now - cycle_held_time) > (3000 * TICKS_PER_MS))
			break;

		if (digin[PING_BUTTON].edge == -1) {
			system_mode_cur = static_cast<SystemModeParams>(system_mode_cur + 1);
			digin[PING_BUTTON].edge = 0;
		}

		switch (system_mode_cur) {
			case (NUM_SYSMODE_PARAMS):
				system_mode_cur = SET_TRIGOUT_FUNC;
			case (SET_TRIGOUT_FUNC):
				set_led_brightness(1024, PWM_EOF_LED);
				if (settings.trigout_function == TRIGOUT_IS_ENDOFRISE)
					set_rgb_led(LED_CYCLE, c_RED);
				else if (settings.trigout_function == TRIGOUT_IS_ENDOFFALL)
					set_rgb_led(LED_CYCLE, c_ORANGE);
				else if (settings.trigout_function == TRIGOUT_IS_HALFRISE)
					set_rgb_led(LED_CYCLE, c_GREEN);
				else if (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)
					set_rgb_led(LED_CYCLE, c_WHITE);

				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_ENVA, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				break;

			case (SET_TRIGOUT_IS_TRIG):
				if (settings.trigout_is_trig) {
					set_rgb_led(LED_CYCLE, c_WHITE);
					if (now & 0x0F00)
						set_led_brightness(0, PWM_EOF_LED);
					else
						set_led_brightness(1024, PWM_EOF_LED);
				} else {
					set_rgb_led(LED_CYCLE, c_ORANGE);
					if (now & 0x1000)
						set_led_brightness(0, PWM_EOF_LED);
					else
						set_led_brightness(1024, PWM_EOF_LED);
				}
				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_ENVA, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				break;

			case (SET_LIMIT_SKEW):
				set_rgb_led(LED_ENVA, c_BLUE);
				if (settings.limit_skew)
					set_rgb_led(LED_CYCLE, c_WHITE);
				else
					set_rgb_led(LED_CYCLE, c_ORANGE);

				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				set_led_brightness(0, PWM_EOF_LED);
				break;

			case (SET_FREE_RUNNING_PING):
				if (settings.free_running_ping) {
					set_rgb_led(LED_PING, (now & 0x1000) ? c_WHITE : c_DIMBLUE);
					set_rgb_led(LED_CYCLE, c_ORANGE);
				} else {
					set_rgb_led(LED_PING, c_DIMBLUE);
					set_rgb_led(LED_CYCLE, c_WHITE);
				}
				set_rgb_led(LED_ENVA, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				set_led_brightness(0, PWM_EOF_LED);
				break;

			case (SET_TRIGIN_FUNCTION):
				set_led_brightness(1024, PWM_EOF_LED);
				set_rgb_led(LED_ENVA, c_RED);
				if (settings.trigin_function == TRIGIN_IS_QNT)
					set_rgb_led(LED_CYCLE, c_GREEN);
				else if (settings.trigin_function == TRIGIN_IS_ASYNC)
					set_rgb_led(LED_CYCLE, c_ORANGE);
				else if (settings.trigin_function == TRIGIN_IS_ASYNC_SUSTAIN)
					set_rgb_led(LED_CYCLE, c_RED);

				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				break;

			case (SET_CYCLEJACK_FUNCTION):
				if (settings.cycle_jack_behavior == CYCLE_JACK_RISING_EDGE_TOGGLES)
					set_rgb_led(LED_CYCLE, (now & 0x1000) ? c_RED : c_GREEN);
				else
					set_rgb_led(LED_CYCLE, (now & 0x2000) ? c_ORANGE : c_OFF);

				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_ENVA, c_OFF);
				set_rgb_led(LED_ENVB, c_OFF);
				set_led_brightness(0, PWM_EOF_LED);
				break;
		}

		if (digin[CYCLE_BUTTON].edge == -1) {
			digin[CYCLE_BUTTON].edge = 0;
			HAL_Delay(50); //to de-noise the cycle button

			switch (system_mode_cur) {
				case (SET_TRIGOUT_FUNC):
					settings.trigout_function = static_cast<TrigOutFunctions>(settings.trigout_function + 1);
					if (settings.trigout_function >= NUM_TRIGOUT_FUNCTIONS)
						settings.trigout_function = TRIGOUT_IS_ENDOFRISE;
					break;

				case (SET_TRIGOUT_IS_TRIG):
					settings.trigout_is_trig = settings.trigout_is_trig ? 0 : 1;
					break;

				case (SET_LIMIT_SKEW):
					settings.limit_skew = settings.limit_skew ? 0 : 1;
					break;

				case (SET_FREE_RUNNING_PING):
					settings.free_running_ping = settings.free_running_ping ? 0 : 1;
					break;

				case (SET_TRIGIN_FUNCTION):
					settings.trigin_function = static_cast<TrigInFunctions>(settings.trigin_function + 1);
					if (settings.trigin_function >= NUM_TRIGIN_FUNCTIONS)
						settings.trigin_function = TRIGIN_IS_ASYNC;
					break;

				case (SET_CYCLEJACK_FUNCTION):
					settings.cycle_jack_behavior = static_cast<CycleJackBehaviors>(settings.cycle_jack_behavior + 1);
					if (settings.cycle_jack_behavior >= NUM_CYCLEJACK_BEHAVIORS)
						settings.cycle_jack_behavior = CYCLE_JACK_RISING_EDGE_TOGGLES;
					break;
				default:
					break;
			}
		}
	}

	write_settings();

	while (digin[CYCLE_BUTTON].state || digin[PING_BUTTON].state) {
		set_rgb_led(LED_PING, c_WHITE);
		set_rgb_led(LED_CYCLE, c_WHITE);
		set_rgb_led(LED_ENVA, c_PURPLE);
		set_rgb_led(LED_ENVB, c_PURPLE);
		set_led_brightness(1024, PWM_EOF_LED);

		HAL_Delay(50);

		set_rgb_led(LED_PING, c_OFF);
		set_rgb_led(LED_CYCLE, c_OFF);
		set_rgb_led(LED_ENVA, c_OFF);
		set_rgb_led(LED_ENVB, c_OFF);
		set_led_brightness(0, PWM_EOF_LED);

		HAL_Delay(50);
	}
	digin[CYCLE_BUTTON].edge = 0;
	digin[PING_BUTTON].edge = 0;
}