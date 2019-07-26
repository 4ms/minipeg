#include "globals.h"

extern struct SystemSettings settings;
extern analog_t analog[NUM_ADCS];

//Private:
uint8_t sanity_check_calibration(void);
void calibrate_divmult_pot(void);
uint8_t should_enter_calibration_mode(void);


void check_calibration(void)
{
	if (!sanity_check_calibration())
		default_calibration();

	if (should_enter_calibration_mode())
	{
		calibrate_divmult_pot();
		write_settings();
	}
}


void default_calibration(void)
{
	settings.midpt_array[0] = 68;
	settings.midpt_array[1] = 262;
	settings.midpt_array[2] = 509;
	settings.midpt_array[3] = 743;
	settings.midpt_array[4] = 973;
	settings.midpt_array[5] = 1202;
	settings.midpt_array[6] = 1427;
	settings.midpt_array[7] = 1657;
	settings.midpt_array[8] = 1882;
	settings.midpt_array[9] = 2107;
	settings.midpt_array[10] = 2341;
	settings.midpt_array[11] = 2574;
	settings.midpt_array[12] = 2802;
	settings.midpt_array[13] = 3026;
	settings.midpt_array[14] = 3262;
	settings.midpt_array[15] = 3500;
	settings.midpt_array[16] = 3734;
	settings.midpt_array[17] = 3942;
	settings.midpt_array[18] = 4095;

	// for(uint8_t i=0; i<NUM_DIVMULTS; i++)
	// {
	// 	settings.midpt_array[i] = (i+1)*(4095/NUM_DIVMULTS);
	// }
}

uint8_t sanity_check_calibration(void)
{
	for (uint8_t j = 0; j < (NUM_DIVMULTS-1); j++ ) {
		if (settings.midpt_array[j+1] <= settings.midpt_array[j])
			return(0); //fail
	}	
	return(1); //pass
}

uint8_t should_enter_calibration_mode(void)
{
	if ((analog[0].lpf_val < 5) && CYCLEBUT
		&& (analog[3].lpf_val>4000) && (analog[4].lpf_val>4000) && (analog[5].lpf_val>4000))
		return 1;
	else
		return 0;
}


void calibrate_divmult_pot(void)
{
	uint16_t stab_delay=30;
	uint16_t calib_array[NUM_DIVMULTS];
	uint16_t read_tot;
	uint16_t read_avg;
	uint16_t read1, read2, read3, read4;
	uint8_t j;
	uint16_t t;
	uint16_t diff;

	set_rgb_led(LED_PING, c_OFF);

	set_rgb_led(LED_CYCLE, c_RED);
	t = 0;
	while (t<100) {if (CYCLEBUT) t++; else t=0;}
	t = 0;
	while (t<100) {if (!CYCLEBUT) t++; else t=0;}
	set_rgb_led(LED_CYCLE, c_OFF);

	for ( j = 0; j < NUM_DIVMULTS; j++ ) {
		set_rgb_led(LED_CYCLE, c_OFF); //off = reading pot 
  

		delay_ticks(stab_delay);
		read1 = analog[0].lpf_val;
		delay_ticks(stab_delay);
		read2 = analog[0].lpf_val;
		delay_ticks(stab_delay);
		read3 = analog[0].lpf_val;
		delay_ticks(stab_delay);
		read4 = analog[0].lpf_val;

		read_tot = read1 + read2 + read3 + read4;
		read_avg = read_tot>>2;	
		
		calib_array[j] = read_avg;

		if (j<(NUM_DIVMULTS-1)){		
			set_rgb_led(LED_CYCLE, c_BLUE); //blue = ready for user to change knob

			if (j==0 || j==(NUM_DIVMULTS-2)) diff=80;
			else diff=160;

			delay_ticks(200);

			//wait until knob is detected as being moved
			do {   
				delay_ticks(stab_delay);
				read1 = analog[0].lpf_val;
				delay_ticks(stab_delay);
				read2 = analog[0].lpf_val;
				delay_ticks(stab_delay);
				read3 = analog[0].lpf_val;
				delay_ticks(stab_delay);
				read4 = analog[0].lpf_val;

				read_tot = read1 + read2 + read3 + read4;
				read_avg = read_tot >> 2;	
			} while ((read_avg - calib_array[j]) < diff);

			set_rgb_led(LED_CYCLE, c_GREEN); //green = press cycle button

			t = 0;
			while (t<100) {if (CYCLEBUT) t++; else t=0;}
			t = 0;
			while (t<100) {if (!CYCLEBUT) t++; else t=0;}
			set_rgb_led(LED_CYCLE, c_OFF);
		}
	}

	//convert the calib_array values to mid-points
	for(j=0;j<(NUM_DIVMULTS-1);j++){
		settings.midpt_array[j] = (calib_array[j] + calib_array[j+1]) >> 1;
	}
	settings.midpt_array[NUM_DIVMULTS-1] = 4095;

}