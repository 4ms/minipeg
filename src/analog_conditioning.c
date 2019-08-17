#include "globals.h"

extern uint16_t adc_dma_buffer[NUM_ADCS];

analog_t analog[NUM_ADCS];

//Private:
void setup_fir_lpf(void);

void init_analog_conditioning(void)
{
	analog[POT_DIVMULT].polarity = AP_UNIPOLAR;
	analog[POT_SHAPE].polarity = AP_UNIPOLAR;
	analog[POT_OFFSET].polarity = AP_UNIPOLAR;
	analog[POT_SCALE].polarity = AP_UNIPOLAR;
	analog[CV_SHAPE].polarity = AP_BIPOLAR;
	analog[CV_DIVMULT].polarity = AP_BIPOLAR;
	setup_fir_lpf();
}

void setup_fir_lpf(void)
{
	uint8_t analog_id;
	uint16_t initial_value;

	for (analog_id=0; analog_id<NUM_ADCS; analog_id++)
	{
		if (analog[ analog_id ].polarity == AP_BIPOLAR)
			initial_value = 2048;
		else
			initial_value = 0;

		analog[ analog_id ].lpf_sum = initial_value * MAX_LPF_SIZE;
		analog[ analog_id ].lpf_val = initial_value;
	}
}

//todo: make this a system calibration
const int16_t adc_cal_offset[NUM_ADCS] = {0, 48, 48, 0, 0, 0};

//todo: try: new_value += (new_value - old_value) * abs(new_value - old_value) * COEF
//where COEF might be 0.1

void condition_analog(void)
{
	uint8_t i;
	int32_t t;
	static uint8_t oversample_ctr=0;

	for (i=0; i<NUM_ADCS; i++)
	{		
		analog[i].lpf_sum += adc_dma_buffer[i];
	}

	if (++oversample_ctr >= 16)
	{
		oversample_ctr = 0;
		for (i=0; i<NUM_ADCS; i++)
		{
			t = (analog[i].lpf_sum >> 4) + adc_cal_offset[i];
			if (t > 4095) 
				analog[i].lpf_val = 4095;
			else if (t < 0) 
				analog[i].lpf_val = 0;
			else 
				analog[i].lpf_val = t;

			analog[i].lpf_sum = 0;
		}
	}
}
