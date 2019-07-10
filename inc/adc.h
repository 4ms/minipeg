/*
 * adc.h - adc setup
 */

#pragma once

#include <stm32f0xx.h>


void init_adc(uint16_t *ADC_Buffer, uint8_t adc_buffer_size);
