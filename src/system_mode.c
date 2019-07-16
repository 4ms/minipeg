#include "globals.h"


/*
void read_system_settings(void)
{
	temp_u8=eeprom_read_byte((const uint8_t *)(TAPCLK_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		EOF_IS_TAPCLKOUT=1;
	else
		EOF_IS_TAPCLKOUT=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(PING_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		NO_FREERUNNING_PING=1;
	else
		NO_FREERUNNING_PING=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(HALFRISE_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		EOR_IS_HALFRISE=1;
	else
	if (temp_u8==EEPROM_UNPROGRAMMED && BLUE_DETECT) {//a new chip with the blue jumper should default to HALFRISE mode
		EOR_IS_HALFRISE=1; 
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(HALFRISE_EEPROMADDR),EEPROM_SET);
		eeprom_busy_wait();
	}
	else EOR_IS_HALFRISE=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(EO1_TRIG_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		EO1_IS_TRIG=1;
	else
		EO1_IS_TRIG=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(EO2_TRIG_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		EO2_IS_TRIG=1;
	else
		EO2_IS_TRIG=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(LIMIT_SKEW_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		LIMIT_SKEW=1;
	else
		LIMIT_SKEW=0;

	temp_u8=eeprom_read_byte((const uint8_t *)(ASYNC_SUSTAIN_EEPROMADDR));
	if (temp_u8==EEPROM_SET)
		ASYNC_CAN_SUSTAIN=0;
	else
		ASYNC_CAN_SUSTAIN=1;

}


void handle_system_mode(void)
{
	if (TAPIN)
		entering_system_mode++;
	else
		entering_system_mode=0;

	if (entering_system_mode>SYSTEM_MODE_HOLD_TIME){
		for(d=0;d<5;d++){
			EO1_ON;EO2_ON;
			CYCLE_LED_ON;PINGLEDHIGH;
			output_dac(0x0800);
			_delay_ms(100);
			if (d==4) _delay_ms(500);

			EO1_OFF;EO2_OFF;
			CYCLE_LED_OFF;PINGLEDLOW;
			output_dac(0);
			_delay_ms(100);
		}

		while(TAPIN){
		}

		_delay_ms(50);
		entering_system_mode=0;
		initial_cycle_button_state=CYCLE_BUT_RAW;

		// Setup for the EOF mode
		system_mode_cur=0;
		EO2_ON;
		if (EOF_IS_TAPCLKOUT) 	CYCLE_LED_ON;
		else 					CYCLE_LED_OFF;
		
		update_cycle_button_now=1;

		// Loop until we've held down TAPIN more than SYSTEM_MODE_EXIT_TIME cycles
		while(entering_system_mode<SYSTEM_MODE_EXIT_TIME){

			if (TAPIN || update_cycle_button_now) {
				entering_system_mode++;	
					
				if (!tapin_down){
					flash_cycle_led=0;
					tapin_down=1;

					initial_cycle_button_state=CYCLE_BUT_RAW;

					EO1_OFF;
					EO2_OFF;
					PINGLEDLOW;
					output_dac(0);
					
					if (!update_cycle_button_now) system_mode_cur++;
					else update_cycle_button_now=0;

					switch (system_mode_cur){
				
						case(5):
							system_mode_cur=0;
						case(0):
							EO2_ON;d=EO2_IS_TRIG;

							if (!EOF_IS_TAPCLKOUT && !d) 	CYCLE_LED_OFF;
							else if (!EOF_IS_TAPCLKOUT && d) flash_cycle_led=1;
							else if (EOF_IS_TAPCLKOUT && !d)	CYCLE_LED_ON;
							else flash_cycle_led=2;
						break;

						case(1):
							EO1_ON;d=EO1_IS_TRIG;

							if (!EOR_IS_HALFRISE && !d) 	CYCLE_LED_OFF;
							else if (!EOR_IS_HALFRISE && d) flash_cycle_led=1;
							else if (EOR_IS_HALFRISE && !d)	CYCLE_LED_ON;
							else flash_cycle_led=2;
						break;

						case(2):
							output_dac(0x0FFF);
							if (LIMIT_SKEW)	CYCLE_LED_ON;
							else					CYCLE_LED_OFF;
						break;
						
						case(3):
							PINGLEDHIGH;
							if (NO_FREERUNNING_PING) 	CYCLE_LED_ON;
							else 						CYCLE_LED_OFF;
						break;

						case(4):
							EO1_ON;EO2_ON;
							if (ASYNC_CAN_SUSTAIN) CYCLE_LED_OFF;
							else	CYCLE_LED_ON;
						break;


					}//switch system_mode_cur
				}//if tapin_down

			} else { //if TAPIN
				tapin_down=0;
				entering_system_mode=0;
			}		
			
			// Flash the cycle button if necessary 
			if (flash_cycle_led==1){
				temp_u16++;
				if (temp_u16<SYSTEM_MODE_CYCLE_FLASH_DIM_ON) CYCLE_LED_ON;
				else if (temp_u16<SYSTEM_MODE_CYCLE_FLASH_DIM_OFF) CYCLE_LED_OFF;
				else temp_u16=0;
			}
			if (flash_cycle_led==2){
				temp_u16++;
				if (temp_u16<SYSTEM_MODE_CYCLE_FLASH_BRIGHT_ON) CYCLE_LED_ON;
				else if (temp_u16<SYSTEM_MODE_CYCLE_FLASH_BRIGHT_OFF) CYCLE_LED_OFF;
				else temp_u16=0;
			}

			// Check to see if we've changed states with the Cycle button...
			temp_u8=CYCLE_BUT_RAW;

			if(initial_cycle_button_state!=temp_u8){
				_delay_ms(50); //to de-noise the cycle button

				flash_cycle_led=0;
				initial_cycle_button_state=temp_u8;

				switch(system_mode_cur){
					case(0):
						EO2_IS_TRIG=1-EO2_IS_TRIG;
						if (!EO2_IS_TRIG) EOF_IS_TAPCLKOUT=1-EOF_IS_TAPCLKOUT;
					break;

					case(1):
						EO1_IS_TRIG=1-EO1_IS_TRIG;
						if (!EO1_IS_TRIG) EOR_IS_HALFRISE=1-EOR_IS_HALFRISE;
					break;

					case(2):
						LIMIT_SKEW=1-LIMIT_SKEW;
					break;

					case(3):
						NO_FREERUNNING_PING=1-NO_FREERUNNING_PING;
					break;

					
					case(4):
						ASYNC_CAN_SUSTAIN=1-ASYNC_CAN_SUSTAIN;
					break;
				}
				update_cycle_button_now=1;							


			}
			
		} //while
		

		// write_eeprom_setting(NO_FREERUNNING_PING, PING_EEPROMADDR);
		// write_eeprom_setting(EOR_IS_HALFRISE, HALFRISE_EEPROMADDR);
		// write_eeprom_setting(EOF_IS_TAPCLKOUT, TAPCLK_EEPROMADDR);
		// write_eeprom_setting(EO2_IS_TRIG, EO2_TRIG_EEPROMADDR);
		// write_eeprom_setting(EO1_IS_TRIG, EO1_TRIG_EEPROMADDR);


			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

		if (EOF_IS_TAPCLKOUT) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(TAPCLK_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

		if (NO_FREERUNNING_PING) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(PING_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

		if (EOR_IS_HALFRISE) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(HALFRISE_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);


	 	if (LIMIT_SKEW) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(LIMIT_SKEW_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

	 	if (EO1_IS_TRIG) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(EO1_TRIG_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

	 	if (EO2_IS_TRIG) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(EO2_TRIG_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

			EO1_OFF;EO2_OFF;CYCLE_LED_OFF;PINGLEDLOW;output_dac(0);
			_delay_ms(50);
			EO1_ON;EO2_ON;CYCLE_LED_ON;PINGLEDHIGH;output_dac(0x0800);
			_delay_ms(50);

	 	if (!ASYNC_CAN_SUSTAIN) temp_u8=EEPROM_SET;else temp_u8=EEPROM_CLEAR;
		eeprom_busy_wait();
		eeprom_write_byte ((uint8_t *)(ASYNC_SUSTAIN_EEPROMADDR),temp_u8);
		eeprom_busy_wait();

		eof_on();
		eor_off();
		hr_off();
		tapclkout_off();
		if (CYCLE_BUT) CYCLE_LED_ON;
		else CYCLE_LED_OFF;

		entering_system_mode=0;
		cycle_down=0;
	}

}
*/