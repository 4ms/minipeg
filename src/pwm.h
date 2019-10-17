/*
 * pwm.h
 */

#pragma once

#include <stm32g0xx.h>

enum PwmOutputs {
	PWM_ENV,
	PWM_ENVLED_B,	
	PWM_ENVLED_R,	
	PWM_ENVLED_G,	

	PWM_5VENV,
	PWM_5VENVLED_B,
	PWM_5VENVLED_R,
	PWM_5VENVLED_G,

	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G,

	PWM_EOF_LED,

	PWM_PINGBUT_R,
	PWM_PINGBUT_B,

	NUM_PWMS
};
//PINGBUT_G_EN
//CYCLE_BUT_B

void init_pwm(void);
void update_pwm(uint32_t pwmval, enum PwmOutputs channel);
