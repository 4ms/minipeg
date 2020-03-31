/*
 * pwm.h
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stm32g4xx.h>

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

	PWM_LOCKBUT_R,
	PWM_LOCKBUT_G,
	PWM_LOCKBUT_B,

	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	PWM_EOF_LED,

	NUM_PWMS
};
//non-PWM:
//PINGBUT_G_EN
//CYCLE_BUT_B

void init_pwm(void);
void update_pwm(uint32_t pwmval, enum PwmOutputs channel);

#ifdef __cplusplus
}
#endif
