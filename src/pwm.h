/*
 * pwm.h
 */

#pragma once
#include "drivers/stm32xx.h"
#include <stddef.h>

enum PwmOutputs {
	PWM_ENVA_R,
	PWM_ENVA_G,
	PWM_ENVA_B,

	PWM_ENVB_R,
	PWM_ENVB_G,
	PWM_ENVB_B,

	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G,
	PWM_CYCLEBUT_B,

	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	PWM_EOF_LED,

	PWM_LOCKBUT_R,
	PWM_LOCKBUT_G,
	PWM_LOCKBUT_B,

	NUM_PWMS_LOCKPCB
};

#ifdef LOCK_PCB
#define NUM_PWMS NUM_PWMS_LOCKPCB
#else
#define NUM_PWMS (NUM_PWMS_LOCKPCB - 3)
#endif
//non-PWM:
//PINGBUT_G_EN
//CYCLE_BUT_B

struct PWMOutput {
	GPIO_TypeDef *gpio;
	uint32_t pinnum;
	uint8_t af;
	TIM_HandleTypeDef tim;
	uint8_t timchan;
	uint32_t period;
};

void init_pwm(void);
void update_pwm(uint32_t pwmval, enum PwmOutputs channel);
