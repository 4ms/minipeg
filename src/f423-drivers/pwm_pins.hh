#include "pwm.h"

constexpr inline uint32_t kTimPeriod = 4096;

PWMOutput pwm[NUM_PWMS] = {
	{GPIOD, GPIO_PIN_15, GPIO_AF2_TIM4, {.Instance = TIM4}, 4, kTimPeriod}, // PWM_ENVA_R,
	{GPIOA, GPIO_PIN_15, GPIO_AF1_TIM2, {.Instance = TIM2}, 1, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, GPIO_PIN_8, GPIO_AF2_TIM4, {.Instance = TIM4}, 3, kTimPeriod},	// PWM_ENVA_B,
	{GPIOB, GPIO_PIN_11, GPIO_AF1_TIM2, {.Instance = TIM2}, 4, kTimPeriod}, // PWM_ENVB_R,
	{GPIOE, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, 2, kTimPeriod}, // PWM_ENVB_G,
	{GPIOB, GPIO_PIN_4, GPIO_AF2_TIM3, {.Instance = TIM3}, 1, kTimPeriod},	// PWM_ENVB_B,
	{GPIOC, GPIO_PIN_9, GPIO_AF2_TIM3, {.Instance = TIM3}, 4, kTimPeriod},	// PWM_CYCLEBUT_R,
	{GPIOA, GPIO_PIN_10, GPIO_AF1_TIM1, {.Instance = TIM1}, 3, kTimPeriod}, // PWM_CYCLEBUT_G,
	{GPIOA, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, 4, kTimPeriod}, // PWM_CYCLEBUT_B,
	{GPIOB, GPIO_PIN_5, GPIO_AF2_TIM3, {.Instance = TIM3}, 2, kTimPeriod},	// PWM_PINGBUT_R,
	{GPIOC, GPIO_PIN_8, GPIO_AF2_TIM3, {.Instance = TIM3}, 3, kTimPeriod},	// PWM_PINGBUT_G,
	{GPIOD, GPIO_PIN_13, GPIO_AF2_TIM4, {.Instance = TIM4}, 2, kTimPeriod}, // PWM_PINGBUT_B,
	{GPIOE, GPIO_PIN_9, GPIO_AF1_TIM1, {.Instance = TIM1}, 1, kTimPeriod},	// PWM_EOF_LED,
};
