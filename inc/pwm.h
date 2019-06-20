/*
 * pwm.h
 */

#pragma once

#include <stm32f0xx.h>

enum PwmOutputs {
	PWM_ENV,
	PWM_ENVLED,
	PWM_5VENV,
	PWM_TRIGOUTLED,
	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G,
	PWM_CYCLEBUT_B,
	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	NUM_PWMS
};

void init_pwm(void);
void update_pwm(uint32_t pwmval, uint8_t channel);
