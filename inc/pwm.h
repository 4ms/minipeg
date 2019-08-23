/*
 * pwm.h
 */

#pragma once

#include <stm32f0xx.h>

enum PwmOutputs {
	PWM_ENV,
	PWM_ENVLED_B,	//was PWM_ENVLED
	PWM_ENVLED_R,	//new

	PWM_5VENV,		//
	PWM_5VENVLED_B, //new
	PWM_5VENVLED_R, 	//new

	PWM_CYCLEBUT_R, //
	PWM_CYCLEBUT_BG, //was _B and _G

	PWM_PINGBUT_RG, // was _R and _G
	PWM_PINGBUT_B,

	NUM_PWMS
};

void init_pwm(void);
void update_pwm(uint32_t pwmval, enum PwmOutputs channel);
