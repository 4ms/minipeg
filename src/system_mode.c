#include "globals.h"
extern __IO uint32_t systmr;
extern struct SystemSettings settings;
extern debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];

 
void handle_system_mode(void)
{
	static uint32_t entering_system_mode = 0;
	uint8_t d;
	enum SystemModeParams system_mode_cur;
	// uint8_t update_cycle_button_now;

	if (digin[PING_BUTTON].state)
		entering_system_mode++;
	else
		entering_system_mode=0;

	if (entering_system_mode>SYSTEM_MODE_HOLD_TIME) {
		for (d=0;d<5;d++) {
			set_rgb_led(LED_PING, c_WHITE);
			set_rgb_led(LED_CYCLE, c_WHITE);
			set_rgb_led(LED_ENV, c_PURPLE);
			set_rgb_led(LED_5VENV, c_PURPLE);
			LEDTRIGOUT_ON;

			delay_ms(100);
			if (d==4) delay_ms(500);

			set_rgb_led(LED_PING, c_OFF);
			set_rgb_led(LED_CYCLE, c_OFF);
			set_rgb_led(LED_ENV, c_OFF);
			set_rgb_led(LED_5VENV, c_OFF);
			LEDTRIGOUT_OFF;

			delay_ms(100);
		}

		while(digin[PING_BUTTON].state==1){
			;
		}

		delay_ms(50);
		entering_system_mode=0;

		// Setup for the EOF mode
		// here
		system_mode_cur=SET_TRIGOUT_FUNC;
		
		// update_cycle_button_now=1;

		// Loop until we've held down TAPIN more than SYSTEM_MODE_EXIT_TIME cycles
		while (entering_system_mode<SYSTEM_MODE_EXIT_TIME) {

			if (digin[PING_BUTTON].state)
				entering_system_mode++;	
			else
				entering_system_mode = 0;

			// if (digin[PING_BUTTON].edge==1 || update_cycle_button_now) {

				set_rgb_led(LED_PING, c_OFF);
				set_rgb_led(LED_CYCLE, c_OFF);
				set_rgb_led(LED_ENV, c_OFF);
				set_rgb_led(LED_5VENV, c_OFF);
				LEDTRIGOUT_OFF;

				if (digin[PING_BUTTON].edge==1 ) {
					system_mode_cur++;
					digin[PING_BUTTON].edge = 0;
				}

				switch (system_mode_cur) {
			
					case(NUM_SYSMODE_PARAMS):
						system_mode_cur=SET_TRIGOUT_FUNC;
					case(SET_TRIGOUT_FUNC):
						LEDTRIGOUT_ON;
						if (settings.trigout_function == TRIGOUT_IS_ENDOFRISE)
							set_rgb_led(LED_CYCLE, c_RED);
						else if (settings.trigout_function == TRIGOUT_IS_ENDOFFALL)
							set_rgb_led(LED_CYCLE, c_ORANGE);
						else if (settings.trigout_function == TRIGOUT_IS_HALFRISE)
							set_rgb_led(LED_CYCLE, c_CYAN);
						else if (settings.trigout_function == TRIGOUT_IS_TAPCLKOUT)
							set_rgb_led(LED_CYCLE, c_WHITE);
					break;

					case(SET_TRIGOUT_IS_TRIG):
						if (settings.trigout_is_trig) {
							set_rgb_led(LED_CYCLE, c_WHITE);
							if (systmr & 0x0F00) LEDTRIGOUT_OFF;
							else LEDTRIGOUT_ON;
						} else {
							set_rgb_led(LED_CYCLE, c_ORANGE);
							if (systmr & 0x1000) LEDTRIGOUT_OFF;
							else LEDTRIGOUT_ON;
						}
					break;

					case(SET_LIMIT_SKEW):
						set_rgb_led(LED_ENV, c_BLUE);
						if (settings.limit_skew)
							set_rgb_led(LED_CYCLE, c_WHITE);
						else
							set_rgb_led(LED_CYCLE, c_ORANGE);
					break;
					
					case(SET_FREE_RUNNING_PING):
						if (settings.free_running_ping) {
							set_rgb_led(LED_PING, (systmr & 0x1000) ? c_WHITE : c_DIMBLUE);
							set_rgb_led(LED_CYCLE, c_ORANGE);
						} else {
							set_rgb_led(LED_PING, c_DIMBLUE);
							set_rgb_led(LED_CYCLE, c_WHITE);
						}
					break;

					case(SET_TRIGIN_FUNCTION):
						LEDTRIGOUT_ON;
						set_rgb_led(LED_ENV, c_RED);
						if (settings.trigin_function == TRIGIN_IS_QNT)
							set_rgb_led(LED_CYCLE, c_CYAN);
						else if (settings.trigin_function == TRIGIN_IS_ASYNC)
							set_rgb_led(LED_CYCLE, c_ORANGE);
						else if (settings.trigin_function == TRIGIN_IS_ASYNC_SUSTAIN)
							set_rgb_led(LED_CYCLE, c_RED);
					break;

					case(SET_CYCLEJACK_FUNCTION):
						if (settings.cycle_jack_behavior==CYCLE_JACK_RISING_EDGE_TOGGLES)
							set_rgb_led(LED_CYCLE, (systmr & 0x1000) ? c_RED : c_CYAN);
						else
							set_rgb_led(LED_CYCLE, (systmr & 0x2000) ? c_ORANGE : c_OFF);
					break;
				}
			// }		

			if (digin[CYCLE_BUTTON].edge==1) {
				digin[CYCLE_BUTTON].edge = 0;
				delay_ms(50); //to de-noise the cycle button


				switch(system_mode_cur) {
					case(SET_TRIGOUT_FUNC):
						if (++settings.trigout_function == NUM_TRIGOUT_FUNCTIONS)
							settings.trigout_function = 0;
						break;

					case(SET_TRIGOUT_IS_TRIG):
						settings.trigout_is_trig = settings.trigout_is_trig ? 0 : 1;
						break;

					case(SET_LIMIT_SKEW):
						settings.limit_skew = settings.limit_skew ? 0 : 1;
						break;

					case(SET_FREE_RUNNING_PING):
						settings.free_running_ping = settings.free_running_ping ? 0 : 1;
						break;
					
					case(SET_TRIGIN_FUNCTION):
						if (++settings.trigin_function == NUM_TRIGIN_FUNCTIONS)
							settings.trigin_function = 0;
						break;

					case(SET_CYCLEJACK_FUNCTION):
						if (++settings.cycle_jack_behavior == NUM_CYCLEJACK_FUNCTIONS)
							settings.cycle_jack_behavior = 0;
						break;

					default:
						break;

				}
				// update_cycle_button_now=1;							


			}
			
		} //while
		
		write_settings();
		entering_system_mode=0;
		
		while(digin[PING_BUTTON].state==1){
			;
		}

	}
}