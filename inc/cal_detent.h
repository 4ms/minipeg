/*
 * cal_detent.h
 */

#pragma once

#include <stm32f0xx.h>

void default_calibration(void);
uint8_t sanity_check_calibration(void);
void calibrate_divmult_pot(void);
uint8_t check_calibration_mode(void);