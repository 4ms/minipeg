/*
 * adc.h - adc setup
 */

#pragma once

#include <stm32f0xx.h>

#define MIN_ADC_CHANGE 10

void init_adc(uint16_t *ADC_Buffer);
