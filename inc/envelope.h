
#pragma once
#include <stm32f0xx.h>

int8_t get_clk_div_nominal(uint8_t adc_val);
uint32_t get_clk_div_time(int8_t clock_divide_amount, uint32_t clk_time);
uint32_t get_fall_time(uint8_t skew, uint32_t div_clk_time);
int16_t calc_curve(int16_t t_dacout, char cur_curve);

