/*
 * analog_conditioning.h
 */

#pragma once

#include <stm32f0xx.h>


enum AdcChannels
{
	POT_DIVMULT,
	CV_SHAPE,
	CV_DIVMULT,
	POT_SHAPE,
	POT_OFFSET,
	POT_SCALE,

	NUM_ADCS
};

enum AnalogPolarity{
	AP_UNIPOLAR,
	AP_BIPOLAR
};

#define MAX_LPF_SIZE 16

typedef struct AnalogConditioned {
	// uint8_t lpf_size;
	// uint8_t lpf_size_shift;
	// uint8_t lpf_i;
	uint32_t lpf_sum;
	//uint16_t lpf[MAX_LPF_SIZE];

	// uint8_t bracket_size;

	enum AnalogPolarity polarity;

	uint16_t raw_val;
	uint16_t lpf_val;
	// uint16_t bracketed_val;

} analog_t;


void condition_analog(void);
void init_analog_conditioning(void);
