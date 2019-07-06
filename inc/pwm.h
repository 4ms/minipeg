/*
 * pwm.h
 */

#pragma once

#include <stm32f0xx.h>

enum PwmOutputs {
	PWM_ENV, 		//TIM3-CH3	controls TRIGOUTLED
	PWM_ENVLED, 	//TIM3-CH2 init turns on led
	PWM_5VENV, 		//TIM16-CH1 init makes jack go high
	PWM_TRIGOUTLED, //TIM3-CH4 init turns on led
	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G, //TIM3-CH1 init turns on led
	PWM_CYCLEBUT_B,
	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	NUM_PWMS
};

void init_pwm(void);
void update_pwm(uint32_t pwmval, uint8_t channel);
