#pragma once
#include "pwm.h"

constexpr inline uint32_t kTimPeriod = 4096;

PWMOutput pwm[NUM_PWMS] = {
	{GPIOD, GPIO_PIN_15, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_4, kTimPeriod}, // PWM_ENVA_R,
	{GPIOA, GPIO_PIN_15, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_1, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, GPIO_PIN_8, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_3, kTimPeriod},	// PWM_ENVA_B,
	{GPIOB, GPIO_PIN_11, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_4, kTimPeriod}, // PWM_ENVB_R,
	{GPIOE, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_2, kTimPeriod}, // PWM_ENVB_G,
	{GPIOB, GPIO_PIN_4, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_1, kTimPeriod},	// PWM_ENVB_B,
	{GPIOC, GPIO_PIN_9, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_4, kTimPeriod},	// PWM_CYCLEBUT_R,
	{GPIOA, GPIO_PIN_10, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_3, kTimPeriod}, // PWM_CYCLEBUT_G,
	{GPIOA, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_4, kTimPeriod}, // PWM_CYCLEBUT_B,
	{GPIOB, GPIO_PIN_5, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_2, kTimPeriod},	// PWM_PINGBUT_R,
	{GPIOC, GPIO_PIN_8, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_3, kTimPeriod},	// PWM_PINGBUT_G,
	{GPIOD, GPIO_PIN_13, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_2, kTimPeriod}, // PWM_PINGBUT_B,
	{GPIOE, GPIO_PIN_9, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_1, kTimPeriod},	// PWM_EOF_LED,
};
