#include "pwm.h"

constexpr inline uint32_t kTimPeriod = 4096;

PWMOutput pwm[NUM_PWMS] = {
	{GPIOD, 15, GPIO_AF2_TIM4, {.Instance = TIM4}, 4, kTimPeriod}, // PWM_ENVA_R,
	{GPIOA, 15, GPIO_AF1_TIM2, {.Instance = TIM2}, 1, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, 8, GPIO_AF2_TIM4, {.Instance = TIM4}, 3, kTimPeriod},  // PWM_ENVA_B,
	{GPIOB, 11, GPIO_AF1_TIM2, {.Instance = TIM2}, 4, kTimPeriod}, // PWM_ENVB_R,
	{GPIOE, 11, GPIO_AF1_TIM1, {.Instance = TIM1}, 2, kTimPeriod}, // PWM_ENVB_G,
	{GPIOB, 4, GPIO_AF2_TIM3, {.Instance = TIM3}, 1, kTimPeriod},  // PWM_ENVB_B,
	{GPIOC, 9, GPIO_AF2_TIM3, {.Instance = TIM3}, 4, kTimPeriod},  // PWM_CYCLEBUT_R,
	{GPIOA, 10, GPIO_AF1_TIM1, {.Instance = TIM1}, 3, kTimPeriod}, // PWM_CYCLEBUT_G,
	{GPIOA, 11, GPIO_AF1_TIM1, {.Instance = TIM1}, 4, kTimPeriod}, // PWM_CYCLEBUT_B,
	{GPIOB, 5, GPIO_AF2_TIM3, {.Instance = TIM3}, 2, kTimPeriod},  // PWM_PINGBUT_R,
	{GPIOC, 8, GPIO_AF2_TIM3, {.Instance = TIM3}, 3, kTimPeriod},  // PWM_PINGBUT_G,
	{GPIOD, 13, GPIO_AF2_TIM4, {.Instance = TIM4}, 2, kTimPeriod}, // PWM_PINGBUT_B,
	{GPIOE, 9, GPIO_AF1_TIM1, {.Instance = TIM1}, 1, kTimPeriod},  // PWM_EOF_LED,
};
